#ifndef MBASE_INF_T2T_MODEL_H
#define MBASE_INF_T2T_MODEL_H

#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_sampling_set.h>
#include <mbase/inference/inf_device_desc.h>

MBASE_BEGIN

class InfProcessorTextToText;
class InfEmbedderProcessor;

class MBASE_API InfModelTextToText : public InfModelBase {
public:
	enum class flags : U8 {
		INF_MODEL_SUCCESS,
		INF_MODEL_ERR_CANT_LOAD_MODEL,
		INF_MODEL_ERR_MISSING_MODEL,
		INF_MODEL_ERR_NO_SENTENCE,
		INF_MODEL_ERR_UPDATE_LOOP_OCCUPIED,
		INF_MODEL_INFO_REGISTERING_PROCESSOR,
		INF_MODEL_INFO_INITIALIZING_MODEL,
		INF_MODEL_INFO_DESTROYING_MODEL,
		INF_MODEL_INFO_PROCESSOR_IS_BEING_DESTROYED,
		INF_MODEL_INFO_UPDATE_REQUIRED,
		INF_MODEL_ERR_PROC_UNMATCH, // Called if the registered processor match with the model
		INF_MODEL_ERR_PROCESSOR_ALREADY_REGISTERED,
		INF_MODEL_ERR_INVALID_INPUT,
		INF_MODEL_ERR_MODEL_CONTEXT_FULL,
		INF_MODEL_ERR_INVALID_CONTEXT_LENGTH,
		INF_MODEL_ERR_PROCESSOR_NOT_FOUND,
		INF_MODEL_ERR_PROCESSOR_BELONGS_TO_ANOTHER_MODEL,
		INF_MODEL_ERR_UNABLE_REGISTER_PROCESSOR,
		INF_MODEL_ERR_NOT_INITIALIZED,
		INF_MODEL_ERR_TOKENIZATION_FAILED,
		INF_MODEL_ERR_LORA_MISSING,
		INF_MODEL_ERR_LORA_NAME_MISSING,
		INF_MODEL_ERR_LORA_EXISTS,
		INF_MODEL_ERR_LORA_FILE_INVALID,
		INF_MODEL_ERR_LORA_OPERATION_ACTIVE,
		INF_MODEL_ERR_LORA_NOTHING_TO_OPERATE,
		INF_MODEL_ERR_GENERIC
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	InfModelTextToText();
	~InfModelTextToText();
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_lora_operation() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_lora_operation() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_available(const U32& in_context_size) const;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_embedding_model() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool has_lora_adapter(const mbase::string& in_name, inf_lora_adapter& out_adapter);
	llama_model* get_raw_model();
	mbase::vector<inf_text_token> get_special_tokens() const;
	mbase::vector<mbase::string> get_special_tokens_string() const;
	const mbase::string& get_model_name() const;
	const mbase::string& get_architecture() const;
	const mbase::string& get_sys_start() const;
	const mbase::string& get_assistant_start() const;
	const mbase::string& get_usr_start() const;
	const mbase::string& get_sys_end() const;
	const mbase::string& get_assistant_end() const;
	const mbase::string& get_usr_end() const;
	inf_text_token get_eot_token() const;
	inf_text_token get_lf_token() const;
	I32 get_vocab_count() const;
	size_type get_size() const;
	U32 get_embedding_length() const;
	U32 get_head_count() const;
	U32 get_layer_count() const;
	U32 get_max_embedding_context() const;
	bool is_token_eof_generation(inf_text_token in_token) const;
	flags is_token_special(const mbase::string& in_string) const;
	flags is_token_control(inf_text_token in_token) const;
	const mbase::string& get_quantization_string() const;
	const U32& get_total_context_size() const;
	const U32& get_occupied_context_size() const;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
	static bool get_mbase_chat_template_id(const mbase::string& in_architecture, mbase::string& out_id);
	/* ===== NON-MEMBER FUNCTIONS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	flags initialize_model_ex(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices = mbase::vector<InfDeviceDescription>());
	flags initialize_model(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1);
	flags initialize_model_ex_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices = mbase::vector<InfDeviceDescription>());
	flags initialize_model_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1);
	flags destroy();
	flags destroy_sync();
	flags register_context_process(
		InfProcessorTextToText* in_processor, 
		const U32& in_context_length,
		U32 in_batch_size,
		U32 in_thread_count,
		U32 in_batch_thread_count,
		const bool& in_flash_attention,
		const inf_sampling_set& in_sampler_set
	);
	flags register_context_process(
		InfEmbedderProcessor* in_processor,
		const U32& in_context_length,
		U32 in_thread_count
	);
	flags declare_lora_remove(const inf_lora_adapter& in_adapter);
	flags declare_lora_adapter(const inf_lora_adapter& in_adapter);
	flags start_lora_operation();

	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== NON-MODIFIER METHODS BEGIN ===== */
	flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens);
	/* ===== NON-MODIFIER METHODS END ===== */

	/* ===== INTERFACE METHODS BEGIN ===== */
	virtual GENERIC on_lora_operate(const mbase::vector<inf_lora_adapter>& out_active_loras);
	/* ===== INTERFACE METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	GENERIC update() override;
	GENERIC update_t() override;
	/* ===== STATE-MODIFIER METHODS END ===== */

private:
	GENERIC _initialize_model();
	GENERIC _destroy_model();
	GENERIC _lora_operate();

	llama_model* mModel;
	mbase::string mQuantizationString;
	mbase::string mModelName;
	mbase::string mModelArchitecture;
	mbase::string mEmbeddedSystemPrompt;
	mbase::string mUsrStart;
	mbase::string mSystemStart;
	mbase::string mAssistantStart;
	mbase::string mSystemEnd;
	mbase::string mAssistantEnd;
	mbase::string mUserEnd;
	mbase::wstring mModelPath;
	llama_model_params mSuppliedParams;
	inf_text_token mEndOfToken;
	mbase::vector<ggml_backend_dev_t> mPhysicalDevices;
	mbase::vector<inf_lora_adapter> mLoraDeclares;
	mbase::vector<inf_lora_adapter> mLoraRemoves;
	mbase::vector<inf_lora_adapter> mLoraAdapters;
	U64 mModelSize;
	U32 mOccupiedContext;
	U32 mTotalContextSize;
	F32 mQuantizationCoefficient;
	bool mIsEmbeddingModel; // Not supported if (llama_model_has_encoder(model) && llama_model_has_decoder(model) is true)
	processor_signal mLoraOperationSignal;
};

MBASE_END

#endif // MBASE_INF_T2T_MODEL_H