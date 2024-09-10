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

bool InfTextToTextProcessor::has_client() const
{
	return mAssignedClient != NULL;
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

InfTextToTextProcessor::flags InfTextToTextProcessor::tokenize_input(context_line* in_lines, size_type in_count, mbase::vector<inf_token>& out_tokens)
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
	std::cout << totalMessage << std::endl;
	return tokenize_input(totalMessage.data(), totalMessage.size(), out_tokens);
}

InfTextToTextProcessor::flags InfTextToTextProcessor::execute_input(const mbase::vector<inf_token>& in_tokens, bool in_abandon)
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
	
	if(!is_running())
	{
		return flags::INF_PROC_INFO_HALTED;
	}

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

	update();
	return flags::INF_PROC_SUCCESS;
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
	printf("INPUT SIGNAL RECEIVED\n");
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
	printf("INPUT SIGNAL PROCESSED: %d\n", decodeResult);
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

	mGeneratedToken = llama_sample_token_greedy(mModelContext, &tokenCandidates);
	llama_batch_clear(mInputBatch);
	
	if (llama_token_is_eog(t2tModel->get_raw_model(), mGeneratedToken) || !mGeneratedToken)
	{
		// means end of generation
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
			mContextState = context_state::AWAITING_FOR_CURSOR_ALIGNMENT;
			mFinishState = finish_state::TOKEN_LIMIT_REACHED;
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
	mContextState = context_state::AWAITING_FOR_INPUT;
	mFinishState = finish_state::FINISHED;

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

//InfInactiveClientCleaner::InfInactiveClientCleaner(InfProcessor& in_host) : mHostProcessor(&in_host)
//{
//
//}
//
//GENERIC InfInactiveClientCleaner::on_call(user_data in_data)
//{
//	if(mHostProcessor)
//	{
//		mHostProcessor->update_inactivity();
//	}
//}
//
//InfProcessor::InfProcessor() :
//	mLogicHandlers(),
//	mRegisteredClients(),
//	mProcessedModel(NULL),
//	mPresetCandidates(),
//	mModelContext(NULL),
//	mProcessorId(0),
//	mContextIdCounter(0),
//	mClientsMutex(),
//	mMaxClients(gInfProcessorDefaultMaxSeq),
//	mRegisteredBatchSize(0),
//	mInactivityThreshold(gProcessorMinimumInactivityThreshold),
//	mIsRunning(false),
//	mClientCleaner(*this)
//{
//	mClientCleaner.set_target_time(1000);
//	mClientCleaner.set_tick_limit(100000);
//}
//
//InfProcessor::~InfProcessor()
//{
//	this->destroy();
//}
//
//typename InfProcessor::iterator InfProcessor::begin() noexcept
//{
//	return mRegisteredClients.begin();
//}
//
//typename InfProcessor::iterator InfProcessor::end() noexcept
//{
//	return mRegisteredClients.end();
//}
//
//typename InfProcessor::const_iterator InfProcessor::begin() const noexcept
//{
//	return mRegisteredClients.begin();
//}
//
//typename InfProcessor::const_iterator InfProcessor::end() const noexcept
//{
//	return mRegisteredClients.end();
//}
//
//typename InfProcessor::const_iterator InfProcessor::cbegin() const noexcept
//{
//	return mRegisteredClients.cbegin();
//}
//
//typename InfProcessor::const_iterator InfProcessor::cend() const noexcept
//{
//	return mRegisteredClients.cend();
//}
//
//typename InfProcessor::reverse_iterator InfProcessor::rbegin() noexcept
//{
//	return mRegisteredClients.rbegin();
//}
//
//typename InfProcessor::reverse_iterator InfProcessor::rend() noexcept
//{
//	return mRegisteredClients.rend();
//}
//
//typename InfProcessor::const_reverse_iterator InfProcessor::crbegin() const noexcept
//{
//	return mRegisteredClients.crbegin();
//}
//
//typename InfProcessor::const_reverse_iterator InfProcessor::crend() const noexcept
//{
//	return mRegisteredClients.crend();
//}
//
//bool InfProcessor::is_registered() const
//{
//	return (mModelContext != NULL);
//}
//
//bool InfProcessor::is_running() const
//{
//	return mIsRunning;
//}
//
//InfProcessor::flags InfProcessor::get_context_size(U32& out_size)
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//	out_size = llama_n_ctx(mModelContext);
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::get_client_count(U32& out_size)
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//	out_size = mRegisteredClients.size();
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::get_max_clients(U32& out_size) 
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//	out_size = mMaxClients;
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::get_process_thread_count(U32& out_count)
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//	out_count = llama_n_threads(mModelContext);
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::get_max_batch_size(U32& out_size)
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//	out_size = llama_n_batch(mModelContext);
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfModel* InfProcessor::get_processed_model()
//{
//	return mProcessedModel;
//}
//
//U32 InfProcessor::get_inactivity_threshold()
//{
//	return mInactivityThreshold;
//}
//
//InfInactiveClientCleaner* InfProcessor::get_client_cleaner()
//{
//	return &mClientCleaner;
//}
//
//InfProcessor::flags InfProcessor::destroy()
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//	mClientsMutex.acquire();
//	for (client_list::iterator It = mRegisteredClients.begin(); It != mRegisteredClients.end(); ++It)
//	{
//		InfClient* myClient = *It;
//		unregister_client(*myClient);
//	}
//	resume();
//	update_t();
//	mClientsMutex.release();
//	update();
//
//	// CLEANING PROCESSOR
//	mLogicHandlers.clear();
//	mPresetCandidates.clear();
//	llama_free(mModelContext);
//	mModelContext = NULL;
//	mProcessorId = 0;
//	mContextIdCounter = 0;
//	mMaxClients = gInfProcessorDefaultMaxSeq;
//	mRegisteredBatchSize = 0;
//	InfModel::iterator tmpIt;
//	mProcessedModel->_unregister_processor(*this, tmpIt);
//	mProcessedModel = NULL;
//	mIsRunning = false;
//	// CLEANING PROCESSOR
//
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::_destroy(InfModel::iterator& _out_it)
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//	mClientsMutex.acquire();
//	for(client_list::iterator It = mRegisteredClients.begin(); It != mRegisteredClients.end(); ++It)
//	{
//		InfClient* myClient = *It;
//		unregister_client(*myClient);
//	}
//	resume();
//	update_t();
//	mClientsMutex.release();
//	update();
//
//	// CLEANING PROCESSOR
//	mLogicHandlers.clear();
//	mPresetCandidates.clear();
//	llama_free(mModelContext);
//	mModelContext = NULL;
//	mProcessorId = 0;
//	mContextIdCounter = 0;
//	mMaxClients = gInfProcessorDefaultMaxSeq;
//	mRegisteredBatchSize = 0;
//	InfModel::iterator tmpIt;
//	mProcessedModel->_unregister_processor(*this, _out_it);
//	mProcessedModel = NULL;
//	mIsRunning = false;
//	// CLEANING PROCESSOR
//
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::tokenize_input(CBYTEBUFFER in_data, size_type in_size, mbase::vector<inf_token>& out_tokens)
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//
//	if(!in_size)
//	{
//		return flags::INF_PROC_ERR_INVALID_TOKEN_SIZE;
//	}
//
//	mbase::vector<inf_token> tokenizedInput(in_size * 4);
//	I32 tokenCount = llama_tokenize(get_processed_model()->get_raw_model(), in_data, in_size, tokenizedInput.data(), in_size * 4, false, true);
//	if(tokenCount == -1)
//	{
//		return flags::INF_PROC_ERR_INVALID_TOKEN_INPUT;
//	}
//
//	tokenizedInput.resize(tokenCount);
//	out_tokens = std::move(tokenizedInput);
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::register_client(const mbase::vector<inf_token>& in_data, InfClient& out_client, U32 in_token_limit)
//{
//	MBASE_INF_PROC_RETURN_HALTED;
//
//	if (out_client.is_registered())
//	{
//		return flags::INF_PROC_ERR_CLIENT_ALREADY_REGISTERED;
//	}
//
//	if (mRegisteredClients.size() == mMaxClients)
//	{
//		return flags::INF_PROC_ERR_CLIENT_LIMIT_REACHED;
//	}
//
//	if (in_token_limit < gProcessorMinimumTokenCount) // minimum token count
//	{
//		return flags::INF_PROC_ERR_TOKEN_LIMIT_IS_TOO_LOW;
//	}
//
//	U32 contextBatchSize = 0;
//	U32 totalContextSize = 0;
//	get_max_batch_size(contextBatchSize);
//	get_context_size(totalContextSize);
//
//	if(in_token_limit > contextBatchSize || in_data.size() > in_token_limit)
//	{
//		return flags::INF_PROC_ERR_EXCEED_TOKEN_LIMIT;
//	}
//
//	if(mRegisteredBatchSize + in_token_limit > totalContextSize)
//	{
//		return flags::INF_PROC_ERR_CONTEXT_IS_FULL;
//	}
//
//	/*if (in_data.size())
//	{
//		out_client.mParsedTokens = in_data;
//		out_client.mIsDataSet = true;
//	}*/
//
//	out_client._register_self_client_params(
//		this,
//		in_data,
//		++this->mContextIdCounter,
//		in_token_limit
//	);
//
//	/*out_client.mfrBatchCursor = 0;
//	out_client.mfrHostProcessor = this;
//	out_client.mSequenceId = ++this->mContextIdCounter;
//	out_client.mInactivityCounter = 0;
//	out_client.mIsLogicProcessed = true;
//	out_client.mfrMaxTokenCount = in_token_limit;
//	out_client.mFs = InfClient::finish_state::INF_FINISH_STATE_CONTINUE;
//	out_client.mfrBatch = llama_batch_init(in_token_limit, 0, 1);*/
//
//	mRegisteredBatchSize += out_client.mfrMaxTokenCount;
//	mRegisteredClients.push_back(&out_client);
//
//	out_client.on_register();
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::register_client(CBYTEBUFFER in_data, size_type in_size, InfClient& out_client, U32 in_token_limit)
//{
//	mbase::vector<inf_token> parsedTokens;
//	flags tokenizeResult = tokenize_input(in_data, in_size, parsedTokens);
//
//	if(tokenizeResult != flags::INF_PROC_SUCCESS)
//	{
//		return tokenizeResult;
//	}
//	
//	return register_client(std::move(parsedTokens), out_client, in_token_limit);
//}
//
//InfProcessor::flags InfProcessor::register_client(const mbase::string& in_string, InfClient& out_client, U32 in_token_limit)
//{
//	return register_client(in_string.c_str(), in_string.size(), out_client, in_token_limit);
//}
//
//InfProcessor::flags InfProcessor::register_client(InfClient& out_client, U32 in_token_limit)
//{
//	MBASE_INF_PROC_RETURN_HALTED;
//
//	if (out_client.is_registered())
//	{
//		return flags::INF_PROC_ERR_CLIENT_ALREADY_REGISTERED;
//	}
//
//	if (mRegisteredClients.size() == mMaxClients)
//	{
//		return flags::INF_PROC_ERR_CLIENT_LIMIT_REACHED;
//	}
//
//	U32 contextBatchSize = 0;
//	U32 totalContextSize = 0;
//	get_max_batch_size(contextBatchSize);
//	get_context_size(totalContextSize);
//
//	if(in_token_limit > contextBatchSize)
//	{
//		return flags::INF_PROC_ERR_EXCEED_TOKEN_LIMIT;
//	}
//
//	if(mRegisteredBatchSize + in_token_limit > totalContextSize)
//	{
//		return flags::INF_PROC_ERR_CONTEXT_IS_FULL;
//	}
//
//	out_client._register_self_client_params(
//		this,
//		++this->mContextIdCounter,
//		in_token_limit
//	);
//
//	/*out_client.mfrBatchCursor = 0;
//	out_client.mfrHostProcessor = this;
//	out_client.mSequenceId = ++this->mContextIdCounter;
//	out_client.mInactivityCounter = 0;
//	out_client.mIsLogicProcessed = true;
//	out_client.mfrMaxTokenCount = in_token_limit;
//	out_client.mFs = InfClient::finish_state::INF_FINISH_STATE_CONTINUE;
//	out_client.mfrBatch = llama_batch_init(in_token_limit, 0, 1);*/
//	
//	mRegisteredBatchSize += out_client.mfrMaxTokenCount;
//	mRegisteredClients.push_back(&out_client);
//
//	out_client.on_register();
//	return flags::INF_PROC_SUCCESS;
//}
//
//InfProcessor::flags InfProcessor::unregister_client(InfClient& in_client)
//{
//	MBASE_INF_PROC_RETURN_UNREGISTERED;
//
//	if (!in_client.is_registered() || in_client.is_unregistering())
//	{
//		return flags::INF_PROC_SUCCESS;
//	}
//
//	if(in_client.mfrHostProcessor != this)
//	{
//		return flags::INF_PROC_ERR_BELONGS_TO_ANOTHER_PROCESSOR;
//	}
//
//	in_client.mIsUnregistering = true;
//	return flags::INF_PROC_SUCCESS;
//}
//
//GENERIC InfProcessor::set_max_client_count(U32 in_max_clients)
//{
//	mMaxClients = in_max_clients;
//}
//
//GENERIC InfProcessor::set_inactivity_threshold(U32 in_threshold)
//{
//	mInactivityThreshold = in_threshold;
//}
//
//GENERIC InfProcessor::update_inactivity()
//{
//	if (this->is_registered())
//	{
//		for (InfProcessor::client_list::iterator It = begin(); It != end(); ++It)
//		{
//			InfClient* tmpClient = *It;
//
//			if (tmpClient->get_inactivity_counter() == get_inactivity_threshold())
//			{
//				unregister_client(*tmpClient);
//				continue;
//			}
//			++tmpClient->mInactivityCounter;
//		}
//	}
//	mClientCleaner.reset_tick_counter();
//}
//
//GENERIC InfProcessor::halt()
//{
//	mIsRunning = false;
//}
//
//GENERIC InfProcessor::resume()
//{
//	mIsRunning = true;
//}
//
//GENERIC InfProcessor::update()
//{
//	if(!this->is_registered() || !this->is_running())
//	{
//		return;
//	}
//
//	mbase::lock_guard mRg(mProcHandlerMutex);
//	for(logic_handlers::iterator It = mLogicHandlers.begin(); It != mLogicHandlers.end(); ++It)
//	{
//		InfClient* myClient = *It;
//		myClient->mInactivityCounter = 0;
//		mbase::char_stream& myCharacterStream = myClient->mfrGeneratedToken;
//		if(!myClient->is_registered())
//		{
//			mRegisteredBatchSize -= myClient->mfrMaxTokenCount;
//			if(myClient->mFs == mbase::InfClient::finish_state::INF_FINISH_STATE_ABANDONED)
//			{
//				myCharacterStream.set_cursor_front();
//				myClient->on_finish(myClient->mfrBatchCursor);
//			}
//			myClient->_reset_client();
//			myClient->on_unregister();
//			myClient->mFs = mbase::InfClient::finish_state::INF_FINISH_STATE_CONTINUE;
//			continue;
//		}
//
//		if (!myClient->is_data_set())
//		{
//			if(myClient->is_processing())
//			{
//				// means execution abandoned
//				myClient->mIsProcessing = false;
//				myClient->mSamplingOrder.clear();
//				myCharacterStream.set_cursor_front();
//				myClient->mFs = mbase::InfClient::finish_state::INF_FINISH_STATE_ABANDONED;
//				myClient->on_finish(myClient->mfrBatchCursor);
//				myClient->mFs = mbase::InfClient::finish_state::INF_FINISH_STATE_CONTINUE;
//				continue;
//			}
//			else
//			{
//				// means token limit reached
//				myClient->mSamplingOrder.clear();
//				myCharacterStream.set_cursor_front();
//				myClient->on_finish(myClient->mfrBatchCursor);
//				myClient->mFs = mbase::InfClient::finish_state::INF_FINISH_STATE_CONTINUE;
//				continue;
//			}
//		}
//
//		if(!myClient->is_processing())
//		{
//			myClient->mFs = mbase::InfClient::finish_state::INF_FINISH_STATE_SUCCESS;
//			myClient->mSamplingOrder.clear();
//			myClient->on_write(myCharacterStream.get_buffer(), myCharacterStream.get_pos());
//			myCharacterStream.set_cursor_front();
//			myClient->on_finish(myClient->mfrBatchCursor);
//			myClient->mFs = mbase::InfClient::finish_state::INF_FINISH_STATE_CONTINUE;
//			continue;
//		}
//
//		myClient->on_write(myCharacterStream.get_buffer(), myCharacterStream.get_pos());
//		myCharacterStream.set_cursor_front();
//	}
//	mLogicHandlers.clear();
//}
//
//GENERIC InfProcessor::update_t()
//{
//	if (!this->is_registered() || !this->is_running())
//	{
//		return;
//	}
//
//	mbase::lock_guard lgClient(mClientsMutex);
//	for (client_list::iterator It = mRegisteredClients.begin(); It != mRegisteredClients.end(); ++It)
//	{
//		if (!this->is_registered() || !this->is_running())
//		{
//			return;
//		}
//		InfClient* myClient = *It;
//		if(myClient->is_unregistering())
//		{
//			mbase::lock_guard procMutex(mProcHandlerMutex);
//			if(myClient->is_processing())
//			{
//				myClient->mFs = InfClient::finish_state::INF_FINISH_STATE_ABANDONED;
//			}
//
//			llama_kv_cache_seq_rm(mModelContext, myClient->mSequenceId, -1, -1);
//			myClient->mfrHostProcessor = NULL;
//			It = mRegisteredClients.erase(It);
//			mLogicHandlers.push_back(myClient);
//			if (It == mRegisteredClients.end())
//			{
//				break;
//			}
//			continue;
//		}
//
//		if (!myClient->is_logic_processed())
//		{
//			continue;
//		}
//
//		if (!myClient->is_data_set())
//		{
//			continue;
//		}
//
//		if(!myClient->is_processing())
//		{
//			mbase::vector<InfProcessor::inf_token>& generatedTokens = myClient->mParsedTokens;
//			U32 parsedTokenSize = generatedTokens.size();
//			if (!parsedTokenSize)
//			{
//				continue;
//			}
//
//			for (size_type i = 0; i < generatedTokens.size(); i++)
//			{
//				I32 seqId = myClient->mSequenceId;
//				llama_batch_add(myClient->mfrBatch, generatedTokens[i], i, { seqId }, false);
//			}
//
//			//generatedTokens.clear();
//			myClient->mfrBatch.logits[myClient->mfrBatch.n_tokens - 1] = true;
//			myClient->mIsProcessing = true;
//			myClient->mfrBatchCursor = myClient->mfrBatch.n_tokens;
//		}
//		else
//		{
//			
//			if(myClient->mfrBatchCursor >= myClient->mfrMaxTokenCount)
//			{
//				myClient->mIsDataSet = false;
//				myClient->mIsProcessing = false;
//				myClient->mFs = InfClient::finish_state::INF_FINISH_STATE_TOKEN_LIMIT_REACHED;
//				llama_batch_clear(myClient->mfrBatch);
//				llama_kv_cache_seq_rm(mModelContext, myClient->mSequenceId, -1, -1);
//				//mClientsMutex.release();
//				mbase::lock_guard procMutex(mProcHandlerMutex);
//				mLogicHandlers.push_back(myClient);
//				continue;
//			}
//			
//			llama_decode(mModelContext, myClient->mfrBatch);
//
//			I32 modelVocab = 0;
//			I32 seqId = myClient->mSequenceId;
//			get_processed_model()->get_vocab_count(modelVocab);
//			float* logits = llama_get_logits_ith(mModelContext, myClient->mfrBatch.n_tokens - 1);
//
//			for (llama_token token_id = 0; token_id < modelVocab; ++token_id)
//			{ 
//				mPresetCandidates.emplace_back(llama_token_data{ token_id, logits[token_id], 0.0f });
//			}
//
//			// TODO: implement an object that will provide a token sampling interface
//			llama_token_data_array tokenCandidates = { mPresetCandidates.data(), mPresetCandidates.size(), false };
//
//			llama_sample_repetition_penalties(mModelContext, &tokenCandidates, myClient->mParsedTokens.data(), myClient->mParsedTokens.size(), 1.0, 0.0, 0.0);
//
//			mbase::vector<InfSamplingBase*> tmpSamplers;
//			myClient->get_sampling_order(tmpSamplers);
//
//			for(mbase::vector<InfSamplingBase*>::iterator It = tmpSamplers.begin(); It != tmpSamplers.end(); ++It)
//			{
//				InfSamplingBase* activeSampler = *It;
//				activeSampler->set_context(mModelContext);
//				activeSampler->set_token_array(&tokenCandidates);
//				activeSampler->apply_sampling();
//			}
//			
//			llama_token generatedToken;
//
//			generatedToken = llama_sample_token_greedy(mModelContext, &tokenCandidates);
//			// TODO: Handle unknown token generation
//			
//			if (llama_token_is_eog(get_processed_model()->get_raw_model(), generatedToken)) // || batch_cursor >= predictMax
//			{
//				I32 resultLength = llama_token_to_piece(get_processed_model()->get_raw_model(), generatedToken, myClient->mfrGeneratedToken.get_buffer(), 128, 0, true);
//				mbase::string strGeneratedToken(myClient->mfrGeneratedToken.get_buffer(), resultLength);
//				myClient->mfrGeneratedToken.set_cursor_pos(resultLength);
//				myClient->mIsDataSet = false;
//				myClient->mIsProcessing = false;
//				myClient->mFs = InfClient::finish_state::INF_FINISH_STATE_SUCCESS;
//				llama_batch_clear(myClient->mfrBatch);
//				mPresetCandidates.clear();
//				llama_kv_cache_seq_rm(mModelContext, myClient->mSequenceId, -1, -1);
//				//mClientsMutex.release();
//				mbase::lock_guard procMutex(mProcHandlerMutex);
//				mLogicHandlers.push_back(myClient);
//				continue;
//			}
//
//			I32 resultLength = llama_token_to_piece(get_processed_model()->get_raw_model(), generatedToken, myClient->mfrGeneratedToken.get_buffer(), 128, 0, true);
//			llama_batch_clear(myClient->mfrBatch);
//			llama_batch_add(myClient->mfrBatch, generatedToken, myClient->mfrBatchCursor, { seqId }, true);
//			mbase::string strGeneratedToken(myClient->mfrGeneratedToken.get_buffer(), resultLength);
//			// TODO: CHECK IF THE GENERATED TOKEN IS SPECIAL
//			myClient->mfrGeneratedToken.set_cursor_pos(resultLength);
//
//			myClient->mfrBatchCursor++;
//			mPresetCandidates.clear();
//			myClient->mIsLogicProcessed = false;
//			//mClientsMutex.release();
//
//			mbase::lock_guard procMutex(mProcHandlerMutex);
//			mLogicHandlers.push_back(myClient);
//		}
//	}
//}

MBASE_END
