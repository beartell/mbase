#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_sampling.h>
#include <mbase/framework/timer_loop.h>
#include <mbase/framework/timers.h>
#include <common/common.h>

MBASE_BEGIN

#define MBASE_INF_PROC_RETURN_UNREGISTERED \
if(this->signal_state_initializing())\
{\
	return flags::INF_PROC_INFO_INITIALIZING;\
}\
if(!this->is_registered())\
{\
	return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;\
}\
if (this->signal_state_destroying())\
{\
	return flags::INF_PROC_INFO_DESTROYING;\
}

#define MBASE_INF_PROC_RETURN_HALTED \
if(!this->is_registered())\
{\
	return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;\
}\
if(!this->is_running())\
{\
	return flags::INF_PROC_ERR_HALTED;\
}

static U32 gInfProcMaxTokenLength = 128;

InfProcessorBase::InfProcessorBase() :
	mTargetModel_md_model(NULL),
	mIsRunning(false),
	mIsRegistered(false),
	mContextLength(0),
	mInactivityThreshold(0)
{
}

bool InfProcessorBase::is_registered() const
{
	return mIsRegistered;
}

bool InfProcessorBase::is_running()
{
	return mIsRunning;
}

bool InfProcessorBase::signal_state_initializing() const
{
	return mInitializeSignal.get_signal_state();
}

bool InfProcessorBase::signal_state_destroying() const
{
	return mDestroySignal.get_signal_state();
}

bool InfProcessorBase::signal_initializing() const
{
	return mInitializeSignal.get_signal();
}

bool InfProcessorBase::signal_destroying() const
{
	return mDestroySignal.get_signal();
}

U32 InfProcessorBase::get_context_size()
{
	return mContextLength;
}

InfModelBase* InfProcessorBase::get_processed_model()
{
	return mTargetModel_md_model;
}

U32 InfProcessorBase::get_inactivity_threshold()
{
	return mInactivityThreshold;
}

processor_signal& InfProcessorBase::get_initialize_signal()
{
	return mInitializeSignal;
}

processor_signal& InfProcessorBase::get_destroy_signal()
{
	return mDestroySignal;
}

const mbase::string& InfProcessorBase::get_context_identifier()
{
	return mContextIdentifier;
}

GENERIC InfProcessorBase::set_inactivity_threshold(U32 in_threshold)
{
	mInactivityThreshold = in_threshold;
}

GENERIC InfProcessorBase::halt()
{
	mIsRunning = false;
}

GENERIC InfProcessorBase::resume()
{
	mIsRunning = true;
}

InfTextToTextProcessor::InfTextToTextProcessor():
	mSamplerChain(NULL),
	mModelContext(NULL),
	mPresetCandidates(),
	mGeneratedToken(0),
	mContextCursor(0),
	mBatchSize(0),
	mThreadCount(0),
	mBatchThreadCount(0),
	mFinishState(finish_state::FINISHED),
	mAssignedClient(NULL),
	mLastFailCode(init_fail_code::MODEL_NOT_INITIALIZED),
	mFlashAttention(false)
{
	
}

InfTextToTextProcessor::~InfTextToTextProcessor()
{
	destroy_sync();
}

InfTextToTextProcessor::init_fail_code InfTextToTextProcessor::get_last_fail_code() const
{
	return mLastFailCode;
}

bool InfTextToTextProcessor::is_available() const
{
	if (signal_input_process() || signal_decode_process() || signal_state_decode_process())
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

bool InfTextToTextProcessor::signal_token_generated() const
{
	return mTokenGeneratedSignal.get_signal();
}

bool InfTextToTextProcessor::signal_init_method() const
{
	return mInitializeMethodSignal.get_signal();
}

bool InfTextToTextProcessor::signal_destroy_method() const
{
	return mDestroyMethodSignal.get_signal();
}

bool InfTextToTextProcessor::signal_init_fail_method() const
{
	return mInitializeFailSignal.get_signal();
}

U32 InfTextToTextProcessor::get_max_token_length()
{
	return mContextLength;
}

InfClientTextToText* InfTextToTextProcessor::get_assigned_client()
{
	return mAssignedClient;
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
	if(signal_state_initializing())
	{
		return flags::INF_PROC_INFO_INITIALIZING;
	}

	if(!is_registered())
	{
		return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;
	}

	if(signal_state_destroying())
	{
		return flags::INF_PROC_INFO_DESTROYING;
	}

	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_token_vector& out_tokens)
{
	MBASE_INF_PROC_RETURN_UNREGISTERED;
	
	if(!in_size)
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}

	inf_token_vector tokenizedInput(in_size * 4);
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

InfTextToTextProcessor::flags InfTextToTextProcessor::tokenize_input(context_line* in_lines, size_type in_count, inf_token_vector& out_tokens, bool in_append_assistant_token)
{
	MBASE_INF_PROC_RETURN_UNREGISTERED;
	
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

InfTextToTextProcessor::flags InfTextToTextProcessor::execute_input(const inf_token_vector& in_tokens, bool in_abandon)
{
	MBASE_INF_PROC_RETURN_UNREGISTERED;

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
	}

	mTokenizedInput = in_tokens;
	mInputSignal.set_signal();
	start_processor();

	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::next()
{
	MBASE_INF_PROC_RETURN_UNREGISTERED;

	if(is_available())
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}

	mDecodeSignal.set_signal();
	if (!is_running())
	{
		return flags::INF_PROC_INFO_HALTED;
	}
	
	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::set_inference_client(InfClientTextToText* in_client)
{
	MBASE_INF_PROC_RETURN_UNREGISTERED;

	if(get_assigned_client())
	{
		return flags::INF_PROC_ERR_ALREADY_PROCESSING;
	}

	if(!in_client)
	{
		return flags::INF_PROC_ERR_MISSING_CLIENT;
	}

	if(in_client->is_registered())
	{
		InfTextToTextProcessor* clientProc = NULL;
		in_client->get_host_processor(clientProc);
		if(clientProc != this)
		{
			return flags::INF_PROC_ERR_BELONGS_TO_ANOTHER_PROCESSOR;
		}
		else
		{
			return flags::INF_PROC_SUCCESS;
		}
	}

	mAssignedClient = in_client;
	mAssignedClient->_on_register(this);

	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::initialize(
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
	if (signal_state_initializing())
	{
		return flags::INF_PROC_INFO_INITIALIZING;
	}

	if (signal_state_destroying())
	{
		return flags::INF_PROC_INFO_DESTROYING;
	}

	if(is_registered())
	{
		return flags::INF_PROC_SUCCESS;
	}

	mTargetModel_md_model = in_model;
	mContextLength = in_context_length;
	mContextIdentifier = in_context_id;
	mBatchSize = in_batch_size;
	mThreadCount = in_thread_count;
	mBatchThreadCount = in_batch_thread_count;

	mDiagnostics.log(PcDiagnostics::flags::LOGTYPE_INFO, PcDiagnostics::flags::LOGIMPORTANCE_HIGH, "Initializing context with length (%d) and id (%s)", in_context_length, in_context_id.c_str());
	mInitializeSignal.set_signal_with_state();
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
	while(signal_state_initializing())
	{

	}
	if(!is_registered())
	{
		return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;
	}
	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::destroy()
{
	clear_samplers();
	if(!is_registered())
	{
		return flags::INF_PROC_SUCCESS;
	}

	release_inference_client();

	if(signal_state_destroying())
	{
		return flags::INF_PROC_INFO_DESTROYING;
	}
	on_destroying();
	start_processor();
	mDestroySignal.set_signal_with_state();
	return flags::INF_PROC_INFO_DESTROYING;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::destroy_sync()
{
	destroy();
	while(signal_state_destroying())
	{
		// block until operation finishes
	}

	return flags::INF_PROC_INFO_NEED_UPDATE;
}

GENERIC InfTextToTextProcessor::release_inference_client()
{
	InfClientTextToText* assignedClient = get_assigned_client();
	if(assignedClient)
	{
		assignedClient->_on_unregister();
		mAssignedClient = NULL;
	}
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

GENERIC InfTextToTextProcessor::on_initialize_fail(init_fail_code out_code)
{

}

GENERIC InfTextToTextProcessor::on_destroying()
{

}

GENERIC InfTextToTextProcessor::update()
{
	if(signal_state_destroying())
	{
		return;
	}

	if(signal_init_fail_method())
	{
		mInitializeFailSignal.reset_signal_with_state();
		on_initialize_fail(get_last_fail_code());
	}

	if(signal_init_method())
	{
		mInitializeMethodSignal.reset_signal_with_state();
		on_initialize();
	}

	if(is_registered())
	{
		if(signal_token_generated())
		{
			// do things with the generated token
			InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);
			mTokenGeneratedSignal.reset_signal_with_state();
			InfClientTextToText* t2tClient = get_assigned_client();
			if(!t2tClient)
			{
				// means client is abandoned the context
				return;
			}

			bool isTokenControl = false;
			bool isFinish = false;
			if(t2tModel->is_token_control(mGeneratedToken) == InfModelTextToText::flags::INF_MODEL_SUCCESS)
			{
				isTokenControl = true;
			}
			
			if(mFinishState != finish_state::CONTINUE)
			{
				isFinish = true;
			} 

			IBYTE tokenString[64] = { 0 };
			I32 tokenLength = llama_token_to_piece(t2tModel->get_raw_model(), mGeneratedToken, tokenString, 64, false, true);
			// TODO: Handle if llama_token_to_piece is failed
			t2tClient->on_write(tokenString, tokenLength, mGeneratedToken, isTokenControl, isFinish);

			if(isFinish)
			{
				stop_processor();
				t2tClient->on_finish(mContextCursor, mFinishState);
			}
			return;
		}
	}
	else
	{
		if(signal_destroy_method())
		{
			mDestroyMethodSignal.reset_signal_with_state();
			stop_processor();
			on_destroy();
		}
	}
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
	mFinishState = finish_state::CONTINUE;
	int decodeResult = llama_decode(mModelContext, mInputBatch);
	mInputSignal.reset_signal_with_state();
	mDecodeSignal.set_signal_state();
}

GENERIC InfTextToTextProcessor::_decode_next()
{
	// Main Decode loop
	mDecodeSignal.reset_signal();
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
	mGeneratedToken = llama_sampler_sample(mSamplerChain, mModelContext, -1);
	
	llama_sampler_accept(mSamplerChain, mGeneratedToken);

	clear_token_candidates();
	
	if (llama_token_is_eog(t2tModel->get_raw_model(), mGeneratedToken))
	{
		// means end of generation
		llama_sampler_reset(mSamplerChain);
		mFinishState = finish_state::FINISHED;
		llama_kv_cache_clear(mModelContext);
		mDecodeSignal.reset_signal_with_state();
		mTokenGeneratedSignal.set_signal();
	}

	else
	{

		if (mContextCursor == mContextLength)
		{
			// means token limit is reached
			llama_sampler_reset(mSamplerChain);
			mFinishState = finish_state::TOKEN_LIMIT_REACHED;
			llama_kv_cache_clear(mModelContext);
			mDecodeSignal.reset_signal_with_state();
		}

		else
		{
			mInputBatch = llama_batch_get_one(&mGeneratedToken, 1);
			//llama_batch_add(mInputBatch, mGeneratedToken, mContextCursor, { 0 }, true);
			mContextCursor++;
			llama_decode(mModelContext, mInputBatch);
		}
		mTokenGeneratedSignal.set_signal();
	}
}

GENERIC InfTextToTextProcessor::_initialize_context()
{
	llama_context_params ctxParams = llama_context_default_params();
	ctxParams.n_ctx = mContextLength;
	ctxParams.n_batch = mContextLength;
	ctxParams.n_seq_max = 1;
	ctxParams.n_threads = mThreadCount;
	ctxParams.n_threads_batch = mBatchThreadCount;
	ctxParams.n_ubatch = mBatchSize;
	ctxParams.flash_attn = mFlashAttention;
	
	mInitializeFailSignal.reset_signal_with_state();

	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);

	if(!t2tModel || !t2tModel->is_initialized())
	{
		clear_samplers();
		mLastFailCode = init_fail_code::MODEL_NOT_INITIALIZED;
		mInitializeFailSignal.set_signal_with_state();
		mInitializeSignal.reset_signal_with_state();
		return;
	}

	mModelContext = llama_new_context_with_model(t2tModel->get_raw_model(), ctxParams);
	if (!mModelContext)
	{
		clear_samplers();
		mLastFailCode = init_fail_code::NOT_ENOUGH_MEMORY;
		mInitializeFailSignal.set_signal_with_state();
		mInitializeSignal.reset_signal_with_state();
		return;
	}
	
	mContextCursor = 0;
		
	I32 modelVocabCount = 0;
	inf_token eotToken = 0;
	inf_token nlToken = 0;
	
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
	
	mIsRunning = true;
	mInitializeSignal.reset_signal_with_state();
	mIsRegistered = true;

	mInitializeMethodSignal.set_signal_with_state();
}

GENERIC InfTextToTextProcessor::_destroy_context()
{
	llama_free(mModelContext);
	mModelContext = NULL;
	// What to do with mInputBatch ?
	mPresetCandidates.clear();
	mTokenizedInput.clear();
	mSamplerDescriptions.clear();
	mGeneratedToken = 0;
	mContextCursor = 0;
	mBatchSize = 0;
	mThreadCount = 0;
	mBatchThreadCount = 0;
	mFlashAttention = false;
	mIsRunning = false;
	mFinishState = finish_state::FINISHED;
	mAssignedClient = NULL;
	mLastFailCode = init_fail_code::MODEL_NOT_INITIALIZED;

	clear_samplers();

	mInitializeSignal.reset_signal_with_state();
	mTokenGeneratedSignal.reset_signal_with_state();
	mDecodeSignal.reset_signal_with_state();
	mDestroySignal.reset_signal_with_state();
	mTargetModel_md_model = NULL;
	mIsRegistered = false;

	mDestroyMethodSignal.set_signal_with_state();
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
			}

			if (is_running())
			{
				if (signal_input_process())
				{
					_decode_input();
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
		}
	}
}

MBASE_END
