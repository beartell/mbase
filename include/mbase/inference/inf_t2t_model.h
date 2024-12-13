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
		INF_MODEL_INFO_REGISTERING_PROCESSOR,
		INF_MODEL_INFO_INITIALIZING_MODEL,
		INF_MODEL_INFO_DESTROYING_MODEL,
		INF_MODEL_INFO_PROCESSOR_IS_BEING_DESTROYED,
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
		INF_MODEL_ERR_GENERIC
	};

	InfModelTextToText();
	~InfModelTextToText();

	bool is_available(const U32& in_context_size) const;
	bool is_embedding_model() const;
	llama_model* get_raw_model();
	flags get_special_tokens(mbase::vector<inf_text_token>& out_tokens);
	flags get_special_tokens(mbase::vector<mbase::string>& out_tokens);
	flags get_model_name(mbase::string& out_name);
	flags get_architecture(mbase::string& out_architecture);
	flags get_sys_start(mbase::string& out_start);
	flags get_sys_start(mbase::vector<inf_text_token>& out_tokens);
	flags get_assistant_start(mbase::string& out_start);
	flags get_assistant_start(mbase::vector<inf_text_token>& out_tokens);
	flags get_usr_start(mbase::string& out_start);
	flags get_usr_start(mbase::vector<inf_text_token>& out_tokens);
	flags get_sys_end(mbase::string& out_end);
	flags get_assistant_end(mbase::string& out_end);
	flags get_usr_end(mbase::string& out_end);
	flags get_eot_token(inf_text_token& out_token);
	flags get_lf_token(inf_text_token& out_token);
	flags get_vocab_count(I32& out_count);
	flags get_size(size_type& out_size);
	flags get_embedding_length(U32& out_embed_length);
	flags get_head_count(U32& out_head_count);
	flags get_layer_count(U32& out_layer_count);
	bool is_token_eof_generation(inf_text_token in_token);
	flags is_token_special(const mbase::string& in_string);
	flags is_token_control(inf_text_token in_token);
	U32 get_total_context_size() const;
	U32 get_occupied_context_size() const;

	static bool get_mbase_chat_template_id(const mbase::string& in_architecture, mbase::string& out_id);

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
		U32 in_batch_size,
		U32 in_thread_count
	);
	flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens);

	GENERIC update() override;
	GENERIC update_t() override;

private:
	GENERIC _initialize_model();
	GENERIC _destroy_model();
	GENERIC _get_special_tokens(mbase::vector<inf_text_token>& out_tokens);
	GENERIC _get_special_tokens(mbase::vector<mbase::string>& out_tokens);

	llama_model* mModel;
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
	mbase::vector<inf_text_token> mSystemStartTokenized;
	mbase::vector<inf_text_token> mAssistantStartTokenized;
	mbase::vector<inf_text_token> mUserStartTokenized;
	mbase::vector<ggml_backend_dev_t> mPhysicalDevices;
	U64 mModelSize;
	U32 mOccupiedContext;
	U32 mTotalContextSize;
	F32 mQuantizationCoefficient;
	bool mIsEmbeddingModel; // Not supported if (llama_model_has_encoder(model) && llama_model_has_decoder(model) is true)
};

MBASE_END

#endif // MBASE_INF_T2T_MODEL_H