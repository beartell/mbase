#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/inference/inf_chat_templates.h>
#include <mbase/inference/inf_device_desc.h>
#include <mbase/filesystem.h>
#include <iostream>

MBASE_BEGIN

#define MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED \
if(this->signal_destroying())\
{\
	return flags::INF_MODEL_INFO_DESTROYING_MODEL;\
}\
if(this->signal_initializing())\
{\
	return flags::INF_MODEL_INFO_INITIALIZING_MODEL;\
}\
if(!this->is_initialized())\
{\
	return flags::INF_MODEL_ERR_NOT_INITIALIZED;\
}

InfModelTextToText::InfModelTextToText() :
	mModel(NULL),
	mEndOfToken(0),
	mModelSize(0),
	mOccupiedContext(0),
	mTotalContextSize(0),
	mQuantizationCoefficient(0.0f),
	mIsEmbeddingModel(false)
{
	mModelCategory = inf_model_category::TEXT_TO_TEXT;
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
			InfProcessorBase* baseProcessor = It->mSubject;
			if(baseProcessor)
			{
				baseProcessor->stop_processor();
				baseProcessor->destroy_sync();
				baseProcessor->update();
			}
		}
		
		llama_model_free(mModel);
	}
}

bool InfModelTextToText::signal_lora_operation() const
{
	return mLoraOperationSignal.get_signal();
}

bool InfModelTextToText::signal_state_lora_operation() const
{
	return mLoraOperationSignal.get_signal_state();
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

llama_model* InfModelTextToText::get_raw_model()
{
	return mModel;
}

mbase::vector<inf_text_token> InfModelTextToText::get_special_tokens() const
{
	mbase::vector<inf_text_token> out_tokens;
	const llama_vocab* tmpVocab = llama_model_get_vocab(mModel);
	for (I32 i = 0; i < llama_vocab_n_tokens(tmpVocab); i++)
	{
		llama_token_attr lta = llama_vocab_get_attr(tmpVocab, i);
		if (lta != LLAMA_TOKEN_ATTR_NORMAL)
		{
			out_tokens.push_back(i);
		}
	}

	return out_tokens;
}

mbase::vector<mbase::string> InfModelTextToText::get_special_tokens_string() const
{
	mbase::vector<mbase::string> out_tokens;
	mbase::vector<inf_text_token> specialTokens = get_special_tokens();
	const llama_vocab* tmpVocab = llama_model_get_vocab(mModel);
	for (auto& n : specialTokens)
	{
		IBYTE myChars[128] = { 0 };
		I32 tokenLength = llama_token_to_piece(tmpVocab, n, myChars, 128, 1, true);
		if(!tokenLength || tokenLength < 0)
		{
			continue;
		}
		out_tokens.push_back(myChars);
	}
	return out_tokens;
}

const mbase::string& InfModelTextToText::get_model_name() const
{
	return mModelName;
}

const mbase::string&InfModelTextToText::get_architecture() const
{
	return mModelArchitecture;
}

const mbase::string& InfModelTextToText::get_sys_start() const
{
	return mSystemStart;
}

const mbase::string& InfModelTextToText::get_assistant_start() const
{
	return mAssistantStart;
}

const mbase::string& InfModelTextToText::get_usr_start() const
{
	return mUsrStart;
}

const mbase::string& InfModelTextToText::get_sys_end() const
{
	return mSystemEnd;
}

const mbase::string& InfModelTextToText::get_assistant_end() const
{
	return mAssistantStart;
}

const mbase::string& InfModelTextToText::get_usr_end() const
{
	return mUserEnd;
}

inf_text_token InfModelTextToText::get_eot_token() const
{
	const llama_vocab* tmpVocab = llama_model_get_vocab(mModel);
	return llama_vocab_eot(tmpVocab);
}

inf_text_token InfModelTextToText::get_lf_token() const
{
	const llama_vocab* tmpVocab = llama_model_get_vocab(mModel);
	return llama_vocab_nl(tmpVocab);
}

I32 InfModelTextToText::get_vocab_count() const
{
	const llama_vocab* tmpVocab = llama_model_get_vocab(mModel);
	return llama_vocab_n_tokens(tmpVocab);
}

typename InfModelTextToText::size_type InfModelTextToText::get_size() const
{
	return llama_model_size(mModel);
}

U32 InfModelTextToText::get_embedding_length() const
{
	return llama_model_n_embd(mModel);
}

U32 InfModelTextToText::get_head_count() const
{
	return llama_model_n_head(mModel);
}

U32 InfModelTextToText::get_layer_count() const
{
	return llama_model_n_layer(mModel);
}

U32 InfModelTextToText::get_max_embedding_context() const
{
	return llama_model_n_ctx_train(mModel);
}

bool InfModelTextToText::is_token_eof_generation(inf_text_token in_token) const
{
	if (!this->is_initialized())
	{
		return false;
	}
	const llama_vocab* tmpVocab = llama_model_get_vocab(mModel);
	return llama_vocab_is_eog(tmpVocab, in_token);
}

InfModelTextToText::flags InfModelTextToText::is_token_special(const mbase::string& in_string) const
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	mbase::vector<mbase::string> specialTokens = get_special_tokens_string();

	if (std::find(specialTokens.begin(), specialTokens.end(), in_string) == specialTokens.end())
	{
		return flags::INF_MODEL_ERR_GENERIC;
	}

	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::is_token_control(inf_text_token in_token) const
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;
	const llama_vocab* tmpVocab = llama_model_get_vocab(mModel);
	if (llama_vocab_get_attr(tmpVocab, in_token) & LLAMA_TOKEN_ATTR_CONTROL)
	{
		return flags::INF_MODEL_SUCCESS;
	}
	return flags::INF_MODEL_ERR_GENERIC;
}

const mbase::string& InfModelTextToText::get_quantization_string() const
{
	return mQuantizationString;
}

const U32& InfModelTextToText::get_total_context_size() const
{
	return mTotalContextSize;
}

const U32& InfModelTextToText::get_occupied_context_size() const
{
	return mOccupiedContext;
}

InfModelTextToText::flags InfModelTextToText::initialize_model_ex(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices)
{
	if(is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	} 
	
	if(signal_initializing())
	{
		return flags::INF_MODEL_INFO_INITIALIZING_MODEL;
	}

	if(signal_state_initializing())
	{
		return flags::INF_MODEL_INFO_UPDATE_REQUIRED;
	}

	I32 inputLayers = in_gpu_layers;

	if(inputLayers == -1)
	{
		inputLayers = 0;
	}

	if(!mbase::is_file_valid(in_path))
	{
		return flags::INF_MODEL_ERR_MISSING_MODEL;
	}

	// TODO: Check if the given total context size is too small

	mSuppliedParams = llama_model_default_params();
	mSuppliedParams.use_mmap = in_use_mmap;
	mSuppliedParams.use_mlock = in_use_mlock;
	
	I32 gpuCount = 0;
	mPhysicalDevices.clear();
	if(in_devices.size())
	{
		for(auto& tmpDeviceDesc : in_devices)
		{
			if(tmpDeviceDesc.get_device_type() == mbase::InfDeviceDescription::device_type::GPU)
			{
				gpuCount++;
			}
			mPhysicalDevices.push_back(tmpDeviceDesc.get_internal_dev_handle());
		}

		if(mPhysicalDevices.size())
		{
			// llama.cpp expects null terminated device list
			mPhysicalDevices.push_back(nullptr);
			mSuppliedParams.devices = mPhysicalDevices.data();
		}

		if(gpuCount)
		{
			mSuppliedParams.n_gpu_layers = in_gpu_layers;
			mSuppliedParams.split_mode = LLAMA_SPLIT_MODE_ROW;
		}
	}

	mTotalContextSize = in_total_context_size;
	mModelPath = in_path;
	mRegisteredProcessors.clear(); // Since the registered processors is not on the destruction list, make sure it is fresh

	mInitializeSignal.set_signal();
	start_processor();
	return flags::INF_MODEL_INFO_INITIALIZING_MODEL;
}

InfModelTextToText::flags InfModelTextToText::initialize_model(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers)
{
	return initialize_model_ex(in_path, in_total_context_size, in_gpu_layers, false, true);
}

InfModelTextToText::flags InfModelTextToText::initialize_model_ex_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices)
{
	initialize_model_ex(in_path, in_total_context_size, in_gpu_layers, in_use_mmap, in_use_mlock, in_devices);

	while(signal_initializing())
	{
		mbase::sleep(2);
	}

	if(!is_initialized())
	{
		return flags::INF_MODEL_ERR_CANT_LOAD_MODEL;
	}

	return flags::INF_MODEL_INFO_UPDATE_REQUIRED;
}

InfModelTextToText::flags InfModelTextToText::initialize_model_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers)
{
	return initialize_model_ex_sync(in_path, in_total_context_size, in_gpu_layers, false, true);
}

InfModelTextToText::flags InfModelTextToText::destroy()
{
	if(!is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	if(signal_destroying())
	{
		return flags::INF_MODEL_INFO_DESTROYING_MODEL;
	}

	if(signal_state_destroying())
	{
		return flags::INF_MODEL_INFO_UPDATE_REQUIRED;
	}

	for (context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end();)
	{
		if(!It->mSubject)
		{
			It = mRegisteredProcessors.erase(It->mItSelf);
			continue;
		}
		InfProcessorTextToText* baseProcessor = static_cast<InfProcessorTextToText*>(It->mSubject);
		baseProcessor->stop_processor();
		baseProcessor->destroy();
		++It;
	}

	mDestroySignal.set_signal();
	start_processor();
	return flags::INF_MODEL_INFO_DESTROYING_MODEL;
}

InfModelTextToText::flags InfModelTextToText::destroy_sync()
{
	if(!is_initialized())
	{
		return flags::INF_MODEL_SUCCESS;
	}

	destroy();
	while(signal_destroying())
	{
		mbase::sleep(2);
	}

	return flags::INF_MODEL_INFO_UPDATE_REQUIRED;
}

InfModelTextToText::flags InfModelTextToText::register_context_process
(
	InfProcessorTextToText* in_processor, 
	const U32& in_context_length, 
	U32 in_batch_size,
	U32 in_thread_count,
	U32 in_batch_thread_count,
	const bool& in_flash_attention,
	const inf_sampling_set& in_sampler_set
)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	if(is_embedding_model())
	{
		return flags::INF_MODEL_ERR_PROC_UNMATCH;
	}

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

	in_processor->initialize(
		this, 
		in_context_length, 
		mbase::string::generate_uuid(),
		in_batch_size,
		in_thread_count,
		in_batch_thread_count,
		in_flash_attention,
		in_sampler_set
	); // 100% success

	mProcessorListMutex.acquire();
	mRegisteredProcessors.push_back(watcher_type());
	watcher_type& newWatcher = mRegisteredProcessors.back();
	newWatcher.mItSelf = mRegisteredProcessors.end_node();
	newWatcher.mSubject = in_processor;
	in_processor->acquire_object_watcher(&newWatcher);
	mProcessorListMutex.release();
	return flags::INF_MODEL_INFO_REGISTERING_PROCESSOR;
}

InfModelTextToText::flags InfModelTextToText::register_context_process
(
	InfEmbedderProcessor* in_processor,
	const U32& in_context_length,
	U32 in_thread_count
)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	if(!is_embedding_model())
	{
		return flags::INF_MODEL_ERR_PROC_UNMATCH;
	}

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

	if(!in_thread_count)
	{
		in_thread_count = 1;
	}

	in_processor->initialize(
		this,
		mbase::string::generate_uuid(),
		in_context_length,
		in_thread_count
	); // 100% success
	
	mProcessorListMutex.acquire();
	mRegisteredProcessors.push_back(watcher_type());
	watcher_type& newWatcher = mRegisteredProcessors.back();
	newWatcher.mItSelf = mRegisteredProcessors.end_node();
	newWatcher.mSubject = in_processor;
	in_processor->acquire_object_watcher(&newWatcher);
	mProcessorListMutex.release();
	return flags::INF_MODEL_INFO_REGISTERING_PROCESSOR;
}

InfModelTextToText::flags InfModelTextToText::unregister_context_process(
		InfProcessorBase* in_processor
)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	if (!in_processor->is_registered())
	{
		return flags::INF_MODEL_ERR_PROCESSOR_NOT_FOUND;
	}

	mProcessorListMutex.acquire();
	for (context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
	{
		InfProcessorBase* baseProcessor = It->mSubject;
		if(baseProcessor == in_processor)
		{
			baseProcessor->destroy();
			return flags::INF_MODEL_INFO_PROCESSOR_IS_BEING_DESTROYED;
		}
	}
	mProcessorListMutex.release();

	return flags::INF_MODEL_ERR_PROCESSOR_NOT_FOUND;
}

InfModelTextToText::flags InfModelTextToText::declare_lora_remove(const inf_lora_adapter& in_adapter)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	if(mbase::find(mLoraAdapters.begin(), mLoraAdapters.end(), in_adapter) == mLoraAdapters.end())
	{
		return flags::INF_MODEL_ERR_LORA_MISSING;
	}

	if(!in_adapter.mAdapterName.size())
	{
		return flags::INF_MODEL_ERR_LORA_NAME_MISSING;
	}

	if(signal_lora_operation())
	{
		return flags::INF_MODEL_ERR_LORA_OPERATION_ACTIVE;	
	}

	if(signal_state_lora_operation())
	{
		return flags::INF_MODEL_INFO_UPDATE_REQUIRED;
	}

	if(mbase::find(mLoraRemoves.begin(), mLoraRemoves.end(), in_adapter) != mLoraRemoves.end())
	{
		// Removed adapter already exists in remove list, do not bother pushing
		return flags::INF_MODEL_SUCCESS;
	}

	mLoraRemoves.push_back(in_adapter);

	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::declare_lora_adapter(const inf_lora_adapter& in_adapter)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	if(mbase::find(mLoraAdapters.begin(), mLoraAdapters.end(), in_adapter) != mLoraAdapters.end())
	{
		return flags::INF_MODEL_ERR_LORA_EXISTS;
	}

	if(signal_lora_operation())
	{
		return flags::INF_MODEL_ERR_LORA_OPERATION_ACTIVE;	
	}

	if(signal_state_lora_operation())
	{
		return flags::INF_MODEL_INFO_UPDATE_REQUIRED;
	}

	if(mbase::find(mLoraDeclares.begin(), mLoraDeclares.end(), in_adapter) != mLoraDeclares.end())
	{
		// LoRA already declared, don't bother pushing
		return flags::INF_MODEL_SUCCESS;
	}

	mLoraDeclares.push_back(in_adapter);

	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::start_lora_operation()
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	if(!mLoraDeclares.size() && !mLoraRemoves.size())
	{
		// If declared and removes are empty, nothing to operate on P2
		return flags::INF_MODEL_ERR_LORA_NOTHING_TO_OPERATE;
	}

	if(signal_lora_operation())
	{
		return flags::INF_MODEL_SUCCESS;	
	}

	mLoraOperationSignal.set_signal();
	return flags::INF_MODEL_SUCCESS;
}

InfModelTextToText::flags InfModelTextToText::tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens)
{
	MBASE_INF_T2T_MODEL_RETURN_UNINITIALIZED;

	if(!in_data || !in_size)
	{
		return flags::INF_MODEL_ERR_INVALID_INPUT;
	}

	inf_text_token_vector tokenizedInput(in_size * 4);
	llama_model* rawModel = get_raw_model();
	try
	{
		const llama_vocab* tmpVocab = llama_model_get_vocab(mModel);
		I32 tokenCount = llama_tokenize(tmpVocab, in_data, static_cast<I32>(in_size), tokenizedInput.data(), static_cast<I32>(tokenizedInput.capacity()), false, true);
		if(tokenCount == -1)
		{
			return flags::INF_MODEL_ERR_TOKENIZATION_FAILED;
		}

		tokenizedInput.resize_on_preset(tokenCount);
	}
	catch([[maybe_unused]] const std::exception& e)
	{
		return flags::INF_MODEL_ERR_TOKENIZATION_FAILED;
	}
	
	out_tokens = std::move(tokenizedInput);
	return flags::INF_MODEL_SUCCESS;
}

GENERIC InfModelTextToText::on_lora_operate(const mbase::vector<inf_lora_adapter>& out_active_loras)
{
}

GENERIC InfModelTextToText::_initialize_model()
{
	mbase::GgufMetaConfigurator tempConfigurator(mModelPath);
	if(!tempConfigurator.is_open())
	{
		mInitFailCode = init_fail_code::PATH_NOT_FOUND;
		mInitializeSignal.set_signal_finished();
		mIsInitFailed = true;
		return;
	}

	U32 tmpModelQuantizationNumber = 0;

	tempConfigurator.get_key("general.architecture", mModelArchitecture);
	tempConfigurator.get_key("general.name", mModelName);
	tempConfigurator.get_key("general.file_type", tmpModelQuantizationNumber);

	llama_ftype fileType = (llama_ftype)tmpModelQuantizationNumber;

	switch (fileType)
	{
	case LLAMA_FTYPE_ALL_F32:
		mQuantizationString = "F32";
		break;
	case LLAMA_FTYPE_MOSTLY_F16:
		mQuantizationString = "F16";
		break;
	case LLAMA_FTYPE_MOSTLY_Q4_0:
		mQuantizationString = "Q4_0";
		break;
	case LLAMA_FTYPE_MOSTLY_Q4_1:
		mQuantizationString = "Q4_1";
		break;
	case LLAMA_FTYPE_MOSTLY_Q8_0:
		mQuantizationString = "Q8_0";
		break;
	case LLAMA_FTYPE_MOSTLY_Q5_0:
		mQuantizationString = "Q5_0";
		break;
	case LLAMA_FTYPE_MOSTLY_Q5_1:
		mQuantizationString = "Q5_1";
		break;
	case LLAMA_FTYPE_MOSTLY_Q2_K:
		mQuantizationString = "Q2_K";
		break;
	case LLAMA_FTYPE_MOSTLY_Q3_K_S:
		mQuantizationString = "Q3_K_S";
		break;
	case LLAMA_FTYPE_MOSTLY_Q3_K_M:
		mQuantizationString = "Q3_K_M";
		break;
	case LLAMA_FTYPE_MOSTLY_Q3_K_L:
		mQuantizationString = "Q3_K_L";
		break;
	case LLAMA_FTYPE_MOSTLY_Q4_K_S:
		mQuantizationString = "Q4_K_S";
		break;
	case LLAMA_FTYPE_MOSTLY_Q4_K_M:
		mQuantizationString = "Q4_K_M";
		break;
	case LLAMA_FTYPE_MOSTLY_Q5_K_S:
		mQuantizationString = "Q5_K_S";
		break;
	case LLAMA_FTYPE_MOSTLY_Q5_K_M:
		mQuantizationString = "Q5_K_M";
		break;
	case LLAMA_FTYPE_MOSTLY_Q6_K:
		mQuantizationString = "Q6_K";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ2_XXS:
		mQuantizationString = "IQ2_XXS";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ2_XS:
		mQuantizationString = "IQ2_XS";
		break;
	case LLAMA_FTYPE_MOSTLY_Q2_K_S:
		mQuantizationString = "Q2_K_S";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ3_XS:
		mQuantizationString = "IQ3_XS";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ3_XXS:
		mQuantizationString = "IQ3_XXS";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ1_S:
		mQuantizationString = "IQ1_S";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ4_NL:
		mQuantizationString = "IQ4_NL";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ3_S:
		mQuantizationString = "IQ3_S";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ3_M:
		mQuantizationString = "IQ3_M";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ2_S:
		mQuantizationString = "IQ2_S";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ2_M:
		mQuantizationString = "IQ2_M";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ4_XS:
		mQuantizationString = "IQ4_XS";
		break;
	case LLAMA_FTYPE_MOSTLY_IQ1_M:
		mQuantizationString = "IQ1_M";
		break;
	case LLAMA_FTYPE_MOSTLY_BF16:
		mQuantizationString = "BF16";
		break;
	case LLAMA_FTYPE_MOSTLY_TQ1_0:
		mQuantizationString = "TQ1";
		break;
	case LLAMA_FTYPE_MOSTLY_TQ2_0:
		mQuantizationString = "TQ2_0";
		break;
	case LLAMA_FTYPE_GUESSED:
		mQuantizationString = "GUESSED";
		break;
	default:
		mQuantizationString = "UNKNOWN";
		break;
	}

	mbase::tokenizer_align_instruct_template(mModelArchitecture,
		mSystemStart,
		mAssistantStart,
		mUsrStart,
		mSystemEnd,
		mAssistantEnd,
		mUserEnd
	);
	
	// Before diving into loading model, 
	// calculate how much memory we need to load the model 
	// if there is not enough memory for loading the model, abort.
	mSuppliedParams.use_mmap = false;
	mSuppliedParams.use_mlock = true;
	
	mModel = llama_model_load_from_file(mbase::to_utf8(mModelPath).c_str(), mSuppliedParams);
	if (!mModel)
	{
		mInitFailCode = init_fail_code::LLAMA_SYSTEM_ERROR;
		mInitializeSignal.set_signal_finished();
		mIsInitFailed = true;
		return;
	}
	
	if(llama_model_has_encoder(mModel) && llama_model_has_decoder(mModel))
	{
		mIsEmbeddingModel = false;
	}

	// This context is for finding out the pooling type of the model.
	// If the pooling type is not NONE, mark the model as embedding model

	// Another use case of this dummy context is to find the minimum memory usage of a context for a particular size. (NOT IMPLEMENTED YET)

	llama_context* dummyContext = NULL;
	llama_context_params lcp = llama_context_default_params();
	
	lcp.n_ctx = 32;
	lcp.n_batch = 32;
	lcp.n_ubatch = 32;

	dummyContext = llama_init_from_model(mModel, lcp);

	enum llama_pooling_type lpt = llama_pooling_type(dummyContext);

	// Looking at the pooling type to check of the model is embedding model or not may be problematic...
	if(lpt == llama_pooling_type::LLAMA_POOLING_TYPE_NONE)
	{
		mIsEmbeddingModel = false;
	}
	else
	{
		mIsEmbeddingModel = true;
	}

	llama_free(dummyContext);

	mIsInitialized = true;
	mInitializeSignal.set_signal_finished();
}

GENERIC InfModelTextToText::_destroy_model()
{
	mbase::lock_guard tmpListMutex(mProcessorListMutex);
	for (context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
	{
		InfProcessorBase* baseProcessor = It->mSubject;
		if(baseProcessor)
		{
			baseProcessor->destroy_sync();
		}
	}

	llama_model_free(mModel);
	mModel = NULL;

	mModelName.clear();
	mModelArchitecture.clear();
	mUsrStart.clear();
	mSystemStart.clear();
	mAssistantStart.clear();
	mUserEnd.clear();
	mSystemEnd.clear();
	mAssistantEnd.clear();
	mModelPath.clear();
	mEndOfToken = 0;
	mOccupiedContext = 0;

	/* RESETTING ALL SIGNALS ON LOGIC LOOP */

	mDestroySignal.set_signal_finished();
}

GENERIC InfModelTextToText::_lora_operate()
{
	// LoRA operation order is as follows:
	// - Stop all registered context processors
	// - Initialize declared loras
	// - Remove, loras from context processors
	// - Remove, remove tagged loras
	
	mbase::lock_guard tmpListMutex(mProcessorListMutex);
	for(context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
	{
		InfModelBase::watcher_type& wt = *It;
		if(wt.mSubject)
		{
			InfProcessorBase* baseProcessor = wt.mSubject;
			baseProcessor->stop_processor();
		}
	}
	
	for(mbase::vector<inf_lora_adapter>::iterator It = mLoraDeclares.begin(); It != mLoraDeclares.end(); ++It)
	{
		llama_adapter_lora* loraOut = llama_adapter_lora_init(mModel, mbase::to_utf8(It->mLoraPath).c_str());
		if(loraOut)
		{
			// Means adapter init is successful
			inf_lora_adapter loraAdapter = *It;
			loraAdapter.mAdapterHandle = loraOut;
			mLoraAdapters.push_back(loraAdapter);
		}
	}

	mLoraDeclares.clear(); // Clear declared loras

	// removing loras from context processors
	for(context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
	{
		InfModelBase::watcher_type& wt = *It;
		if(wt.mSubject)
		{
			InfProcessorTextToText* baseProcessor = static_cast<InfProcessorTextToText*>(wt.mSubject);
			baseProcessor->_internal_adapter_remove(mLoraRemoves);
		}
	}

	mbase::vector<inf_lora_adapter> newAssignedAdapters;

	for(mbase::vector<inf_lora_adapter>::iterator It = mLoraAdapters.begin(); It != mLoraAdapters.end(); ++It)
	{
		if(mbase::find(mLoraRemoves.begin(), mLoraRemoves.end(), *It) != mLoraRemoves.end())
		{
			llama_adapter_lora_free(It->mAdapterHandle);
		}

		else
		{
			newAssignedAdapters.push_back(*It);
		}
	}

	mLoraRemoves.clear();
	mLoraAdapters = newAssignedAdapters;

	mLoraOperationSignal.set_signal_finished();
}

GENERIC InfModelTextToText::update()
{
	// load and unload control
	if(signal_initializing())
	{
		on_initializing();
		return;
	}

	if(signal_destroying())
	{
		on_destroying();
		return;
	}

	if(signal_state_destroying())
	{
		// Means destruction finished
		// Reset all signals
		reset_base_signals();
		mLoraOperationSignal.reset_signal_with_state();
		mbase::lock_guard tmpListMutex(mProcessorListMutex);
		for(context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end(); ++It)
		{
			InfModelBase::watcher_type& wt = *It;
			if(wt.mSubject)
			{
				InfProcessorBase* baseProcessor = wt.mSubject;
				baseProcessor->update();
			}
		}

		mRegisteredProcessors.clear();
		mIsInitialized = false;
		on_destroy();
		return;
	}

	if(signal_state_initializing())
	{
		// Means init finished
		reset_base_signals();

		if(is_initialize_failed())
		{
			on_initialize_fail(mInitFailCode);
		}
		else
		{
			mIsInitialized = true;
			on_initialize();
		}
		return;
	}

	if(signal_state_lora_operation())
	{
		mLoraOperationSignal.reset_signal_state();
		on_lora_operate(mLoraAdapters);
		return;
	}

	mbase::lock_guard tmpListMutex(mProcessorListMutex);
	for(context_processor_list::iterator It = mRegisteredProcessors.begin(); It != mRegisteredProcessors.end();)
	{
		InfModelBase::watcher_type& wt = *It;
		if(!wt.mSubject)
		{
			It = mRegisteredProcessors.erase(wt.mItSelf);
			mOccupiedContext -= wt.mContextLength;
			continue;
		}
		InfProcessorBase* baseProcessor = wt.mSubject;
		baseProcessor->update();
		++It;
	}
}

GENERIC InfModelTextToText::update_t()
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

		if(signal_lora_operation())
		{
			_lora_operate();
		}
	}
}

MBASE_END