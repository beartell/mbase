#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_processor.h>

MBASE_BEGIN

#define MBASE_INF_MODEL_RETURN_UNINITIALIZED \
if(!this->is_initialized())\
{\
	return flags::INF_MODEL_ERR_NOT_INITIALIZED;\
}

inf_proc_update_t::inf_proc_update_t() : mIsThreadRunning(false)
{

}

GENERIC inf_proc_update_t::destroy()
{
	mIsThreadRunning = false;
}

GENERIC inf_proc_update_t::run()
{
	mIsThreadRunning = true;
}

GENERIC inf_proc_update_t::on_call(user_data in_data)
{
	InfRegisteredProcStructure* freshProcStructure = reinterpret_cast<InfRegisteredProcStructure*>(in_data);
	while(mIsThreadRunning)
	{
		freshProcStructure->mProcessor->update_t();
	}
	delete freshProcStructure;
}

InfModel::InfModel() :
	mModel(NULL),
	mRegisteredProcessors(),
	mModelKvals(),
	mModelName(),
	mEndOfTokenString(),
	mUsrStart(),
	mSystemStart(),
	mAssistantStart(),
	mEndOfToken(0),
	mAiLoops(8)
{

}

InfModel::~InfModel()
{
	this->unload_model();
}

typename InfModel::iterator InfModel::begin() noexcept
{
	return mRegisteredProcessors.begin();
}

typename InfModel::iterator InfModel::end() noexcept
{
	return mRegisteredProcessors.end();
}

typename InfModel::const_iterator InfModel::begin() const noexcept
{
	return mRegisteredProcessors.begin();
}

typename InfModel::const_iterator InfModel::end() const noexcept
{
	return mRegisteredProcessors.end();
}

typename InfModel::const_iterator InfModel::cbegin() const noexcept
{
	return mRegisteredProcessors.cbegin();
}

typename InfModel::const_iterator InfModel::cend() const noexcept
{
	return mRegisteredProcessors.cend();
}

typename InfModel::reverse_iterator InfModel::rbegin() noexcept
{
	return mRegisteredProcessors.rbegin();
}

typename InfModel::reverse_iterator InfModel::rend() noexcept
{
	return mRegisteredProcessors.rend();
}

typename InfModel::const_reverse_iterator InfModel::crbegin() const noexcept
{
	return mRegisteredProcessors.crbegin();
}

typename InfModel::const_reverse_iterator InfModel::crend() const noexcept
{
	return mRegisteredProcessors.crend();
}

bool InfModel::is_initialized() const
{
	return (mModel != NULL);
}

llama_model* InfModel::get_raw_model()
{
	return mModel;
}

InfModel::flags InfModel::get_special_tokens(mbase::vector<inf_token>& out_tokens)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	for (I32 i = 0; i < llama_n_vocab(mModel); i++)
	{
		llama_token_attr lta = llama_token_get_attr(mModel, i);
		if(lta != LLAMA_TOKEN_ATTR_NORMAL)
		{
			out_tokens.push_back(i);
		}
	}
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_special_tokens(mbase::vector<mbase::string>& out_tokens)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	mbase::vector<inf_token> specialTokens;
	get_special_tokens(specialTokens);

	for(auto& n : specialTokens)
	{
		IBYTE myChars[128] = { 0 };
		I32 tokenLength = llama_token_to_piece(mModel, n, myChars, 128, 1, true);
		out_tokens.push_back(myChars);
	}
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_model_name(mbase::string& out_name)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_name = mModelKvals["general.basename"];
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_vocabulary_type(mbase::string& out_type)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_architecture(mbase::string& out_architecture)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_architecture = mModelKvals["general.architecture"];
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_finetune_type(mbase::string& out_type)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_type = mModelKvals["general.finetune"];
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_embedding_length(I32& out_length)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	mbase::string outArchitecture;
	get_architecture(outArchitecture);

	mbase::string totalKey = outArchitecture + ".embedding_length";
	mbase::string embeddingLengthString = mModelKvals[totalKey];
	out_length = embeddingLengthString.to_i32();
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_model_base_name(mbase::string& out_name)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_name = mModelKvals["general.basename"];
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
	out_start = mSystemStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_assistant_start(mbase::string& out_start)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_start = mAssistantStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_usr_start(mbase::string& out_start)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_start = mUsrStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_sys_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_end = mEndOfTokenString;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_assistant_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_end = mEndOfTokenString;
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_usr_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_end = mEndOfTokenString;
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

InfModel::flags InfModel::get_model_params(mbase::unordered_map<mbase::string, mbase::string>& out_params)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_params = mModelKvals;
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
	if(llama_token_get_attr(mModel, in_token) & LLAMA_TOKEN_ATTR_CONTROL)
	{
		return flags::INF_MODEL_SUCCESS;
	}
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::get_metadata_count(size_type& out_count)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_count = mModelKvals.size();
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::initialize(const mbase::string& in_model_path, const I32& in_gpu_layers)
{
	if(this->is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	if(!in_model_path.size())
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
	
	lmp.split_mode = LLAMA_SPLIT_MODE_NONE;
	mModel = llama_load_model_from_file(in_model_path.c_str(), lmp);
	if(!mModel)
	{
		return flags::INF_MODEL_ERR_CANT_LOAD_MODEL;
	}
	size_type modelParamCount = 0;
	get_model_param_count(modelParamCount);

	for (int i = 0; i < modelParamCount; i++)
	{
		char tempBuf[512] = { 0 };
		char outValue[512] = { 0 };
		llama_model_meta_key_by_index(mModel, i, tempBuf, 512);
		llama_model_meta_val_str(mModel, tempBuf, outValue, 512);
		mModelKvals.insert(mbase::pair(mbase::string(tempBuf), mbase::string(outValue)));
	}

	if(mModelKvals.find("general.basename") == mModelKvals.end())
	{
		mModelKvals["general.basename"] = mModelKvals["general.name"];
	}

	mbase::string& modelName = mModelKvals["general.basename"];

	mbase::vector<inf_token> tokenList;
	if(llama_token_eot(mModel) != -1)
	{
		tokenList.push_back(llama_token_eot(mModel));
	}
	if(llama_token_eos(mModel) != -1)
	{
		tokenList.push_back(llama_token_eos(mModel));
	}
	
	if(!tokenList.size())
	{
		// MEANS THIS IS NOT AN INSTRUCT MODEL
	}

	mEndOfToken = tokenList.front();
	char outValue[512] = { 0 };
	llama_token_to_piece(mModel, mEndOfToken, outValue, 512, 0, true);
	mEndOfTokenString = mbase::string(outValue) + '\n';
	// TODO: MAKE SURE TO CHECK IF WE HAVE ENOUGH MEMORY TO LOAD THE MODEL

	// POSSIBLE START CANDIDATES:
	// <|im_start|>
	// <|start_header_id|>
	// <|assistant|>
	// <|system|>
	// <|user|>
	mSystemStart = "SYSTEM: ";
	mAssistantStart = "ASSISTANT: ";
	mUsrStart = "USER: ";
	if (mModelKvals.find("tokenizer.chat_template") == mModelKvals.end())
	{
		// CHAT TEMPLATE NOT FOUND
		// MODEL IS POSSIBLY NON INSTRUCT
	}
	else
	{
		mbase::vector<mbase::string> sysBosCandidates = { "<|im_start|>", "<|start_header_id|>", "<|assistant|>", "<|system|>" };
		mbase::vector<mbase::string> mSpecialTokens;
		get_special_tokens(mSpecialTokens);

		for(auto& n : sysBosCandidates)
		{
			mbase::vector<mbase::string>::iterator foundToken = std::find(mSpecialTokens.begin(), mSpecialTokens.end(), n);
			if(foundToken != mSpecialTokens.end())
			{
				mSystemStart = *foundToken;
			}
		}

		if(mSystemStart == "<|im_start|>")
		{
			mSystemStart += "system\n";
			mAssistantStart = "<|im_start|>assistant\n";
		}

		else if(mSystemStart == "<|start_header_id|>")
		{
			mSystemStart += "system<|end_header_id|>";
			mAssistantStart = "<|start_header_id|>assistant<|end_header_id|>";
		}

		else if (mSystemStart == "<|system|>") 
		{
			
			mAssistantStart = "<|assistant|>";
		}

		mbase::vector<mbase::string> usrBosCandidates = { "<|im_start|>", "<|user|>", "<|start_header_id|>"};
		for (auto& n : usrBosCandidates)
		{
			mbase::vector<mbase::string>::iterator foundToken = std::find(mSpecialTokens.begin(), mSpecialTokens.end(), n);
			if (foundToken != mSpecialTokens.end())
			{
				mUsrStart = *foundToken;
			}
		}

		if(mUsrStart == "<|im_start|>")
		{
			mUsrStart += "user\n";
			mEndOfTokenString = "<|im_end|>\n";
		}

		else if(mUsrStart == "<|start_header_id|>")
		{
			mUsrStart += "user<|end_header_id|>";
			mEndOfTokenString = "<|eot_id|>";
		}

		else if(mUsrStart == "<|user|>")
		{
			mEndOfTokenString = "<|end|>";
		}
	}
	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::load_model(const mbase::string& in_model_path, const I32& in_gpu_layers)
{
	return initialize(in_model_path, in_gpu_layers);
}

InfModel::flags InfModel::unload_model()
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	// TODO: UNREGISTER ALL PROCESSORS GRACEFULLY

	for(processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end();)
	{
		InfRegisteredProcStructure* tmpRegisteredProcStructure = *It;
		tmpRegisteredProcStructure->mProcessor->_destroy(It);
	}

	llama_free_model(mModel);
	mModel = NULL;
	mModelKvals.clear();
	mModelName = "";
	mEndOfTokenString = "";
	mUsrStart = "";
	mSystemStart = "";
	mEndOfToken = 0;

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
	ctxParams.n_ubatch = in_params.mBatchSize / 8;

	llama_context* newModelContext = llama_new_context_with_model(mModel, ctxParams);
	if(!newModelContext)
	{
		return flags::INF_MODEL_ERR_UNABLE_REGISTER_PROCESSOR;
	}

	out_processor.mModelContext = newModelContext;
	out_processor.mProcessedModel = this;
	out_processor.mMaxClients = in_params.mMaxSequence;
	out_processor.resume();
	InfRegisteredProcStructure* freshProcStructure = new InfRegisteredProcStructure;
	freshProcStructure->mProcessor = &out_processor;
	freshProcStructure->mProcUpdateT.set_user_data(freshProcStructure);
	freshProcStructure->mProcUpdateT.run();
	mAiLoops.execute_job(freshProcStructure->mProcUpdateT);
	mRegisteredProcessors.push_back(freshProcStructure);

	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::unregister_processor(InfProcessor& in_processor)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	
	in_processor.destroy();

	return flags::INF_MODEL_SUCCESS;
}

InfModel::flags InfModel::_unregister_processor(InfProcessor& in_processor, iterator& _out_it)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	iterator It;
	for(iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
	{
		InfRegisteredProcStructure* tmpStruct = *It;
		if(tmpStruct->mProcessor == &in_processor)
		{
			break;
		}
	}

	if(It == mRegisteredProcessors.end())
	{
		return flags::INF_MODEL_ERR_PROCESSOR_NOT_FOUND;
	}

	_out_it = It;
	InfRegisteredProcStructure* tmpStruct = *_out_it;
	_out_it = mRegisteredProcessors.erase(_out_it);
	tmpStruct->mProcUpdateT.destroy();

	return flags::INF_MODEL_SUCCESS;
}

GENERIC InfModel::update()
{

}

MBASE_END