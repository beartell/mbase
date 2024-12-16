#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_client.h>
#include <chrono>

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

InfProcessorTextToText::InfProcessorTextToText():
	mSamplerChain(NULL),
	mModelContext(NULL),
	mPresetCandidates(),
	mContextCursor(0),
	mBatchSize(0),
	mThreadCount(0),
	mBatchProcessThreadCount(0),
	mProcessedBatchLength(0),
	mFinishState(finish_state::FINISHED),
	mLastFailCode(last_fail_code::MODEL_NOT_INITIALIZED),
	mFlashAttention(false),
	mIsInitializeFailed(false)
{
	mModelCategory = inf_model_category::TEXT_TO_TEXT;
}

InfProcessorTextToText::~InfProcessorTextToText()
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

InfProcT2TDiagnostics& InfProcessorTextToText::get_diagnostics()
{
	return mDiagnostics;
}

InfProcessorTextToText::last_fail_code InfProcessorTextToText::get_last_fail_code() const
{
	return mLastFailCode;
}

bool InfProcessorTextToText::is_init_failed() const
{
	return mIsInitializeFailed;
}

bool InfProcessorTextToText::is_available() const
{
	if (signal_input_process() || signal_decode_process() || signal_state_decode_process())
	{
		return false;
	}

	return true;
}

bool InfProcessorTextToText::signal_state_input_process() const
{
	return mInputSignal.get_signal_state();
}

bool InfProcessorTextToText::signal_state_decode_process() const
{
	return mDecodeSignal.get_signal_state();
}

bool InfProcessorTextToText::signal_input_process() const
{
	return mInputSignal.get_signal();
}

bool InfProcessorTextToText::signal_decode_process() const
{
	return mDecodeSignal.get_signal();
}

U32 InfProcessorTextToText::get_batch_size()
{
	return mBatchSize;
}

U32 InfProcessorTextToText::get_max_token_length()
{
	return mContextLength;
}

U32 InfProcessorTextToText::get_context_cursor_position()
{
	return mContextCursor;
}

bool InfProcessorTextToText::has_sampler(InfSamplerDescription::SAMPLER in_sampler_type, InfSamplerDescription& out_sampler)
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

GENERIC InfProcessorTextToText::get_available_samplers(inf_sampling_set& out_samplers)
{
	out_samplers = mSamplerDescriptions;
}

InfProcessorTextToText::flags InfProcessorTextToText::get_processor_status() const
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

InfProcessorTextToText::flags InfProcessorTextToText::token_to_description(const inf_text_token& in_token, inf_token_description& out_description)
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

InfProcessorTextToText::flags InfProcessorTextToText::tokens_to_description_vector(const mbase::vector<inf_text_token>& in_tokens, mbase::vector<inf_token_description>& out_descriptions)
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

InfProcessorTextToText::flags InfProcessorTextToText::tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;
	
	if(!in_size)
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}
	inf_text_token_vector tokenizedInput(in_size * 4);
	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);
	try
	{
		I32 tokenCount = llama_tokenize(t2tModel->get_raw_model(), in_data, in_size, tokenizedInput.data(), in_size * 4, false, true);
		if(tokenCount == -1)
		{
			return flags::INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT;
		}

		tokenizedInput.resize(tokenCount);
	}
	catch(const std::exception& e)
	{
		return flags::INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT;
	}
	
	out_tokens = std::move(tokenizedInput);
	return flags::INF_PROC_SUCCESS;
}

InfProcessorTextToText::flags InfProcessorTextToText::tokenize_input(context_line* in_lines, size_type in_count, inf_text_token_vector& out_tokens, bool in_append_assistant_token)
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

InfProcessorTextToText::flags InfProcessorTextToText::execute_input(const inf_text_token_vector& in_tokens, bool in_abandon)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;

	if(!in_tokens.size())
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}

	if(in_tokens.size() > mContextLength)
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
		if(llama_get_kv_cache_used_cells(mModelContext))
		{
			llama_kv_cache_clear(mModelContext);
		}
	}
	stop_processor(); // Stop the processor if it is not stopped already
	mTokenizedInput = in_tokens;
	mInputSignal.set_signal();
	start_processor();

	return flags::INF_PROC_SUCCESS;
}

InfProcessorTextToText::flags InfProcessorTextToText::execute_input(inf_text_token* in_tokens, size_type in_size, bool in_abandon)
{
	inf_text_token_vector tokenVector;
	for(size_type i = 0; i < in_size; i++)
	{
		tokenVector.push_back(in_tokens[i]);
	}

	return execute_input(tokenVector, in_abandon);
}

InfProcessorTextToText::flags InfProcessorTextToText::execute_input_sync(const inf_text_token_vector& in_tokens, bool in_abandon)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;
	flags outResult = execute_input(in_tokens, in_abandon);
	if(outResult == flags::INF_PROC_SUCCESS || outResult == flags::INF_PROC_ERR_ALREADY_PROCESSING)
	{
		while(signal_input_process())
		{
			mbase::sleep(2);
		}
		return flags::INF_PROC_INFO_NEED_UPDATE;
	}

	return outResult;
}

InfProcessorTextToText::flags InfProcessorTextToText::next(const decode_behavior_description& in_description)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;

	if(signal_decode_process())
	{
		// If it is already processing, ignore the request
		return flags::INF_PROC_SUCCESS;
	}

	if(signal_state_decode_process())
	{
		return flags::INF_PROC_INFO_NEED_UPDATE;
	}

	if(!is_running())
	{
		return flags::INF_PROC_INFO_HALTED;
	}

	mGeneratedTokenVector.clear();
	mDecodeBehavior = in_description;
	mDecodeSignal.set_signal();
	start_processor();

	return flags::INF_PROC_SUCCESS;
}

InfProcessorTextToText::flags InfProcessorTextToText::next_sync(const decode_behavior_description& in_description)
{
	MBASE_INF_T2T_PROC_RETURN_UNREGISTERED;

	flags nextResult = next(in_description);
	if(nextResult == flags::INF_PROC_SUCCESS)
	{
		while(signal_decode_process()){ mbase::sleep(2); }
		return flags::INF_PROC_INFO_NEED_UPDATE;
	}

	return nextResult;
}

InfProcessorTextToText::flags InfProcessorTextToText::set_inference_client(InfClientBase* in_client)
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

InfProcessorTextToText::flags InfProcessorTextToText::initialize(
	InfModelTextToText* in_model, 
	const U32& in_context_length, 
	const mbase::string& in_context_id,
	const U32& in_batch_size,
	const U32& in_thread_count,
	const U32& in_batch_thread_count,
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
	mBatchProcessThreadCount = in_batch_thread_count;
	mSamplerDescriptions = in_sampler_set;
	mFlashAttention = in_flash_attention;

	mInitializeSignal.set_signal();
	on_initializing();
	start_processor();
	return flags::INF_PROC_INFO_INITIALIZING;
}

InfProcessorTextToText::flags InfProcessorTextToText::initialize_sync(
	InfModelTextToText* in_model, 
	const U32& in_context_length, 
	const mbase::string& in_context_id,
	const U32& in_batch_size,
	const U32& in_thread_count,
	const U32& in_batch_thread_count,
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
		in_batch_thread_count,
		in_flash_attention,
		in_sampler_set
	);
	while(signal_initializing())
	{
		mbase::sleep(2);
	}
	
	return flags::INF_PROC_INFO_NEED_UPDATE;
}

InfProcessorTextToText::flags InfProcessorTextToText::destroy()
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

	mDestroySignal.set_signal();
	start_processor();
	return flags::INF_PROC_INFO_DESTROYING;
}

InfProcessorTextToText::flags InfProcessorTextToText::destroy_sync()
{
	destroy();
	while(signal_destroying())
	{
		mbase::sleep(2);
	}

	return flags::INF_PROC_INFO_NEED_UPDATE;
}

GENERIC InfProcessorTextToText::clear_token_candidates()
{
	mPresetCandidates.clear();
}

GENERIC InfProcessorTextToText::clear_samplers()
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

GENERIC InfProcessorTextToText::on_initializing()
{

}

GENERIC InfProcessorTextToText::on_initialize_fail([[maybe_unused]] last_fail_code out_code)
{

}

GENERIC InfProcessorTextToText::on_destroying()
{

}

GENERIC InfProcessorTextToText::_decode_input()
{
	llama_kv_cache_clear(mModelContext);
	std::chrono::high_resolution_clock::time_point beginTime;
	std::chrono::high_resolution_clock::time_point endTime;
	mProcessedBatchLength = 0;
	U32 tmpBatchCursor = 0;
	U32 tmpNumOperated = 0;
	I64 msPassed = 0;
	llama_batch tempBatch = llama_batch_init(mBatchSize, 0, 1);
	for(size_type i = 0; i < mTokenizedInput.size() - 1; i++)
	{
		++tmpBatchCursor;
		inf_common_batch_add(tempBatch, mTokenizedInput[i], i, {0}, false);
		if(tmpBatchCursor == mBatchSize)
		{
			beginTime = std::chrono::high_resolution_clock::now();
			llama_decode(mModelContext, tempBatch);
			endTime = std::chrono::high_resolution_clock::now();
			msPassed += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
			tmpNumOperated++;
			tempBatch.n_tokens = 0;
			tmpBatchCursor = 0;
		}
	}

	inf_common_batch_add(tempBatch, mTokenizedInput.back(), mTokenizedInput.size() - 1, {0}, true);
	beginTime = std::chrono::high_resolution_clock::now();
	llama_decode(mModelContext, tempBatch);
	mContextCursor = llama_get_kv_cache_token_count(mModelContext);
	endTime = std::chrono::high_resolution_clock::now();
	msPassed += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
	tmpNumOperated++;
	
	F32 secondsPassed = (F32)msPassed / 1000.0f;
	mDiagnostics.ppTokensPerSecond = mContextCursor / secondsPassed;
	llama_batch_free(tempBatch);
	mInputSignal.set_signal_finished();
	mFinishState = finish_state::CONTINUE;
}

GENERIC InfProcessorTextToText::_decode_next()
{
	// Main Decode loop
	I64 totalMilliseconds = 1;
	I64 totalGeneratedTokens = 0;
	llama_batch tempBatch = llama_batch_init(mDecodeBehavior.mTokenAtMost, 0, 1);
	for(U32 i = 0; i < mDecodeBehavior.mTokenAtMost; i++)
	{
		std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();
		I32 modelVocab = 0;
		InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);
		t2tModel->get_vocab_count(modelVocab);
		
		inf_text_token tmpGeneratedToken = llama_sampler_sample(mSamplerChain, mModelContext, -1);
		
		//llama_sampler_accept(mSamplerChain, tmpGeneratedToken);
		mGeneratedTokenVector.push_back(tmpGeneratedToken);
		clear_token_candidates();
		if (llama_token_is_eog(t2tModel->get_raw_model(), tmpGeneratedToken))
		{
			// means end of generation
			llama_sampler_reset(mSamplerChain);
			llama_kv_cache_clear(mModelContext);
			mFinishState = finish_state::FINISHED;
			break;
		}

		else
		{
			if (mContextCursor == mContextLength)
			{
				// means token limit is reached
				llama_sampler_reset(mSamplerChain);
				llama_kv_cache_clear(mModelContext);
				mFinishState = finish_state::TOKEN_LIMIT_REACHED;
				break;
			}

			else
			{
				inf_common_batch_add(tempBatch, tmpGeneratedToken, ++mContextCursor, {0}, true);
				llama_decode(mModelContext, tempBatch); // Handle error here
				totalGeneratedTokens++;
				std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
				totalMilliseconds += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
			}
		}
	}
	llama_batch_free(tempBatch);
	if(totalGeneratedTokens)
	{
		F32 secondsPassed = (F32)totalMilliseconds / 1000.0f;
		mDiagnostics.evalTokensPerSecond = totalGeneratedTokens / secondsPassed;
	}

	mDecodeSignal.set_signal_finished();
}

GENERIC InfProcessorTextToText::_initialize_context()
{
	llama_context_params ctxParams = llama_context_default_params();
	ctxParams.n_ctx = mContextLength;
	ctxParams.n_batch = mBatchSize;
	ctxParams.n_seq_max = 1;
	ctxParams.n_threads = mThreadCount;
	ctxParams.n_threads_batch = mBatchProcessThreadCount;
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

	std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();

	mModelContext = llama_new_context_with_model(t2tModel->get_raw_model(), ctxParams);
	if (!mModelContext)
	{
		clear_samplers();
		mLastFailCode = last_fail_code::NOT_ENOUGH_MEMORY;
		mIsInitializeFailed = true;
		mInitializeSignal.set_signal_finished();
		return;
	}
	
	std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	
	mDiagnostics.loadTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();

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

GENERIC InfProcessorTextToText::_destroy_context()
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
	mProcessedBatchLength = 0;
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

GENERIC InfProcessorTextToText::update()
{
	if(signal_destroying())
	{
		return;
	}
	if(signal_state_destroying())
	{
		// Reset all signals on destruction
		reset_base_signals();
		mDecodeSignal.reset_signal_with_state();
		mInputSignal.reset_signal_with_state();
		this->release_object_watcher();
		on_destroy();
	}

	if(signal_state_initializing())
	{
		mInitializeSignal.reset_signal_state();
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
		mInputSignal.reset_signal_state();
		InfClientTextToText* t2tClient = static_cast<InfClientTextToText*>(get_assigned_client());
		if(t2tClient)
		{
			t2tClient->on_batch_processed(this, mProcessedBatchLength);
		}
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
			t2tClient->on_write(this, tokenVector, isFinish);
			if(isFinish)
			{
				t2tClient->on_finish(this, mContextCursor, mFinishState);
			}
		}
	}
}

GENERIC InfProcessorTextToText::update_t()
{
	if(is_registered())
	{
		if (signal_destroying())
		{
			_destroy_context();
		}

		if (is_running())
		{
			if (signal_input_process())
			{
				_decode_input();
			}
			
			while(is_processor_running())
			{
				if (signal_decode_process())
				{
					_decode_next();
				}

				else if(!mDecodeBehavior.mHaltOnWrite)
				{
					// If it is not halt on write, busy wait for logic thread to process the generated tokens
					mbase::sleep(mDecodeBehavior.mHaltDelay);
				}

				else
				{
					break;
				}
			}
		}
	}
	else
	{
		if(signal_initializing())
		{
			_initialize_context();
		}
	}
}

MBASE_END