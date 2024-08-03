#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_processor.h>

MBASE_BEGIN

#define MBASE_INF_MODEL_RETURN_UNINITIALIZED \
if(!this->is_initialized())\
{\
	return flags::INF_MODEL_ERR_NOT_INITIALIZED;\
}

InfModel::InfModel() :
	mModel(NULL),
	mRegisteredProcessors()
{

}

InfModel::~InfModel()
{
	if(this->is_initialized())
	{
		this->unload_model();
	}
}

bool InfModel::is_initialized() const
{
	return (mModel != NULL);
}

llama_model* InfModel::get_raw_model()
{
	return mModel;
}

InfModel::flags InfModel::get_vocabulary_type(mbase::string& out_type)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_rope_type(mbase::string& out_type)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_sys_start(mbase::string& out_start)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_start = "<|assistant|>";
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_usr_start(mbase::string& out_start)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_start = "<|user|>";
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_sys_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	llama_token eosToken = llama_token_eot(mModel);
	char endToken[64] = { 0 };
	llama_token_to_piece(mModel, eosToken, endToken, 63, 0, false);
	out_end = endToken;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_usr_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	
	llama_token eosToken = llama_token_eot(mModel);
	char endToken[64] = { 0 };
	llama_token_to_piece(mModel, eosToken, endToken, 63, 0, false);
	out_end = endToken;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_vocab_count(I32& out_count)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_count = llama_n_vocab(mModel);
	return flags::INF_MODEL_SUCCESS;;
}

InfModel::flags InfModel::get_model_param_count(size_type& out_count)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_count = llama_model_meta_count(mModel);
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_model_params()
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	llama_add_bos_token(mModel);
	size_type modelParamCount = 0;
	get_model_param_count(modelParamCount);
	llama_token myTok = llama_token_bos(mModel);
	
	char tokenVal[512] = { 0 };
	llama_token_to_piece(mModel, myTok, tokenVal, 512, 0, 0);

	printf("bos token: %s\n", tokenVal);

	printf("%d\n", modelParamCount);
	for(int i = 0; i < modelParamCount; i++)
	{
		char tempBuf[512] = { 0 };
		char outValue[512] = { 0 };
		llama_model_meta_key_by_index(mModel, i, tempBuf, 512);
		llama_model_meta_val_str(mModel, tempBuf, outValue, 512);
		printf("%s -- %s\n", tempBuf, outValue);
	}

	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_size(size_type& out_size)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

bool InfModel::is_token_eof_generation(inf_token in_token)
{
	if(!this->is_initialized())
	{
		return false;
	}
	return llama_token_is_eog(mModel, in_token);
}

InfModel::flags InfModel::is_token_control(inf_token in_token)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_metadata_count(size_type& out_count)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::initialize(const mbase::string& in_model, const I32& in_gpu_layers)
{
	if(this->is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	if(!in_model.size())
	{
		return flags::INF_MODEL_ERR_MISSING_MODEL;
	}

	I32 nGpuLayers = in_gpu_layers;
	if(nGpuLayers < 0)
	{
		nGpuLayers = 999;
	}

	llama_model_params lmp = llama_model_default_params();
	lmp.n_gpu_layers = nGpuLayers;
	//lmp.split_mode = LLAMA_SPLIT_MODE_ROW;
	lmp.split_mode = LLAMA_SPLIT_MODE_LAYER;
	mModel = llama_load_model_from_file(in_model.c_str(), lmp);
	if(!mModel)
	{
		return flags::INF_MODEL_ERR_CANT_LOAD_MODEL;
	}

	// TODO: MAKE SURE TO CHECK IF WE HAVE ENOUGH MEMORY TO LOAD THE MODEL

	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::load_model(const mbase::string& in_model, const I32& in_gpu_layers)
{
	return initialize(in_model, in_gpu_layers);
}

InfModel::flags InfModel::unload_model()
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	// TODO: UNREGISTER ALL PROCESSORS GRACEFULLY

	llama_free_model(mModel);
	mModel = NULL;

	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::register_processor(InfProcessor& out_processor, InfProcInitParams in_params)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	if(out_processor.is_registered())
	{
		return flags::INF_MODEL_ERR_PROCESSOR_ALREADY_REGISTERED;
	}

	if (in_params.mContextLength < gInfProcessorMinCtxLength)
	{
		in_params.mContextLength = gInfProcessorMinCtxLength;
	}

	if (in_params.mBatchSize < gInfProcessorMinBatchSize)
	{
		in_params.mBatchSize = gInfProcessorMinBatchSize;
	}

	if (in_params.mMaxSequence < gInfProcessorMinSeq)
	{
		in_params.mMaxSequence = gInfProcessorMinSeq;
	}

	if (in_params.mThreadCount < gInfProcessorMinThreadCount)
	{
		in_params.mThreadCount = gInfProcessorMinThreadCount;
	}

	llama_context_params ctxParams = llama_context_default_params();
	ctxParams.n_ctx = in_params.mContextLength;
	ctxParams.n_batch = in_params.mBatchSize;
	ctxParams.n_seq_max = in_params.mMaxSequence;
	ctxParams.n_threads = in_params.mThreadCount;
	ctxParams.n_threads_batch = in_params.mThreadCount;
	ctxParams.n_ubatch = 32;

	llama_context* newModelContext = llama_new_context_with_model(mModel, ctxParams);
	if(!newModelContext)
	{
		return flags::INF_MODEL_ERR_UNABLE_REGISTER_PROCESSOR;
	}

	out_processor.mModelContext = newModelContext;
	out_processor.mProcessedModel = this;
	out_processor.mMaxClients = in_params.mMaxSequence;

	mRegisteredProcessors.push_back(&out_processor);

	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::unregister_processor(InfProcessor& in_processor)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	if(!in_processor.is_registered())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	if(in_processor.mProcessedModel != this)
	{
		return flags::INF_MODEL_ERR_PROCESSOR_BELONGS_TO_ANOTHER_MODEL;
	}

	return flags::INF_MODEL_SUCCESS;
}

MBASE_END