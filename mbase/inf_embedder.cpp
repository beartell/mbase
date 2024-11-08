#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_client.h>

MBASE_BEGIN

#define MBASE_INF_EMBEDDER_PROC_RETURN_UNREGISTERED \
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

InfEmbedderProcessor::InfEmbedderProcessor():
    mModelContext(NULL),
    mEmbeddingLength(0),
    mBatchSize(0),
    mThreadCount(0)
{
    mProcessorType = processor_type::EMBEDDER;
}

InfEmbedderProcessor::~InfEmbedderProcessor()
{

}

bool InfEmbedderProcessor::is_available() const
{
    
}

InfEmbedderProcessor::init_fail_code InfEmbedderProcessor::get_last_fail_code() const
{
    return init_fail_code::INVALID_MODEL_TYPE;
}

bool InfEmbedderProcessor::signal_init_method() const
{
    return mInitializeMethodSignal.get_signal();
}

bool InfEmbedderProcessor::signal_destroy_method() const
{
    return mDestroyMethodSignal.get_signal();
}

bool InfEmbedderProcessor::signal_init_fail_method() const
{
    return mInitializeFailSignal.get_signal();
}

bool InfEmbedderProcessor::signal_embedding_process() const
{
    return mEmbeddingSignal.get_signal();
}

bool InfEmbedderProcessor::signal_embedding_vector_generated() const
{
    return mVectorGenerated.get_signal();
}

const U32& InfEmbedderProcessor::get_embedding_length()
{
    return mEmbeddingLength;
}

const U32& InfEmbedderProcessor::get_max_token_length()
{
    return mContextLength;
}

InfClientTextToText* InfEmbedderProcessor::get_assigned_client()
{
    return mAssignedClient;
}

InfEmbedderProcessor::flags InfEmbedderProcessor::tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens)
{
    MBASE_INF_EMBEDDER_PROC_RETURN_UNREGISTERED;

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

InfEmbedderProcessor::flags InfEmbedderProcessor::execute_input(const inf_text_token_vector& in_tokens, bool in_abandon)
{
    MBASE_INF_EMBEDDER_PROC_RETURN_UNREGISTERED;

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
        mEmbeddingSignal.reset_signal_with_state();
        stop_processor();
    }
    mTokenizedInput = in_tokens;
    mEmbeddingSignal.set_signal_with_state();
    start_processor();

    return flags::INF_PROC_SUCCESS;
}

InfEmbedderProcessor::flags InfEmbedderProcessor::set_inference_client(InfClientTextToText* in_client)
{
    MBASE_INF_EMBEDDER_PROC_RETURN_UNREGISTERED;

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
		InfProcessorBase* clientProc = NULL;
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

InfEmbedderProcessor::flags InfEmbedderProcessor::initialize(
    InfModelTextToText* in_model,
    const mbase::string& in_context_id,
    const U32& in_context_length,
    const U32& in_batch_size,
    const U32& in_thread_count
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
    mContextIdentifier = in_context_id;
    mContextLength = in_context_length;
    mBatchSize = in_batch_size;
    mThreadCount = in_thread_count;
    
    mInitializeSignal.set_signal_with_state();
    on_initializing();
    start_processor();
    return flags::INF_PROC_INFO_INITIALIZING;
}

InfEmbedderProcessor::flags InfEmbedderProcessor::initialize_sync(
    InfModelTextToText* in_model,
    const mbase::string& in_context_id,
    const U32& in_context_length,
    const U32& in_batch_size,
    const U32& in_thread_count
)
{
    flags initializingErr = initialize(
        in_model,
        in_context_id,
        in_context_length,
        in_batch_size,
        in_thread_count
    );

    if(initializingErr != flags::INF_PROC_INFO_INITIALIZING)
    {
        return initializingErr;
    }

    while(signal_state_initializing()){}

    if(!is_registered())
    {
        return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;
    }

    return flags::INF_PROC_SUCCESS;
}

InfEmbedderProcessor::flags InfEmbedderProcessor::destroy()
{

}

InfEmbedderProcessor::flags InfEmbedderProcessor::destroy_sync()
{

}

GENERIC InfEmbedderProcessor::release_inference_client()
{

}

GENERIC InfEmbedderProcessor::_initialize_context()
{
    llama_context_params ctxParams = llama_context_default_params();
    ctxParams.n_ctx = mContextLength;
    ctxParams.n_batch = mContextLength;
    ctxParams.n_seq_max = 1;
    ctxParams.n_threads = mThreadCount;
    ctxParams.n_threads_batch = mThreadCount;
    ctxParams.n_ubatch = mBatchSize;
    ctxParams.embeddings = true;

    mInitializeFailSignal.reset_signal_with_state();

    InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);

	if(!t2tModel || !t2tModel->is_initialized())
	{
		mInitFailCode = init_fail_code::MODEL_NOT_INITIALIZED;
		mInitializeFailSignal.set_signal_with_state();
		mInitializeSignal.reset_signal_with_state();
		return;
	}

    mModelContext = llama_new_context_with_model(t2tModel->get_raw_model(), ctxParams);
    if(!mModelContext)
    {
        mInitFailCode = init_fail_code::NOT_ENOUGH_MEMORY;
        mInitializeFailSignal.set_signal_with_state();
        mInitializeSignal.reset_signal_with_state();
        return;
    }

    t2tModel->get_embedding_length(mEmbeddingLength);
    mIsRunning = true;
    mInitializeSignal.reset_signal_with_state();
    mIsRegistered = true;

    mInitializeMethodSignal.set_signal_with_state();
}

GENERIC InfEmbedderProcessor::_destroy_context()
{
    llama_free(mModelContext);
    mModelContext = NULL;
    mTokenizedInput.clear();
    mEmbeddingVector.clear();
    mBatchSize = 0;
	mThreadCount = 0;
    mIsRunning = false;
    mAssignedClient = NULL;
    mInitFailCode = init_fail_code::MODEL_NOT_INITIALIZED;

    mInitializeSignal.reset_signal_with_state();
    mEmbeddingSignal.reset_signal_with_state();
    mDestroySignal.reset_signal_with_state();
    mTargetModel_md_model = NULL;
    mIsRegistered = false;

    mDestroyMethodSignal.set_signal_with_state();
}

GENERIC InfEmbedderProcessor::_calculate_embeddings()
{
    llama_kv_cache_clear(mModelContext);
    mInputBatch = llama_batch_init(mContextLength, 0, 1);

    for(size_type i = 0; i < mTokenizedInput.size(); ++i)
    {
        common_batch_add(mInputBatch, mTokenizedInput[i], i, {0}, true);
    }

    InfModelTextToText* t2tModel = static_cast<InfModelTextToText*>(this->mTargetModel_md_model);
    llama_model* rawModel = t2tModel->get_raw_model();
    

    if(llama_model_has_encoder(rawModel) && !llama_model_has_decoder(rawModel))
    {
        llama_encode(mModelContext, mInputBatch);
    }
    
    else if(!llama_model_has_encoder(rawModel) && llama_model_has_decoder(rawModel))
    {
        llama_decode(mModelContext, mInputBatch);
    }


}

GENERIC InfEmbedderProcessor::update()
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
        if(signal_embedding_vector_generated())
        {
            mEmbeddingSignal.reset_signal_with_state();
            InfClientTextToText* t2tClient = get_assigned_client();
			if(!t2tClient)
			{
				// means client is abandoned the context
				return;
			}
            
            t2tClient->on_embedding_data(mEmbeddingVector.data(), mEmbeddingVector.size());
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

GENERIC InfEmbedderProcessor::update_t() 
{
    while(is_processor_running())
    {
        if(is_registered())
        {
            if(signal_destroying())
            {
                _destroy_context();
            }

            if(is_running())
            {
                if(signal_embedding_process())
                {
                    _calculate_embeddings();
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