#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_client.h>

MBASE_BEGIN

#define MBASE_INF_T2T_PROC_RETURN_UNREGISTERED \
if (this->signal_destroying())\
{\
	return flags::INF_PROC_INFO_DESTROYING;\
}\
if(this->signal_initializing())\
{\
	return flags::INF_PROC_INFO_INITIALIZING;\
}\
if(!this->is_registered())\
{\
	return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;\
}

#define MBASE_INF_T2T_PROC_RETURN_HALTED \
if(!this->is_registered())\
{\
	return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;\
}\
if(!this->is_running())\
{\
	return flags::INF_PROC_ERR_HALTED;\
}

InfTextToTextProcessor::InfTextToTextProcessor():
	mSamplerChain(NULL),
	mModelContext(NULL),
	mPresetCandidates(),
	mContextCursor(0),
	mBatchSize(0),
	mThreadCount(0),
	mFinishState(finish_state::FINISHED),
	mLastFailCode(last_fail_code::MODEL_NOT_INITIALIZED),
	mFlashAttention(false),
	mIsInitializeFailed(false)
{
	mModelCategory = inf_model_category::TEXT_TO_TEXT;
}

InfTextToTextProcessor::~InfTextToTextProcessor()
{
	if(mModelContext)
	{
		stop_processor();
		llama_free(mModelContext);
		this->release_object_watcher();

		if(mAssignedClient)
		{
			mAssignedClient->on_unregister(this);
			mAssignedClient = NULL;
		}
	}
}

InfTextToTextProcessor::last_fail_code InfTextToTextProcessor::get_last_fail_code() const
{
	return mLastFailCode;
}

bool InfTextToTextProcessor::is_init_failed() const
{
	return mIsInitializeFailed;
}

bool InfTextToTextProcessor::is_available() const
{
	if (signal_input_process() || signal_decode_process() || signal_state_decode_process() || signal_state_input_process())
	{
		return false;
	}

	return true;
}

bool InfTextToTextProcessor::signal_state_input_process() const
{
	return mInputSignal.get_signal_state();
}

bool InfTextToTextProcessor::signal_state_decode_process() const
{
	return mDecodeSignal.get_signal_state();
}

bool InfTextToTextProcessor::signal_input_process() const
{
	return mInputSignal.get_signal();
}

bool InfTextToTextProcessor::signal_decode_process() const
{
	return mDecodeSignal.get_signal();
}

U32 InfTextToTextProcessor::get_max_token_length()
{
	return mContextLength;
}

bool InfTextToTextProcessor::has_sampler(InfSamplerDescription::SAMPLER in_sampler_type, InfSamplerDescription& out_sampler)
{
	for(inf_sampling_set::iterator It = mSamplerDescriptions.begin(); It != mSamplerDescriptions.end(); ++It)
	{
		InfSamplerDescription smpBase = *It;
		if(smpBase.mSamplerType == in_sampler_type)
		{
			out_sampler = smpBase;
			return true;
		}
	}
	return false;
}

GENERIC InfTextToTextProcessor::get_available_samplers(inf_sampling_set& out_samplers)
{
	out_samplers = mSamplerDescriptions;
}

bool InfTextToTextProcessor::has_client() const
{
	return mAssignedClient != NULL;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::get_processor_status() const
{
	if(signal_initializing())
	{
		return flags::INF_PROC_INFO_INITIALIZING;
	}

	if(signal_destroying())
	{
		return flags::INF_PROC_INFO_DESTROYING;
	}

	if(signal_input_process())
	{
		return flags::INF_PROC_INFO_INITIAL_INPUT_PROCESSING;
	}

	if(signal_decode_process())
	{
		return flags::INF_PROC_INFO_DECODING;
	}

	return flags::INF_PROC_INFO_NEED_UPDATE;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::token_to_description(const inf_text_token& in_token, inf_token_description& out_description)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;
	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);

	IBYTE tokenString[64] = {0};
	I32 tokenLength = llama_token_to_piece(t2tModel->get_raw_model(), in_token, tokenString, 64, false, true);

	if(!tokenLength)
	{
		// UNKNOWN ERROR
		// GO BACK HERE LATER
	}

	out_description.mTokenString = std::move(mbase::string(tokenString, tokenLength));
	
	if(t2tModel->is_token_control(in_token) == InfModelTextToText::flags::INF_MODEL_SUCCESS)
	{
		out_description.mIsSpecial = true;
	}

	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::tokens_to_description_vector(const mbase::vector<inf_text_token>& in_tokens, mbase::vector<inf_token_description>& out_descriptions)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;
	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);

	for(mbase::vector<inf_text_token>::const_iterator cIt = in_tokens.cbegin(); cIt != in_tokens.cend(); ++cIt)
	{
		const inf_text_token& cvTokenRef = *cIt;
		IBYTE tokenString[64] = {0};
		I32 tokenLength = llama_token_to_piece(t2tModel->get_raw_model(), cvTokenRef, tokenString, 64, false, true);
		if(t2tModel->is_token_control(cvTokenRef) == InfModelTextToText::flags::INF_MODEL_SUCCESS)
		{
			out_descriptions.push_back({std::move(mbase::string(tokenString, tokenLength)), true});
			continue;
		}
		out_descriptions.push_back({std::move(mbase::string(tokenString, tokenLength)), false});
	}

	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;
	
	if(!in_size)
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}
	inf_text_token_vector tokenizedInput(in_size * 4);
	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);
	I32 tokenCount = llama_tokenize(t2tModel->get_raw_model(), in_data, in_size, tokenizedInput.data(), in_size * 4, false, true);
	if(tokenCount == -1)
	{
		return flags::INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT;
	}

	tokenizedInput.resize(tokenCount);
	out_tokens = std::move(tokenizedInput);
	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::tokenize_input(context_line* in_lines, size_type in_count, inf_text_token_vector& out_tokens, bool in_append_assistant_token)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;
	
	if(!in_lines || !in_count)
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}

	mbase::string totalMessage;
	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);

	for(size_type i = 0; i < in_count; ++i)
	{
		context_line* tmpLine = in_lines + i;
		mbase::string roleString;
		mbase::string endString;
		if(tmpLine->mRole == context_role::SYSTEM)
		{
			t2tModel->get_sys_start(roleString);
			t2tModel->get_sys_end(endString);
		}

		else if(tmpLine->mRole == context_role::ASSISTANT)
		{
			t2tModel->get_assistant_start(roleString);
			t2tModel->get_assistant_end(endString);
		}

		else if(tmpLine->mRole == context_role::USER)
		{
			t2tModel->get_usr_start(roleString);
			t2tModel->get_usr_end(endString);
		}
		
		totalMessage += (roleString + tmpLine->mMessage + endString);
	}
	
	if(totalMessage.size())
	{
		context_line* lastLine = in_lines + (in_count - 1);
		if(lastLine->mRole == context_role::USER)
		{
			if (in_append_assistant_token)
			{
				mbase::string assistantToken;
				t2tModel->get_assistant_start(assistantToken);
				totalMessage += assistantToken;
			}
		}
	}
	return tokenize_input(totalMessage.data(), totalMessage.size(), out_tokens);
}

InfTextToTextProcessor::flags InfTextToTextProcessor::execute_input(const inf_text_token_vector& in_tokens, bool in_abandon)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;

	if(!in_tokens.size())
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}

	if(in_tokens.size() > mBatchSize)
	{
		return flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT;
	}

	if (!is_running())
	{
		return flags::INF_PROC_INFO_HALTED;
	}

	if(!in_abandon)
	{
		if(!is_available())
		{
			return flags::INF_PROC_ERR_ALREADY_PROCESSING;
		}
	}

	else
	{
		mInputSignal.reset_signal_with_state();
		mDecodeSignal.reset_signal_with_state();
		stop_processor();
	}

	mTokenizedInput = in_tokens;
	mInputSignal.set_signal();
	start_processor();

	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::next(const decode_behavior_description& in_description)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;

	if(signal_decode_process())
	{
		// If it is already processing, ignore the request
		return flags::INF_PROC_SUCCESS;
	}

	if(signal_state_decode_process())
	{
		return flags::INF_PROC_ERR_ALREADY_PROCESSING;
	}

	if(!is_running())
	{
		return flags::INF_PROC_INFO_HALTED;
	}

	start_processor();
	mGeneratedTokenVector.clear();
	mDecodeBehavior = in_description;
	mDecodeSignal.set_signal();
	
	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::next_sync(const decode_behavior_description& in_description)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;

	while(signal_input_process()){}

	return next(in_description);
}

InfTextToTextProcessor::flags InfTextToTextProcessor::set_inference_client(InfClientBase* in_client)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;

	if(get_assigned_client())
	{
		return flags::INF_PROC_ERR_ALREADY_PROCESSING;
	}

	if(!in_client)
	{
		return flags::INF_PROC_ERR_MISSING_CLIENT;
	}
	
	if(mAssignedClient == in_client)
	{
		return flags::INF_PROC_SUCCESS;
	}

	release_inference_client();

	mAssignedClient = in_client;
	mAssignedClient->on_register(this);

	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::initialize(
	InfModelTextToText* in_model, 
	const U32& in_context_length, 
	const mbase::string& in_context_id,
	const U32& in_batch_size,
	const U32& in_thread_count,
	const bool& in_flash_attention,
	const inf_sampling_set& in_sampler_set
)
{
	if (signal_initializing())
	{
		return flags::INF_PROC_INFO_INITIALIZING;
	}

	if (signal_destroying())
	{
		return flags::INF_PROC_INFO_DESTROYING;
	}

	if(is_registered())
	{
		return flags::INF_PROC_ERR_ALREADY_INITIALIZED;
	}

	mTargetModel_md_model = in_model;
	mContextLength = in_context_length;
	mContextIdentifier = in_context_id;
	mBatchSize = in_batch_size;
	mThreadCount = in_thread_count;
	mSamplerDescriptions = in_sampler_set;
	mFlashAttention = in_flash_attention;

	mInitializeSignal.set_signal();
	on_initializing();
	start_processor();
	return flags::INF_PROC_INFO_INITIALIZING;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::initialize_sync(
	InfModelTextToText* in_model, 
	const U32& in_context_length, 
	const mbase::string& in_context_id,
	const U32& in_batch_size,
	const U32& in_thread_count,
	const bool& in_flash_attention,
	const inf_sampling_set& in_sampler_set
)
{
	initialize(
		in_model, 
		in_context_length, 
		in_context_id,
		in_batch_size,
		in_thread_count,
		in_flash_attention,
		in_sampler_set
	);
	while(signal_initializing())
	{

	}
	
	return flags::INF_PROC_INFO_NEED_UPDATE;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::destroy()
{
	if(!is_registered())
	{
		return flags::INF_PROC_SUCCESS;
	}

	if(signal_destroying())
	{
		return flags::INF_PROC_INFO_DESTROYING;
	}

	release_inference_client();
	on_destroying();

	start_processor();
	mDestroySignal.set_signal();
	return flags::INF_PROC_INFO_DESTROYING;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::destroy_sync()
{
	destroy();
	while(signal_destroying())
	{
		// block until operation finishes
	}

	return flags::INF_PROC_INFO_NEED_UPDATE;
}

GENERIC InfTextToTextProcessor::clear_token_candidates()
{
	mPresetCandidates.clear();
}

GENERIC InfTextToTextProcessor::clear_samplers()
{
	// TODO: Do not allow clearing when processing tokens
	// TODO: Delete the return when the sampler interface is properly working
	
	if(mSamplerChain)
	{
		llama_sampler_free(mSamplerChain);
		mSamplerDescriptions.clear();
		mSamplerChain = NULL;
	}
}

GENERIC InfTextToTextProcessor::on_initializing()
{

}

GENERIC InfTextToTextProcessor::on_initialize_fail([[maybe_unused]] last_fail_code out_code)
{

}

GENERIC InfTextToTextProcessor::on_destroying()
{

}

GENERIC InfTextToTextProcessor::_decode_input()
{
	// if the input signal is set, process
	if(llama_get_kv_cache_used_cells(mModelContext))
	{
		llama_kv_cache_clear(mModelContext);
	}
	
	llama_sampler_reset(mSamplerChain);
	mInputBatch = llama_batch_get_one(mTokenizedInput.data(), mTokenizedInput.size());

	mContextCursor = mInputBatch.n_tokens;
	mTokenizedInput.clear();
	
	[[maybe_unused]] I32 decodeResult = llama_decode(mModelContext, mInputBatch);
	mInputSignal.set_signal_finished();
	mFinishState = finish_state::CONTINUE;
}

GENERIC InfTextToTextProcessor::_decode_next()
{
	// Main Decode loop
	for(U32 i = 0; i < mDecodeBehavior.mTokenAtMost; i++)
	{
		I32 modelVocab = 0;
		InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);
		t2tModel->get_vocab_count(modelVocab);

		PTRF32 logits = llama_get_logits_ith(mModelContext, mInputBatch.n_tokens - 1);
		for (llama_token token_id = 0; token_id < modelVocab; ++token_id)
		{
			mPresetCandidates.emplace_back(llama_token_data{ token_id, logits[token_id], 0.0f });
		}
		llama_token_data_array tokenCandidates = { mPresetCandidates.data(), mPresetCandidates.size(), false};
		llama_sampler_apply(mSamplerChain, &tokenCandidates);
		inf_text_token tmpGeneratedToken = llama_sampler_sample(mSamplerChain, mModelContext, -1);
		
		llama_sampler_accept(mSamplerChain, tmpGeneratedToken);
		mGeneratedTokenVector.push_back(tmpGeneratedToken);
		clear_token_candidates();
		
		if (llama_token_is_eog(t2tModel->get_raw_model(), tmpGeneratedToken))
		{
			// means end of generation
			llama_sampler_reset(mSamplerChain);
			mFinishState = finish_state::FINISHED;
			llama_kv_cache_clear(mModelContext);
			break;
		}

		else
		{
			if (mContextCursor == mContextLength)
			{
				// means token limit is reached
				llama_sampler_reset(mSamplerChain);
				mFinishState = finish_state::TOKEN_LIMIT_REACHED;
				llama_kv_cache_clear(mModelContext);
				break;
			}

			else
			{
				mInputBatch = llama_batch_get_one(&tmpGeneratedToken, 1);
				mContextCursor++;
				llama_decode(mModelContext, mInputBatch); // Handle error here
			}
		}
	}

	mDecodeSignal.set_signal_finished();
}

GENERIC InfTextToTextProcessor::_initialize_context()
{
	llama_context_params ctxParams = llama_context_default_params();
	ctxParams.n_ctx = mContextLength;
	ctxParams.n_batch = mBatchSize;
	ctxParams.n_seq_max = 1;
	ctxParams.n_threads = mThreadCount;
	ctxParams.n_threads_batch = mThreadCount;
	ctxParams.n_ubatch = mBatchSize / 4;
	ctxParams.flash_attn = mFlashAttention;

	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);

	if(!t2tModel || !t2tModel->is_initialized())
	{
		clear_samplers();
		mLastFailCode = last_fail_code::MODEL_NOT_INITIALIZED;
		mIsInitializeFailed = true;
		mInitializeSignal.set_signal_finished();
		return;
	}

	mModelContext = llama_new_context_with_model(t2tModel->get_raw_model(), ctxParams);
	if (!mModelContext)
	{
		clear_samplers();
		mLastFailCode = last_fail_code::NOT_ENOUGH_MEMORY;
		mIsInitializeFailed = true;
		mInitializeSignal.set_signal_finished();
		return;
	}
	
	mContextCursor = 0;
		
	I32 modelVocabCount = 0;
	inf_text_token eotToken = 0;
	inf_text_token nlToken = 0;
	
	t2tModel->get_vocab_count(modelVocabCount);
	t2tModel->get_eot_token(eotToken);
	t2tModel->get_lf_token(nlToken);
	I32 seedValue = 1048204757;

	mSamplerChain = llama_sampler_chain_init(llama_sampler_chain_default_params());

	if(!mSamplerDescriptions.size())
	{
		// If no sampler is present, apply greedy
		llama_sampler_chain_add(mSamplerChain, llama_sampler_init_greedy());
	}

	else
	{
		for(inf_sampling_set::iterator It = mSamplerDescriptions.begin(); It != mSamplerDescriptions.end(); ++It)
		{
			if(It->mSamplerType == InfSamplerDescription::SAMPLER::REPETITION)
			{
				InfSamplingRepetition repeatSampler = It->mRepetition;
				llama_sampler_chain_add(
					mSamplerChain, 
					llama_sampler_init_penalties(
						modelVocabCount, 
						eotToken, 
						nlToken, 
						repeatSampler.mPenaltyN, 
						repeatSampler.mRepeatPenalty, 
						repeatSampler.mPenaltyFrequency, 
						repeatSampler.mPenaltyPresent, 
						repeatSampler.mPenaltyLinefeed, 
						repeatSampler.mPenaltyEos
					)
				);
			}

			else if(It->mSamplerType == InfSamplerDescription::SAMPLER::TOP_K)
			{
				U32 kValue = It->mTopK;
				llama_sampler_chain_add(
					mSamplerChain,
					llama_sampler_init_top_k(kValue)
				);
			}

			else if(It->mSamplerType == InfSamplerDescription::SAMPLER::TOP_P)
			{
				F32 topValue = It->mTopP;
				llama_sampler_chain_add(
					mSamplerChain,
					llama_sampler_init_top_p(topValue, 1)
				);
			}

			else if(It->mSamplerType == InfSamplerDescription::SAMPLER::MIN_P)
			{
				F32 minValue = It->mMinP;
				llama_sampler_chain_add(
					mSamplerChain,
					llama_sampler_init_min_p(minValue, 1)
				);
			}

			else if(It->mSamplerType == InfSamplerDescription::SAMPLER::TYPICAL_P)
			{
				F32 typicalValue = It->mTypicalP;
				llama_sampler_chain_add(
					mSamplerChain,
					llama_sampler_init_typical(typicalValue, 1)
				);
			}

			else if(It->mSamplerType == InfSamplerDescription::SAMPLER::TEMP)
			{
				F32 temperature = It->mTemp;
				llama_sampler_chain_add(
					mSamplerChain,
					llama_sampler_init_temp(temperature)
				);
			}
			else if(It->mSamplerType == InfSamplerDescription::SAMPLER::MIROSTAT_V2)
			{
				InfSamplingMirostatV2 mirostatObject = It->mMiroV2;
				llama_sampler_chain_add(
					mSamplerChain,
					llama_sampler_init_mirostat_v2(
						seedValue,
						mirostatObject.mTau,
						mirostatObject.mEta
					)	
				);
			}
		}

		llama_sampler_chain_add(
			mSamplerChain,
			llama_sampler_init_dist(seedValue)
		);
	}

	mInitializeSignal.set_signal_finished();
}

GENERIC InfTextToTextProcessor::_destroy_context()
{
	// CONTEXT FACTORY RESET

	llama_free(mModelContext);
	mModelContext = NULL;
	mPresetCandidates.clear();
	mTokenizedInput.clear();
	mSamplerDescriptions.clear();
	mGeneratedTokenVector.clear();
	mContextCursor = 0;
	mBatchSize = 0;
	mThreadCount = 0;
	mFlashAttention = false;
	mIsRunning = false;
	mIsInitializeFailed = false;
	mFinishState = finish_state::FINISHED;
	mAssignedClient = NULL;
	mLastFailCode = last_fail_code::MODEL_NOT_INITIALIZED;

	clear_samplers();
	
	mTargetModel_md_model = NULL;
	mIsRegistered = false;
	mDestroySignal.set_signal_finished();
}

GENERIC InfTextToTextProcessor::update()
{
	if(signal_destroying())
	{
		return;
	}
	if(signal_state_destroying())
	{
		// Reset all signals on destruction
		stop_processor();
		reset_base_signals();
		mDecodeSignal.reset_signal_with_state();
		mInputSignal.reset_signal_with_state();
		this->release_object_watcher();
		on_destroy();
	}

	if(signal_state_initializing())
	{
		mInitializeSignal.reset_signal_state();
		stop_processor();
		if(is_init_failed())
		{
			this->release_object_watcher();
			on_initialize_fail(get_last_fail_code());
		}
		else
		{
			mIsRunning = true;
			mIsRegistered = true;
			on_initialize();
		}
	}

	if(signal_state_input_process())
	{
		
	}

	if(signal_state_decode_process())
	{
		mDecodeSignal.reset_signal_state();		
		bool isFinish = false;

		if(mFinishState != finish_state::CONTINUE)
		{
			isFinish = true;
		}

		if(isFinish)
		{
			stop_processor();
			mInputSignal.reset_signal_with_state();
		}

		InfClientTextToText* t2tClient = static_cast<InfClientTextToText*>(get_assigned_client());

		if(t2tClient)
		{
			inf_text_token_vector tokenVector = mGeneratedTokenVector;
			if(mDecodeBehavior.mHaltOnWrite)
			{
				stop_processor();
			}

			t2tClient->on_write(this, tokenVector, isFinish);
			if(isFinish)
			{
				t2tClient->on_finish(this, mContextCursor, mFinishState);
			}
		}
	}
}

GENERIC InfTextToTextProcessor::update_t()
{
	while(is_processor_running())
	{
		if(is_registered())
		{
			if (signal_destroying())
			{
				_destroy_context();
				continue;
			}

			if (is_running())
			{
				if (signal_input_process())
				{
					_decode_input();
					continue;
				}

				if (signal_decode_process())
				{
					_decode_next();
				}
			}
		}
		else
		{
			if(signal_initializing())
			{
				_initialize_context();
			}
			else
			{
				mbase::sleep(15);
			}
		}
	}
}

MBASE_END