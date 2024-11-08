#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/inference/inf_chat_templates.h>

MBASE_BEGIN

#define MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED \
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

InfModelTextToText::InfModelTextToText() :
	mModel(NULL),
	mEndOfToken(0),
	mUsrStart(),
	mSystemStart(),
	mAssistantStart(),
	mOccupiedContext(0),
	mBlockCount(0),
	mHeadCount(0),
	mEmbeddingLength(0),
	mQuantizationCoefficient(0.0f),
	mModelSize(0),
	mHasEmbeddedSystemPrompt(false),
	mIsEmbeddingModel(false)
{

}

InfModelTextToText::~InfModelTextToText()
{
	stop_processor();
	if (!is_initialized())
	{
		
	}
	else
	{
		mbase::lock_guard tmpListMutex(mProcessorListMutex);
		for (context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
		{
			InfTextToTextProcessor* baseProcessor = static_cast<InfTextToTextProcessor*>(*It);
			baseProcessor->stop_processor();
			baseProcessor->destroy_sync();
		}
		
		for(context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
		{
			InfProcessorBase* baseProcessor = *It;
			baseProcessor->update(); // Update all processors
		}
		llama_free_model(mModel);
	}
}

bool InfModelTextToText::is_available(const U32& in_context_size) const
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

bool InfModelTextToText::is_embedding_model() const
{
	return mIsEmbeddingModel;
}

bool InfModelTextToText::signal_init_fail_method() const
{
	return mInitFailSignal.get_signal();
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

InfModelTextToText::flags InfModelTextToText::get_special_tokens(mbase::vector<inf_text_token>& out_tokens)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	_get_special_tokens(out_tokens);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_special_tokens(mbase::vector<mbase::string>& out_tokens)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	_get_special_tokens(out_tokens);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_model_name(mbase::string& out_name)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_name = mModelName;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_architecture(mbase::string& out_architecture)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_architecture = mModelArchitecture;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_embedding_length(U32& out_length)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_length = mEmbeddingLength;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_sys_start(mbase::string& out_start)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_start = mSystemStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_sys_start(mbase::vector<inf_text_token>& out_tokens)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_tokens = mSystemStartTokenized;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_assistant_start(mbase::string& out_start)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_start = mAssistantStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_assistant_start(mbase::vector<inf_text_token>& out_tokens)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_tokens = mAssistantStartTokenized;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_usr_start(mbase::string& out_start)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_start = mUsrStart;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_usr_start(mbase::vector<inf_text_token>& out_tokens)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_tokens = mUserStartTokenized;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_sys_end(mbase::string& out_end)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_end = mSystemEnd;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_assistant_end(mbase::string& out_end)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_end = mAssistantEnd;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_usr_end(mbase::string& out_end)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_end = mUserEnd;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_eot_token(inf_text_token& out_token)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_token = llama_token_eot(mModel);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_lf_token(inf_text_token& out_token)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_token = llama_token_nl(mModel);
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_vocab_count(I32& out_count)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	out_count = llama_n_vocab(mModel);
	return flags::INF_MODEL_SUCCESS;;
}

InfModelTextToText::flags InfModelTextToText::get_size(size_type& out_size)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	return flags::INF_MODEL_SUCCESS;
}

bool InfModelTextToText::is_token_eof_generation(inf_text_token in_token)
{
	if (!this->is_initialized())
	{
		return false;
	}
	return llama_token_is_eog(mModel, in_token);
}

InfModelTextToText::flags InfModelTextToText::is_token_special(const mbase::string& in_string)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	mbase::vector<mbase::string> specialTokens;
	get_special_tokens(specialTokens);

	if (std::find(specialTokens.begin(), specialTokens.end(), in_string) == specialTokens.end())
	{
		return flags::INF_MODEL_ERR_GENERIC;
	}

	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::is_token_control(inf_text_token in_token)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	if (llama_token_get_attr(mModel, in_token) & LLAMA_TOKEN_ATTR_CONTROL)
	{
		return flags::INF_MODEL_SUCCESS;
	}
	return flags::INF_MODEL_ERR_GENERIC;
}

U32 InfModelTextToText::get_total_context_size() const
{
	return mTotalContextSize;
}

U32 InfModelTextToText::get_occupied_context_size() const
{
	return mOccupiedContext;
}

InfModelTextToText::flags InfModelTextToText::initialize_model(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers)
{
	if(is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	if(signal_state_initializing())
	{
		return flags::INF_MODEL_INFO_INITIALIZING_MODEL;
	}

	I32 inputLayers = in_gpu_layers;

	if(inputLayers == -1)
	{
		inputLayers = 0;
	}
	// TODO: Check if the given total context size is too small

	mSuppliedParams = llama_model_default_params();
	mTotalContextSize = in_total_context_size;
	if(ggml_cpu_has_cuda())
	{
		mSuppliedParams.n_gpu_layers = in_gpu_layers;
		mSuppliedParams.split_mode = LLAMA_SPLIT_MODE_NONE;
	}
	
	mModelPath = in_path;

	mInitializeSignal.set_signal_with_state();
	start_processor();
	return flags::INF_MODEL_INFO_INITIALIZING_MODEL;
}

InfModelTextToText::flags InfModelTextToText::initialize_model_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers)
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

	for (context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
	{
		InfTextToTextProcessor* baseProcessor = static_cast<InfTextToTextProcessor*>(*It);
		baseProcessor->stop_processor();
		baseProcessor->destroy();
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

InfModelTextToText::flags InfModelTextToText::register_context_process(InfTextToTextProcessor* in_processor, 
		const U32& in_context_length, 
		U32 in_batch_size,
		U32 in_thread_count,
		U32 in_batch_thread_count,
		const bool& in_flash_attention,
		const inf_sampling_set& in_sampler_set
)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
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

	if(!in_batch_size)
	{
		in_batch_size = in_context_length / 8;
	}

	if(in_batch_size > in_context_length)
	{
		in_batch_size = in_context_length;
	}

	if(!in_thread_count)
	{
		in_thread_count = 1;
	}

	if(!in_batch_thread_count)
	{
		in_batch_thread_count = 1;
	}

	in_processor->initialize(
		this, 
		in_context_length, 
		mbase::string::generate_uuid(),
		in_batch_size,
		in_thread_count,
		in_batch_thread_count,
		in_flash_attention,
		in_sampler_set
	);
	mProcessorListMutex.acquire();
	mRegisteredProcessors.push_back(in_processor);
	mProcessorListMutex.release();
	return flags::INF_MODEL_INFO_REGISTERING_PROCESSOR;
}

GENERIC InfModelTextToText::manual_context_deletion(InfTextToTextProcessor* in_processor)
{
	mbase::lock_guard tmpListMutex(mProcessorListMutex);
	context_processor_list::iterator It = std::find(mRegisteredProcessors.begin(), mRegisteredProcessors.end(), in_processor);
	mRegisteredProcessors.erase(It);
}

GENERIC InfModelTextToText::on_initialize_fail(init_fail_code out_fail_code)
{
}

GENERIC InfModelTextToText::_initialize_model()
{
	mbase::GgufMetaConfigurator tempConfigurator(mModelPath);
	if(!tempConfigurator.is_open())
	{
		mInitFailCode = init_fail_code::PATH_NOT_FOUND;
		mInitializeSignal.reset_signal_with_state();
		mInitFailSignal.set_signal_with_state();
		return;
	}

	// if(!tempConfigurator.get_key("mbase.model_name", mModelName) 
	// 	|| !tempConfigurator.get_key("mbase.model_architecture", mModelArchitecture)
	// 	|| !tempConfigurator.get_key("mbase.quantization_coefficient", mQuantizationCoefficient)
	// 	|| !tempConfigurator.get_key("mbase.block_count", mBlockCount)
	// 	|| !tempConfigurator.get_key("mbase.head_count", mHeadCount)
	// 	|| !tempConfigurator.get_key("mbase.embedding_length", mEmbeddingLength)
	// 	|| !tempConfigurator.get_key("mbase.model_size", mModelSize))
	// {
	// 	mInitFailCode = init_fail_code::MBASE_PARAMS_DONT_MATCH;
	// 	mInitializeSignal.reset_signal_with_state();
	// 	mInitFailSignal.set_signal_with_state();
	// 	return;
	// }

	tempConfigurator.get_key("general.architecture", mModelArchitecture);

	mbase::tokenizer_align_instruct_template(mModelArchitecture,
		mSystemStart,
		mAssistantStart,
		mUsrStart,
		mSystemEnd,
		mAssistantEnd,
		mUserEnd
	);

	tempConfigurator.get_key(mModelArchitecture + ".embedding_length", mEmbeddingLength);

	// Before diving into loading model, 
	// calculate how much memory we need to load the model 
	// if there is not enough memory for loading the model, abort.
	
	mModel = llama_load_model_from_file(mbase::to_utf8(mModelPath).c_str(), mSuppliedParams);
	if (!mModel)
	{
		mInitFailCode = init_fail_code::LLAMA_SYSTEM_ERROR;
		mInitializeSignal.reset_signal_with_state();
		mInitFailSignal.set_signal_with_state();
		return;
	}
	
	std::cout << mSystemStart << "You are a helpful assistant" << mSystemEnd;
	std::cout << mUsrStart << "How are you?" << mUserEnd;
	std::cout << mAssistantStart << "I am fine thanks!" << mAssistantEnd;

	if(llama_model_has_encoder(mModel) && llama_model_has_decoder(mModel))
	{
		mIsEmbeddingModel = false;
	}
	
	mIsInitialized = true;
	mInitializeSignal.reset_signal_with_state();
	mInitMethodSignal.set_signal();
}

GENERIC InfModelTextToText::_destroy_model()
{
	mbase::lock_guard tmpListMutex(mProcessorListMutex);
	for (context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
	{
		InfProcessorBase* baseProcessor = *It;
		InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(baseProcessor);
		t2tProcessor->destroy();
		while(t2tProcessor->signal_state_destroying())
		{

		}
	}
	llama_free_model(mModel);
	mModel = NULL;
	
	mUsrStart.clear();
	mSystemStart.clear();
	mAssistantStart.clear();
	mUserEnd.clear();
	mSystemEnd.clear();
	mAssistantEnd.clear();
	mModelPath.clear();
	mEndOfToken = 0;
	mOccupiedContext = 0;
	mModelTimer.clear_timers();

	/* RESET ALL SIGNALS */
	mInitializeSignal.reset_signal_with_state();
	mIsProcessorRunning = false;
	mDestroySignal.reset_signal_with_state();

	mDestroyMethodSignal.set_signal();
	mIsInitialized = false;
}

GENERIC InfModelTextToText::_get_special_tokens(mbase::vector<inf_text_token>& out_tokens)
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
	mbase::vector<inf_text_token> specialTokens;
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
	if(signal_state_destroying())
	{
		return;
	}

	if(signal_init_fail_method())
	{
		stop_processor();
		mInitFailSignal.reset_signal_with_state();
		on_initialize_fail(mInitFailCode);
	}

	if(signal_init_method())
	{
		stop_processor();
		mInitMethodSignal.reset_signal_with_state();
		on_initialize();
	}

	mbase::lock_guard tmpListMutex(mProcessorListMutex);
	for(context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end();)
	{
		InfProcessorBase* baseProcessor = *It;
		InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(baseProcessor);
		t2tProcessor->update();
		if(!t2tProcessor->is_registered())
		{
			if(!t2tProcessor->signal_initializing())
			{
				mOccupiedContext -= t2tProcessor->get_context_size();
				It = mRegisteredProcessors.erase(It);
				continue;
			}
		}
		++It;
	}

	if (signal_destroy_method())
	{
		stop_processor();
		mDestroyMethodSignal.reset_signal_with_state();
		on_destroy();
	}
}

GENERIC InfModelTextToText::update_t()
{
	while(is_processor_running())
	{
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
	}
}

MBASE_END