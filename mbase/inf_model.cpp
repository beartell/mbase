#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/inference/inf_chat_templates.h>
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
	mUsrStart(),
	mSystemStart(),
	mAssistantStart(),
	mOccupiedContext(0),
	mBlockCount(0),
	mHeadCount(0),
	mEmbeddingLength(0),
	mQuantizationCoefficient(0.0f),
	mModelSize(0),
	mHasEmbeddedSystemPrompt(false)
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
	out_name = mModelName;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_architecture(mbase::string& out_architecture)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_architecture = mModelArchitecture;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_embedding_length(U32& out_length)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_length = mEmbeddingLength;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_rope_type(mbase::string& out_type)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	// IMPLEMENT
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
	out_end = mSystemEnd;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_assistant_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_end = mAssistantEnd;
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::get_usr_end(mbase::string& out_end)
{
	MBASE_INF_MODEL_RETURN_UNINITIALIZED;
	out_end = mUserEnd;
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

U32 InfModelTextToText::get_total_context_size()
{
	return mTotalContextSize;
}

U32 InfModelTextToText::get_occupied_context_size()
{
	return mOccupiedContext;
}

InfModelTextToText::flags InfModelTextToText::initialize_model(const mbase::wstring& in_path, const U32& in_total_context_size, I32 in_gpu_layers)
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

InfModelTextToText::flags InfModelTextToText::initialize_model_sync(const mbase::wstring& in_path, const U32& in_total_context_size, I32 in_gpu_layers)
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

	if(!tempConfigurator.get_key("mbase.model_name", mModelName) 
		|| !tempConfigurator.get_key("mbase.model_architecture", mModelArchitecture)
		|| !tempConfigurator.get_key("mbase.quantization_coefficient", mQuantizationCoefficient)
		|| !tempConfigurator.get_key("mbase.block_count", mBlockCount)
		|| !tempConfigurator.get_key("mbase.head_count", mHeadCount)
		|| !tempConfigurator.get_key("mbase.embedding_length", mEmbeddingLength)
		|| !tempConfigurator.get_key("mbase.model_size", mModelSize))
	{
		mInitFailCode = init_fail_code::MBASE_PARAMS_DONT_MATCH;
		mInitializeSignal.reset_signal_with_state();
		mInitFailSignal.set_signal_with_state();
		return;
	}

	mbase::tokenizer_align_instruct_template(mModelArchitecture,
		mSystemStart,
		mAssistantStart,
		mUsrStart,
		mSystemEnd,
		mAssistantEnd,
		mUserEnd
	);

	mModel = llama_load_model_from_file(mbase::to_utf8(mModelPath).c_str(), mSuppliedParams);
	if (!mModel)
	{
		mInitFailCode = init_fail_code::LLAMA_SYSTEM_ERROR;
		mInitializeSignal.reset_signal_with_state();
		mInitFailSignal.set_signal_with_state();
		return;
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

	if(signal_destroy_method())
	{
		stop_processor();
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