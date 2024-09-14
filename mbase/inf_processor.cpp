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
	mContextSize_md_model(0),
	mProcessorId_md_model(0),
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

InfProcessorBase::flags InfProcessorBase::get_context_size(U32& out_size)
{
	MBASE_INF_PROC_RETURN_UNREGISTERED;
	out_size = mContextSize_md_model;
	return flags::INF_PROC_SUCCESS;
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
	mModelContext(NULL),
	mPresetCandidates(),
	mGeneratedToken(0),
	mContextLength(0),
	mContextCursor(0),
	mTokenizedInput(),
	mContextState(context_state::AWAITING_FOR_INPUT),
	mFinishState(finish_state::FINISHED),
	mAssignedClient(NULL)
{

}

InfTextToTextProcessor::~InfTextToTextProcessor()
{
	destroy_sync();
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

typename InfTextToTextProcessor::inf_token_candidates& InfTextToTextProcessor::get_token_candidates()
{
	return mPresetCandidates;
}

U32 InfTextToTextProcessor::get_max_token_length()
{
	return mContextLength;
}

InfClientTextToText* InfTextToTextProcessor::get_assigned_client()
{
	return mAssignedClient;
}

bool InfTextToTextProcessor::has_sampler(const mbase::string& in_sampler_name)
{
	return mSamplerMap.find(in_sampler_name) != mSamplerMap.end();
}

InfTextToTextProcessor::flags InfTextToTextProcessor::get_sampler(const mbase::string& in_sampler_name, InfSamplingBase*& out_sampler)
{
	if(has_sampler(in_sampler_name))
	{
		out_sampler = mSamplerMap[in_sampler_name];
		return flags::INF_PROC_SUCCESS;
	}
	return flags::INF_PROC_ERR_SAMPLER_NAME_MISMATCH;
}

GENERIC InfTextToTextProcessor::get_available_samplers(mbase::vector<InfSamplingBase*> out_samplers)
{
	for(sampler_map::iterator It = mSamplerMap.begin(); It != mSamplerMap.end(); ++It)
	{
		out_samplers.push_back(It->second);
	}
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

InfTextToTextProcessor::flags InfTextToTextProcessor::tokenize_input(CBYTEBUFFER in_data, size_type in_size, mbase::vector<inf_token>& out_tokens)
{
	MBASE_INF_PROC_RETURN_UNREGISTERED;
	
	if(!in_size)
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}

	mbase::vector<inf_token> tokenizedInput(in_size * 4);
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

InfTextToTextProcessor::flags InfTextToTextProcessor::tokenize_input(context_line* in_lines, size_type in_count, mbase::vector<inf_token>& out_tokens, bool in_append_assistant_token)
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

InfTextToTextProcessor::flags InfTextToTextProcessor::execute_input(const mbase::vector<inf_token>& in_tokens, bool in_abandon, const mbase::vector<mbase::string>& in_sampling_order)
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

	for(mbase::vector<mbase::string>::const_iterator It = in_sampling_order.cbegin(); It != in_sampling_order.cend(); ++It)
	{
		if(mSamplerMap.find(*It) == mSamplerMap.end())
		{
			// ignore
		}
		else
		{
			mSamplingOrder.push_back(mSamplerMap[*It]);
		}
	}

	mTokenizedInput = in_tokens;
	mInputSignal.set_signal();
	
	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::next()
{
	MBASE_INF_PROC_RETURN_UNREGISTERED;
	
	if(is_available())
	{
		return flags::INF_PROC_ERR_INPUT_IS_EMPTY;
	}
	
	mDecodeSignal.set_signal_state();

	if (!is_running())
	{
		return flags::INF_PROC_INFO_HALTED;
	}

	return flags::INF_PROC_SUCCESS;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::set_inference_client(InfClientTextToText* in_client, bool in_reset_on_set)
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

InfTextToTextProcessor::flags InfTextToTextProcessor::initialize(InfModelTextToText* in_model, U32 in_context_length)
{
	if(is_registered())
	{
		return flags::INF_PROC_SUCCESS;
	}

	if(signal_state_initializing())
	{
		return flags::INF_PROC_INFO_INITIALIZING;
	}

	if(signal_state_destroying())
	{
		return flags::INF_PROC_INFO_DESTROYING;
	}
	mTargetModel_md_model = in_model;
	mContextLength = in_context_length;
	mInitializeSignal.set_signal_with_state();
	start_processor();
	return flags::INF_PROC_INFO_INITIALIZING;
}

InfTextToTextProcessor::flags InfTextToTextProcessor::initialize_sync(InfModelTextToText* in_model, U32 in_context_length)
{
	initialize(in_model, in_context_length);
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
	if(!is_registered())
	{
		return flags::INF_PROC_SUCCESS;
	}

	release_inference_client();

	if(signal_state_destroying())
	{
		return flags::INF_PROC_INFO_DESTROYING;
	}

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

GENERIC InfTextToTextProcessor::update()
{
	if(is_registered())
	{
		if(signal_init_method())
		{
			mInitializeMethodSignal.reset_signal_with_state();
			on_initialize();
		}

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
			mbase::string outString(tokenString, tokenLength);

			t2tClient->on_write(outString.c_str(), outString.size(), mGeneratedToken, isTokenControl, isFinish);

			if(isFinish)
			{
				t2tClient->on_finish(mContextCursor, mFinishState);
			}
		}
	}
	else
	{
		if(signal_destroy_method())
		{
			mDestroyMethodSignal.reset_signal_with_state();
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

	llama_batch_clear(mInputBatch);
	for (size_type i = 0; i < mTokenizedInput.size(); ++i)
	{
		llama_batch_add(mInputBatch, mTokenizedInput[i], i, { 0 }, false);
	}

	mInputBatch.logits[mInputBatch.n_tokens - 1] = true;
	mContextCursor = mInputBatch.n_tokens;
	mContextState = context_state::DECODING_INPUT;
	mFinishState = finish_state::CONTINUE;
	int decodeResult = llama_decode(mModelContext, mInputBatch);
	mInputSignal.reset_signal_with_state();
	mDecodeSignal.set_signal_with_state();
}

GENERIC InfTextToTextProcessor::_decode_next()
{
	// if the decode signal state is set, process
	mContextState = context_state::GENERATING_OUTPUT;
	I32 modelVocab = 0;
	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);
	t2tModel->get_vocab_count(modelVocab);

	PTRF32 logits = llama_get_logits_ith(mModelContext, mInputBatch.n_tokens - 1);
	for (llama_token token_id = 0; token_id < modelVocab; ++token_id)
	{
		mPresetCandidates.emplace_back(llama_token_data{ token_id, logits[token_id], 0.0f });
	}

	llama_token_data_array tokenCandidates = { mPresetCandidates.data(), mPresetCandidates.size(), false };

	for(mbase::vector<InfSamplingBase*>::iterator It = mSamplingOrder.begin(); It != mSamplingOrder.end(); ++It)
	{
		InfSamplingBase* currentSampler = *It;
		currentSampler->set_token_array(&tokenCandidates);
		currentSampler->apply_sampling();
	}

	/*llama_sample_top_k(mModelContext, &tokenCandidates, 40, 1);
	llama_sample_tail_free(mModelContext, &tokenCandidates, 1.0, 1);
	llama_sample_top_p(mModelContext, &tokenCandidates, 0.950, 1);
	llama_sample_min_p(mModelContext, &tokenCandidates, 0.050, 1);
	llama_sample_typical(mModelContext, &tokenCandidates, 1.0, 1);
	llama_sample_temp(mModelContext, &tokenCandidates, 0.1);*/
	llama_sample_softmax(mModelContext, &tokenCandidates);

	if(mPenaltyList.size() == 64)
	{
		mPenaltyList.clear();
	}

	//llama_sample_repetition_penalties(mModelContext, &tokenCandidates, mPenaltyList.data(), mPenaltyList.size(), 1.0, 0.0, 0.0);
	mGeneratedToken = mPresetCandidates[0].id;
	mPenaltyList.push_back(mGeneratedToken);

	llama_batch_clear(mInputBatch);
	
	if (llama_token_is_eog(t2tModel->get_raw_model(), mGeneratedToken) || !mGeneratedToken)
	{
		// means end of generation
		mSamplingOrder.clear();
		mFinishState = finish_state::FINISHED;
		llama_kv_cache_clear(mModelContext);
		clear_token_candidates();
		mDecodeSignal.reset_signal_with_state();
		mTokenGeneratedSignal.set_signal();
	}

	else
	{
		clear_token_candidates();

		if (mContextCursor == mContextLength)
		{
			// means token limit is reached
			mSamplingOrder.clear();
			mContextState = context_state::AWAITING_FOR_CURSOR_ALIGNMENT;
			mFinishState = finish_state::TOKEN_LIMIT_REACHED;
			llama_kv_cache_clear(mModelContext);
			mDecodeSignal.reset_signal_with_state();
		}

		else
		{
			llama_batch_add(mInputBatch, mGeneratedToken, mContextCursor, { 0 }, true);
			mContextCursor++;
			llama_decode(mModelContext, mInputBatch);
		}
		mDecodeSignal.reset_signal_state();
		mTokenGeneratedSignal.set_signal();
	}
}

GENERIC InfTextToTextProcessor::_initialize_context()
{
	llama_context_params ctxParams = llama_context_default_params();
	ctxParams.n_ctx = mContextLength;
	ctxParams.n_batch = mContextLength;
	ctxParams.n_seq_max = 1;
	ctxParams.n_threads = 64;
	ctxParams.n_threads_batch = 64;
	ctxParams.n_ubatch = mContextLength / 8;

	InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);

	mModelContext = llama_new_context_with_model(t2tModel->get_raw_model(), ctxParams);
	if (!mModelContext)
	{
		mInitializeSignal.reset_signal_with_state();
		return;
	}

	mInputBatch = llama_batch_init(mContextLength, 0, 1);
	mContextCursor = 0;
	mIsRunning = true;
	mInitializeSignal.reset_signal_with_state();
	mIsRegistered = true;
	
	mInitializeMethodSignal.set_signal_with_state();
}

GENERIC InfTextToTextProcessor::_destroy_context()
{
	llama_batch_free(mInputBatch);
	llama_free(mModelContext);
	mContextCursor = 0;
	mModelContext = NULL;
	mIsRunning = false;
	mPresetCandidates.clear();
	mContextLength = 0;
	mContextCursor = 0;
	mTokenizedInput.clear();
	mPenaltyList.clear();
	mSamplingOrder.clear();
	mContextState = context_state::AWAITING_FOR_INPUT;
	mFinishState = finish_state::FINISHED;

	for(sampler_map::iterator It = mSamplerMap.begin(); It != mSamplerMap.end();)
	{
		delete It->second; // samplers are heap allocated at add_sampler
		It = mSamplerMap.erase(It);
	}

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
			if(signal_destroying())
			{
				_destroy_context();
			}

			if (is_running())
			{
				if(signal_input_process())
				{
					_decode_input(); /* SIG_SET(decode), SIGW_STATE_RESET(input) */
				}

				if(signal_decode_process())
				{
					if(signal_state_decode_process())
					{
						_decode_next();
						/*
						* IF TOKEN END OF GENERATION --> SIG_SET(token_generated), SIGW_STATE_RESET(decode)
						* IF NOT TOKEN END OF GENERATION --> SIG_SET(token_generated), SIG_STATE_RESET(decode)
						* 
						* 
						*/
					}
				}
				else
				{
					// decode is not present
					mbase::sleep(30); // slow down the loop
				}
			}
			else
			{
				mbase::sleep(30); // Since the context is halted, we may slow down the loop
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
				mbase::sleep(30); // unregistered context, we may slow down the loop
			}
		}
	}
}

MBASE_END
