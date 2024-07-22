#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_model.h>
#include <common/common.h>

MBASE_BEGIN

#define MBASE_INF_PROC_RETURN_UNINITIALIZED \
if(!this->is_initialized())\
{\
	return flags::INF_PROC_ERR_NOT_INITIALIZED;\
}

static U32 gInfProcMaxTokenLength = 128;

InfProcessor::InfProcessor() :
	mProcessedHandlers(),
	mFinishHandlers(),
	mPresetCandidates(),
	mGeneratedToken(gInfProcMaxTokenLength),
	mRegisteredClients(),
	mProcessedModel(NULL),
	mModelContext(NULL),
	mProcessorId(0),
	mClientsMutex()
{
}

InfProcessor::~InfProcessor()
{
	this->destroy();
}

bool InfProcessor::is_initialized() const
{
	return (mModelContext != NULL);
}

InfProcessor::flags InfProcessor::get_client_count(I32& out_size)
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;
	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::get_max_clients(I32& out_size) 
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;
	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::get_process_thread_count(I32& out_count)
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;
	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::get_batch_size(I32& out_size)
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;
	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::initialize(InfModel* in_model, InfProcInitParams in_params)
{
	if(this->is_initialized())
	{
		return flags::INF_PROC_ERR_ALREADY_INITIALIZED;
	}

	if(!in_model->is_initialized())
	{
		return flags::INF_PROC_ERR_MODEL_IS_NOT_INITIALIZED;
	}

	if(in_params.mContextLength < gInfProcessorMinCtxLength)
	{
		in_params.mContextLength = gInfProcessorMinCtxLength;
	}

	if(in_params.mBatchSize < gInfProcessorMinBatchSize)
	{
		in_params.mBatchSize = gInfProcessorMinBatchSize;
	}

	if(in_params.mMaxSequence < gInfProcessorMinSeq)
	{
		in_params.mMaxSequence = gInfProcessorMinSeq;
	}

	if(in_params.mThreadCount < gInfProcessorMinThreadCount)
	{
		in_params.mThreadCount = gInfProcessorMinThreadCount;
	}

	// TODO: CHECK IF WE HAVE ENOUGH MEMORY TO CREATE CONTEXT

	mProcessedModel = in_model;

	llama_context_params ctxParams = llama_context_default_params();
	ctxParams.n_ctx = in_params.mContextLength;
	ctxParams.n_batch = in_params.mBatchSize;
	ctxParams.n_seq_max = in_params.mMaxSequence;
	ctxParams.n_threads = in_params.mThreadCount;
	ctxParams.n_threads_batch = in_params.mThreadCount;
	ctxParams.n_ubatch = 512;

	mModelContext = llama_new_context_with_model(mProcessedModel->get_raw_model(), ctxParams);
	if(!mModelContext)
	{
		return flags::INF_PROC_ERR_NOT_INITIALIZED;
	}
	
	mProcessorId = gInfProcessorIdCounter++;
	mMaxClients = ctxParams.n_seq_max;

	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::destroy()
{
	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::tokenize_input(CBYTEBUFFER in_data, size_type in_size, mbase::vector<inf_token>& out_tokens)
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;

	if(!in_size)
	{
		return flags::INF_PROC_ERR_INVALID_TOKEN_SIZE;
	}

	mbase::vector<inf_token> tokenizedInput(in_size * 4);
	I32 tokenCount = llama_tokenize(mProcessedModel->get_raw_model(), in_data, in_size, tokenizedInput.data(), in_size * 4, false, true);
	if(tokenCount == -1)
	{
		return flags::INF_PROC_ERR_INVALID_TOKEN_INPUT;
	}
	tokenizedInput.resize(tokenCount);
	out_tokens = std::move(tokenizedInput);
	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::register_client(CBYTEBUFFER in_data, size_type in_size, InfClient& out_client)
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;

	if(out_client.is_registered())
	{
		return flags::INF_PROC_ERR_CLIENT_ALREADY_REGISTERED;
	}

	if(mRegisteredClients.size() == mMaxClients)
	{
		return flags::INF_PROC_ERR_CLIENT_LIMIT_REACHED;
	}

	flags retCode = flags::INF_PROC_SUCCESS;
	out_client.mIsDataSet = true;
	if(tokenize_input(in_data, in_size, out_client.mParsedTokens) != flags::INF_PROC_SUCCESS)
	{
		out_client.mIsDataSet = false;
		retCode = flags::INF_PROC_WARN_UNABLE_TO_TOKENIZE_INPUT;
	}
	
	mClientsMutex.acquire();
	mRegisteredClients.push_back(&out_client);
	out_client.mSelfIter = mRegisteredClients.end_node();
	mClientsMutex.release();

	out_client.on_register();
	out_client.mHostProcessor = this;
	return retCode;
}

InfProcessor::flags InfProcessor::register_client(const mbase::vector<inf_token>& in_data, InfClient& out_client)
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;

	if (out_client.is_registered())
	{
		return flags::INF_PROC_ERR_CLIENT_ALREADY_REGISTERED;
	}

	if (mRegisteredClients.size() == mMaxClients)
	{
		return flags::INF_PROC_ERR_CLIENT_LIMIT_REACHED;
	}

	if(in_data.size())
	{
		out_client.mParsedTokens = in_data;
		out_client.mIsDataSet = true;
	}
	mClientsMutex.acquire();
	mRegisteredClients.push_back(&out_client);
	out_client.mSelfIter = mRegisteredClients.end_node();
	mClientsMutex.release();

	out_client.on_register();
	out_client.mHostProcessor = this;
	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::register_client(InfClient& out_client)
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;

	if (out_client.is_registered())
	{
		return flags::INF_PROC_ERR_CLIENT_ALREADY_REGISTERED;
	}

	if (mRegisteredClients.size() == mMaxClients)
	{
		return flags::INF_PROC_ERR_CLIENT_LIMIT_REACHED;
	}

	mClientsMutex.acquire();
	mRegisteredClients.push_back(&out_client);
	out_client.mSelfIter = mRegisteredClients.end_node();
	mClientsMutex.release();

	out_client.on_register();
	out_client.mHostProcessor = this;
	return flags::INF_PROC_SUCCESS;
}

InfProcessor::flags InfProcessor::unregister_client(InfClient& in_client)
{
	MBASE_INF_PROC_RETURN_UNINITIALIZED;

	if (!in_client.is_registered())
	{
		return flags::INF_PROC_SUCCESS;
	}

	if(in_client.mHostProcessor != this)
	{
		return flags::INF_PROC_ERR_BELONGS_TO_ANOTHER_PROCESSOR;
	}
	mbase::lock_guard mGuard(mClientsMutex);

	return flags::INF_PROC_SUCCESS;
}

GENERIC InfProcessor::update()
{
	if(!this->is_initialized())
	{
		return;
	}

	for(logic_handlers::iterator It = mProcessedHandlers.begin(); It != mProcessedHandlers.end(); ++It)
	{
		InfClient* myClient = *It;
		mbase::char_stream& myCharacterStream = myClient->mGeneratedToken;

		size_type bufferSize = myCharacterStream.get_pos();
		CBYTEBUFFER bufferContent = myCharacterStream.get_buffer();
		myClient->on_write(bufferContent, bufferSize);
		myCharacterStream.set_cursor_front();
		myClient->mIsLogicProcessed = true;
	}

	for(logic_handlers::iterator It = mFinishHandlers.begin(); It != mFinishHandlers.end(); ++It)
	{
		InfClient* myClient = *It;
		myClient->on_finish(myClient->mGeneratedTokenCount);
		myClient->mGeneratedTokenCount = 0;
	}

	mProcessedHandlers.clear();
	mFinishHandlers.clear();
}

GENERIC InfProcessor::update(InfClient& in_client)
{
	if (!this->is_initialized())
	{
		return;
	}
}

GENERIC InfProcessor::update_t()
{
	if (!this->is_initialized())
	{
		return;
	}

	for (client_list::iterator It = mRegisteredClients.begin(); It != mRegisteredClients.end(); ++It)
	{
		InfClient* myClient = *It;
		if (!myClient->is_logic_processed())
		{
			continue;
		}

		else if (!myClient->is_data_set())
		{
			continue;
		}

		if (myClient->is_processing())
		{
			// TODO: cut the client if predictCount >= predictMax

			llama_batch& clientBatch = myClient->mBatch;
			I32 modelVocab = 0;
			mProcessedModel->get_vocab_count(modelVocab);

			float* logits = llama_get_logits_ith(mModelContext, myClient->mBatch.n_tokens - 1);
			for (llama_token token_id = 0; token_id < modelVocab; ++token_id)
			{
				mPresetCandidates.emplace_back(llama_token_data{ token_id, logits[token_id], 0.0f });
			}

			// TODO: implement an object that will provide a token sampling interface
			llama_token_data_array tokenCandidates = { mPresetCandidates.data(), mPresetCandidates.size(), false };
			const llama_token generatedToken = llama_sample_token_greedy(mModelContext, &tokenCandidates);

			if (mProcessedModel->is_token_eof_generation(generatedToken) == InfModel::flags::INF_MODEL_SUCCESS) // || batch_cursor >= predictMax
			{
				myClient->mIsDataSet = false;
				myClient->mIsProcessing = false;
				llama_kv_cache_seq_rm(mModelContext, myClient->mSequenceId, -1, -1);
				mFinishHandlers.push_back(myClient);
				continue;
			}

			I32 resultLength = llama_token_to_piece(mProcessedModel->get_raw_model(), generatedToken, mGeneratedToken.get_buffer(), 128, false);
			++myClient->mGeneratedTokenCount;
			myClient->mGeneratedToken = mGeneratedToken;
			myClient->mGeneratedToken.set_cursor_pos(resultLength);
			mGeneratedToken.set_cursor_front();

			llama_batch_clear(clientBatch);
			llama_batch_add(clientBatch, generatedToken, myClient->mBatchCursor, { 0 }, true);

			++myClient->mBatchCursor;
			mProcessedHandlers.push_back(myClient);
			myClient->mIsLogicProcessed = false;
			llama_decode(mModelContext, clientBatch);
		}
		else
		{
			mbase::vector<InfProcessor::inf_token>& generatedTokens = myClient->mParsedTokens;
			if (!generatedTokens.size())
			{
				continue;
			}

			for (size_type i = 0; i < generatedTokens.size(); ++i)
			{
				llama_batch_add(myClient->mBatch, generatedTokens[i], i, { myClient->mSequenceId }, false);
			}
			myClient->mBatch.logits[myClient->mBatch.n_tokens - 1] = true;
			llama_decode(mModelContext, myClient->mBatch);
			myClient->mBatchCursor = myClient->mBatch.n_tokens;
			myClient->mIsProcessing = true;
		}
	}
}

GENERIC InfProcessor::update_t(InfClient& in_client)
{
	if (!this->is_initialized())
	{
		return;
	}
}

MBASE_END
