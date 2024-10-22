#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_processor.h>
#include <iostream>

MBASE_BEGIN

#define MBASE_INF_MODEL_RETURN_UNINITIALIZED \
if(this->signal_state_initializing())\
{\
	return flags::INF_MODEL_INFO_INITIALIZING_MODEL;\
}\
if(!this->is_initialized())\
{\
	return flags::INF_MODEL_ERR_NOT_INITIALIZED;\
}\
if(this->signal_state_destroying())\
{\
	return flags::INF_MODEL_INFO_DESTROYING_MODEL;\
}

InfModelBase::InfModelBase() :
	mIsInitialized(false)
{
}

typename InfModelBase::iterator InfModelBase::begin() noexcept
{
	return mRegisteredProcessors.begin();
}

typename InfModelBase::iterator InfModelBase::end() noexcept
{
	return mRegisteredProcessors.end();
}

typename InfModelBase::const_iterator InfModelBase::begin() const noexcept
{
	return mRegisteredProcessors.begin();
}

typename InfModelBase::const_iterator InfModelBase::end() const noexcept
{
	return mRegisteredProcessors.end();
}

typename InfModelBase::const_iterator InfModelBase::cbegin() const noexcept
{
	return mRegisteredProcessors.cbegin();
}

typename InfModelBase::const_iterator InfModelBase::cend() const noexcept
{
	return mRegisteredProcessors.cend();
}

typename InfModelBase::reverse_iterator InfModelBase::rbegin() noexcept
{
	return mRegisteredProcessors.rbegin();
}

typename InfModelBase::reverse_iterator InfModelBase::rend() noexcept
{
	return mRegisteredProcessors.rend();
}

typename InfModelBase::const_reverse_iterator InfModelBase::crbegin() const noexcept
{
	return mRegisteredProcessors.crbegin();
}

typename InfModelBase::const_reverse_iterator InfModelBase::crend() const noexcept
{
	return mRegisteredProcessors.crend();
}

bool InfModelBase::is_initialized() const
{
	return mIsInitialized;
}

bool InfModelBase::signal_state_initializing() const
{
	return mInitializeSignal.get_signal_state();
}

bool InfModelBase::signal_state_destroying() const
{
	return mDestroySignal.get_signal_state();
}

bool InfModelBase::signal_initializing() const
{
	return mInitializeSignal.get_signal();
}

bool InfModelBase::signal_destroying() const
{
	return mDestroySignal.get_signal();
}

InfModelTextToText::InfModelTextToText() :
	mModel(NULL),
	mEndOfToken(0),
	mModelKvals(),
	mEndOfTokenString(),
	mUsrStart(),
	mSystemStart(),
	mAssistantStart(),
	mOccupiedContext(0)
{

}

InfModelTextToText::~InfModelTextToText()
{
	if (!is_initialized())
	{
		
	}
	else
	{
		destroy();
		while (signal_state_destroying())
		{
		}
	}
}

bool InfModelTextToText::is_available(const U32& in_context_size)
{
	if (this->signal_state_initializing())
	{
		return false; 
	}
	if (!this->is_initialized())
	{
		return false; 
	}
	if (this->signal_state_destroying())
	{
		return false; 
	}
	if(get_occupied_context_size() + in_context_size > get_total_context_size())
	{
		return false;
	}
	return true;
}

bool InfModelTextToText::signal_init_method() const
{
	return mInitMethodSignal.get_signal();
}

bool InfModelTextToText::signal_destroy_method() const
{
	return mDestroyMethodSignal.get_signal();
}

llama_model* InfModelTextToText::get_raw_model()
{
	return mModel;
}

InfModelTextToText::flags InfModelTextToText::get_special_tokens(mbase::vector<inf_token>& out_tokens)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	_get_special_tokens(out_tokens);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_special_tokens(mbase::vector<mbase::string>& out_tokens)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	_get_special_tokens(out_tokens);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_model_name(mbase::string& out_name)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_name = mModelKvals["general.basename"];
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_vocabulary_type(mbase::string& out_type)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_architecture(mbase::string& out_architecture)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_architecture = mModelKvals["general.architecture"];
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_finetune_type(mbase::string& out_type)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_type = mModelKvals["general.finetune"];
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_embedding_length(I32& out_length)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	mbase::string outArchitecture;
	get_architecture(outArchitecture);

	mbase::string totalKey = outArchitecture + ".embedding_length";
	mbase::string embeddingLengthString = mModelKvals[totalKey];
	out_length = embeddingLengthString.to_i32();
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_rope_type(mbase::string& out_type)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;

	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_sys_start(mbase::string& out_start)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_start = mSystemStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_sys_start(mbase::vector<inf_token>& out_tokens)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_tokens = mSystemStartTokenized;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_assistant_start(mbase::string& out_start)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_start = mAssistantStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_assistant_start(mbase::vector<inf_token>& out_tokens)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_tokens = mAssistantStartTokenized;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_usr_start(mbase::string& out_start)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_start = mUsrStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_usr_start(mbase::vector<inf_token>& out_tokens)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_tokens = mUserStartTokenized;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_sys_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_end = mEndOfTokenString;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_assistant_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_end = mEndOfTokenString;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_usr_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_end = mEndOfTokenString;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_eot_token(inf_token& out_token)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_token = llama_token_eot(mModel);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_lf_token(inf_token& out_token)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_token = llama_token_nl(mModel);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_vocab_count(I32& out_count)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_count = llama_n_vocab(mModel);
	return flags::INF_MODEL_SUCCESS;;
}

InfModelTextToText::flags InfModelTextToText::get_model_param_count(size_type& out_count)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_count = llama_model_meta_count(mModel);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_model_params(mbase::unordered_map<mbase::string, mbase::string>& out_params)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_params = mModelKvals;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_size(size_type& out_size)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

bool InfModelTextToText::is_token_eof_generation(inf_token in_token)
{
	if (!this->is_initialized())
	{
		return false;
	}
	return llama_token_is_eog(mModel, in_token);
}

InfModelTextToText::flags InfModelTextToText::is_token_special(const mbase::string& in_string)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;

	mbase::vector<mbase::string> specialTokens;
	get_special_tokens(specialTokens);

	if (std::find(specialTokens.begin(), specialTokens.end(), in_string) == specialTokens.end())
	{
		return flags::INF_MODEL_ERR_GENERIC;
	}

	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::is_token_control(inf_token in_token)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	if (llama_token_get_attr(mModel, in_token) & LLAMA_TOKEN_ATTR_CONTROL)
	{
		return flags::INF_MODEL_SUCCESS;
	}
	return flags::INF_MODEL_ERR_GENERIC;
}

InfModelTextToText::flags InfModelTextToText::get_metadata_count(size_type& out_count)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_count = mModelKvals.size();
	return flags::INF_MODEL_SUCCESS;
}

U32 InfModelTextToText::get_total_context_size()
{
	return mTotalContextSize;
}

U32 InfModelTextToText::get_occupied_context_size()
{
	return mOccupiedContext;
}

InfModelTextToText::flags InfModelTextToText::initialize_model(const mbase::string& in_path, const U32& in_total_context_size, I32 in_gpu_layers)
{
	if(is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	if(signal_state_initializing())
	{
		return flags::INF_MODEL_INFO_INITIALIZING_MODEL;
	}

	if(in_gpu_layers == -1)
	{
		in_gpu_layers = 0;
	}
	// TODO: Check if the given total context size is too small

	mSuppliedParams = llama_model_default_params();
	mTotalContextSize = in_total_context_size;
	mSuppliedParams.n_gpu_layers = in_gpu_layers;
	mSuppliedParams.split_mode = LLAMA_SPLIT_MODE_NONE;

	mModelPath = in_path;

	mInitializeSignal.set_signal_with_state();
	start_processor();
	return flags::INF_MODEL_INFO_INITIALIZING_MODEL;
}

InfModelTextToText::flags InfModelTextToText::initialize_model_sync(const mbase::string& in_path, const U32& in_total_context_size, I32 in_gpu_layers)
{
	initialize_model(in_path, in_total_context_size, in_gpu_layers);

	while(signal_state_initializing())
	{
		
	}

	if(!is_initialized())
	{
		return flags::INF_MODEL_ERR_CANT_LOAD_MODEL;
	}

	mInitMethodSignal.reset_signal_with_state();
	on_initialize();

	return flags::INF_MODEL_INFO_INITIALIZING_MODEL;
}

InfModelTextToText::flags InfModelTextToText::destroy()
{
	if(!is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	if(signal_state_destroying())
	{
		return flags::INF_MODEL_INFO_DESTROYING_MODEL;
	}

	start_processor();
	mDestroySignal.set_signal_with_state();
	return flags::INF_MODEL_INFO_DESTROYING_MODEL;
}

InfModelTextToText::flags InfModelTextToText::destroy_sync()
{
	if(!is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	destroy();
	while(signal_state_destroying())
	{
	}
	
	mDestroyMethodSignal.reset_signal_with_state();
	on_destroy();

	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::register_context_process(InfTextToTextProcessor* in_processor, U32 in_context_length)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	if(!in_processor || !in_context_length)
	{
		return flags::INF_MODEL_ERR_INVALID_INPUT;
	}
	
	if (in_processor->is_registered())
	{
		return flags::INF_MODEL_ERR_PROCESSOR_ALREADY_REGISTERED;
	}

	if (in_context_length < gProcessorMinimumTokenCount)
	{
		return flags::INF_MODEL_ERR_INVALID_CONTEXT_LENGTH;
	}

	if(in_processor->signal_state_initializing())
	{
		return flags::INF_MODEL_INFO_REGISTERING_PROCESSOR;
	}

	if(in_processor->signal_state_destroying())
	{
		return flags::INF_MODEL_INFO_PROCESSOR_IS_BEING_DESTROYED;
	}
	
	if(mOccupiedContext + in_context_length > mTotalContextSize)
	{
		return flags::INF_MODEL_ERR_MODEL_CONTEXT_FULL;
	}

	mOccupiedContext += in_context_length;

	in_processor->initialize(this, in_context_length, mbase::string::generate_uuid());
	mProcessorListMutex.acquire();
	mRegisteredProcessors.push_back(in_processor);
	mProcessorListMutex.release();
	return flags::INF_MODEL_INFO_REGISTERING_PROCESSOR;
}

GENERIC InfModelTextToText::_initialize_model()
{
	// TODO: INITIALIZE THE MODEL
	mModel = llama_load_model_from_file(mModelPath.c_str(), mSuppliedParams);
	if (!mModel)
	{
		mInitializeSignal.reset_signal_with_state();
		return;
	}
	size_type modelParamCount = llama_model_meta_count(mModel);

	for (int i = 0; i < modelParamCount; i++)
	{
		char tempBuf[512] = { 0 };
		char outValue[512] = { 0 };
		llama_model_meta_key_by_index(mModel, i, tempBuf, 512);
		llama_model_meta_val_str(mModel, tempBuf, outValue, 512);
		mModelKvals.insert(mbase::pair(mbase::string(tempBuf), mbase::string(outValue)));
	}

	if (mModelKvals.find("general.basename") == mModelKvals.end())
	{
		mModelKvals["general.basename"] = mModelKvals["general.name"];
		if(!mModelKvals["general.basename"].size())
		{
			mModelKvals["general.basename"] = mModelKvals["general.base_model.0.name"];
			// if this is also an empty string, I don't know what the fuck to do anymore...
		}
	}
	
	mbase::string& modelName = mModelKvals["general.basename"];
	mbase::vector<inf_token> tokenList;
	if (llama_token_eot(mModel) != -1)
	{
		tokenList.push_back(llama_token_eot(mModel));
	}

	if (llama_token_eos(mModel) != -1)
	{
		tokenList.push_back(llama_token_eos(mModel));
	}

	if (!tokenList.size())
	{
		// MEANS THIS IS NOT AN INSTRUCT MODEL
	}
	mEndOfToken = tokenList.front();
	char outValue[512] = { 0 };
	llama_token_to_piece(mModel, mEndOfToken, outValue, 512, 0, true);
	mEndOfTokenString = mbase::string(outValue) + '\n';
	mSystemStart = "SYSTEM: ";
	mAssistantStart = "ASSISTANT: ";
	mUsrStart = "USER: ";

	mbase::vector<mbase::string> sysBosCandidates = { "<|im_start|>", "<|start_header_id|>", "<|assistant|>", "<|system|>" };
	mbase::vector<mbase::string> mSpecialTokens;
	for (I32 i = 0; i < llama_n_vocab(mModel); i++)
	{
		llama_token_attr lta = llama_token_get_attr(mModel, i);
		
		if (lta != LLAMA_TOKEN_ATTR_NORMAL)
		{
			IBYTE myChars[128] = { 0 };
			I32 tokenLength = llama_token_to_piece(mModel, i, myChars, 128, 1, true);
			mSpecialTokens.push_back(mbase::string(myChars, tokenLength));
			//out_tokens.push_back(i);
		}
	}

	for (auto& n : sysBosCandidates)
	{
		mbase::vector<mbase::string>::iterator foundToken = std::find(mSpecialTokens.begin(), mSpecialTokens.end(), n);
		if (foundToken != mSpecialTokens.end())
		{
			mSystemStart = *foundToken;
		}
	}

	if (mSystemStart == "<|im_start|>")
	{
		mSystemStart += "system\n";
		mAssistantStart = "<|im_start|>assistant\n";
	}

	else if (mSystemStart == "<|start_header_id|>")
	{
		mSystemStart += "system<|end_header_id|>\n";
		mAssistantStart = "<|start_header_id|>assistant<|end_header_id|>\n";
	}

	else if (mSystemStart == "<|system|>")
	{
		mAssistantStart = "<|assistant|>";
	}

	mbase::vector<mbase::string> usrBosCandidates = { "<|im_start|>", "<|user|>", "<|start_header_id|>" };
	for (auto& n : usrBosCandidates)
	{
		mbase::vector<mbase::string>::iterator foundToken = std::find(mSpecialTokens.begin(), mSpecialTokens.end(), n);
		if (foundToken != mSpecialTokens.end())
		{
			mUsrStart = *foundToken;
		}
	}

	if (mUsrStart == "<|im_start|>")
	{
		mUsrStart += "user\n";
		mEndOfTokenString = "<|im_end|>\n";
	}

	else if (mUsrStart == "<|start_header_id|>")
	{
		mUsrStart += "user<|end_header_id|>\n";
		mEndOfTokenString = "<|eot_id|>\n";
	}

	else if (mUsrStart == "<|user|>")
	{
		mEndOfTokenString = "<|end|>";
	}
	

	mbase::vector<inf_token> tokenArray(32);
	I32 tokenCount = llama_tokenize(mModel, mSystemStart.c_str(), mSystemStart.size(), tokenArray.data(), 32, true, true);

	if(tokenCount > 0)
	{
		tokenArray.resize(tokenCount);
		mSystemStartTokenized = tokenArray;
	}

	tokenCount = llama_tokenize(mModel, mAssistantStart.c_str(), mAssistantStart.size(), tokenArray.data(), 32, true, true);

	if(tokenCount > 0)
	{
		tokenArray.resize(tokenCount);
		mAssistantStartTokenized = tokenArray;
	}

	tokenCount = llama_tokenize(mModel, mUsrStart.c_str(), mUsrStart.size(), tokenArray.data(), 32, true, true);

	if(tokenCount > 0)
	{
		tokenArray.resize(tokenCount);
		mUserStartTokenized = tokenArray;
	}
	
	mIsInitialized = true;
	mInitializeSignal.reset_signal_with_state();
	mInitMethodSignal.set_signal();
}

GENERIC InfModelTextToText::_destroy_model()
{
	mbase::lock_guard tmpListMutex(mProcessorListMutex);
	for (context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end();)
	{
		InfProcessorBase* baseProcessor = *It;
		InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(baseProcessor);
		t2tProcessor->destroy();
		It = mRegisteredProcessors.erase(It);
	}
	llama_free_model(mModel);
	mModel = NULL;
	mEndOfTokenString.clear();
	mUsrStart.clear();
	mModelKvals.clear();
	mSystemStart.clear();
	mAssistantStart.clear();
	mModelPath.clear();
	mEndOfToken = 0;
	mOccupiedContext = 0;
	mModelTimer.clear_timers();

	/* RESET ALL SIGNALS */
	mInitializeSignal.reset_signal_with_state();
	mIsProcessorRunning = false;
	mDestroySignal.reset_signal_with_state();

	mIsInitialized = false;
	mDestroyMethodSignal.set_signal();
}

GENERIC InfModelTextToText::_get_special_tokens(mbase::vector<inf_token>& out_tokens)
{
	for (I32 i = 0; i < llama_n_vocab(mModel); i++)
	{
		llama_token_attr lta = llama_token_get_attr(mModel, i);
		if (lta != LLAMA_TOKEN_ATTR_NORMAL)
		{
			out_tokens.push_back(i);
		}
	}
}
GENERIC InfModelTextToText::_get_special_tokens(mbase::vector<mbase::string>& out_tokens)
{
	mbase::vector<inf_token> specialTokens;
	get_special_tokens(specialTokens);
	for (auto& n : specialTokens)
	{
		IBYTE myChars[128] = { 0 };
		I32 tokenLength = llama_token_to_piece(mModel, n, myChars, 128, 1, true);
		out_tokens.push_back(myChars);
	}
}

GENERIC InfModelTextToText::update()
{
	// load and unload control
	if(signal_init_method())
	{
		stop_processor();
		mInitMethodSignal.reset_signal_with_state();
		on_initialize();
	}

	if(signal_destroy_method())
	{
		mDestroyMethodSignal.reset_signal_with_state();
		on_destroy();
	}

	mbase::lock_guard tmpListMutex(mProcessorListMutex);
	for(context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end();)
	{
		InfProcessorBase* baseProcessor = *It;
		InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(baseProcessor);
		if(t2tProcessor->is_registered())
		{
			if(!t2tProcessor->signal_state_destroying())
			{
				t2tProcessor->update();
			}
		}
		else
		{
			if(!t2tProcessor->signal_state_initializing())
			{
				mOccupiedContext -= t2tProcessor->get_context_size();
				if(t2tProcessor->signal_init_fail_method())
				{
					t2tProcessor->on_initialize_fail(t2tProcessor->get_last_fail_code());
				}
				if(t2tProcessor->signal_destroy_method())
				{
					t2tProcessor->update(); // one last update to invoke the destroy method
				}
				It = mRegisteredProcessors.erase(It);
				continue;
			}
		}
		++It;
	}
}

GENERIC InfModelTextToText::update_t()
{
	while(is_processor_running())
	{
		// THIS PART IS ONLY FOR LOADING AND UNLOADING THE MODEL
		if(is_initialized())
		{
			if(signal_destroying())
			{
				_destroy_model();
			}
		}
		else
		{
			if(signal_initializing())
			{
				_initialize_model();
			}
		}
		mbase::sleep(50); // Since this loop will not be invoked much outside of (init and destroy), we may decrease its speed 
	}
}

MBASE_END