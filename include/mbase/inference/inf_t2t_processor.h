#ifndef MBASE_INF_T2T_PROCESSOR_H
#define MBASE_INF_T2T_PROCESSOR_H

#include <mbase/common.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_sampling_set.h>
#include <mbase/inference/inf_context_line.h>
#include <mbase/inference/inf_t2t_proc_diagnostics.h>

MBASE_BEGIN

struct decode_behavior_description {
	U32 mHaltDelay = 2; // in milliseconds
	U32 mTokenAtMost = 1;
	bool mHaltOnWrite = false;
};

class InfModelTextToText;

class MBASE_API InfProcessorTextToText : public mbase::InfProcessorBase {
public:
	using inf_text_token_candidates = mbase::vector<llama_token_data>;

	enum class last_fail_code {
		MODEL_NOT_INITIALIZED,
		NOT_ENOUGH_MEMORY,
		INVALID_MODEL_TYPE
	};

	enum class cache_mode {
		AUTO_LOGIT_STORE_MODE,
		KV_LOCK_MODE
	};

	InfProcessorTextToText();
	~InfProcessorTextToText();

	InfProcT2TDiagnostics& get_diagnostics();
	last_fail_code get_last_fail_code() const;
	cache_mode get_manual_cache_mode() const;
	bool is_update_required() const;
	bool is_init_failed() const;
	bool is_available() const;
	bool is_manual_caching() const;
	bool signal_state_lora_operate() const;
	bool signal_state_input_process() const;
	bool signal_state_decode_process() const;
	bool signal_state_kv_locked_process() const;
	bool signal_lora_operate_process() const;
	bool signal_input_process() const;
	bool signal_decode_process() const;
	bool signal_kv_locked_process() const;
	#ifdef MBASE_INTERNAL_API
		inf_text_token_candidates& get_token_candidates();
	#endif // MBASE_INTERNAL_API
	const U32& get_batch_size() const;
	const U32& get_max_token_length() const;
	const U32& get_context_cursor_position() const;
	I32 get_cache_token_count() const;
	I32 get_batch_thread_count() const;
	I32 get_thread_count() const;
	bool has_sampler(InfSamplerDescription::SAMPLER in_sampler_type, InfSamplerDescription& out_sampler);
	GENERIC get_available_samplers(inf_sampling_set& out_samplers);
	flags get_processor_status() const;
	flags token_to_description(const inf_text_token& in_token, inf_token_description& out_description);
	flags tokens_to_description_vector(const mbase::vector<inf_text_token>& in_tokens, mbase::vector<inf_token_description>& out_descriptions);
	flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens);
	flags tokenize_input(context_line* in_lines, size_type in_count, inf_text_token_vector& out_tokens, bool in_append_assistant_token = true);
	flags execute_input(const inf_text_token_vector& in_tokens, bool in_kv_locked = false);
	flags execute_input(inf_text_token* in_tokens, size_type in_size, bool in_kv_locked = false);
	flags execute_input_sync(const inf_text_token_vector& in_tokens, bool in_kv_locked = false);
	flags next(const decode_behavior_description& in_description);
	flags next_sync(const decode_behavior_description& in_description);
	flags clear_response();
	flags set_inference_client(InfClientBase* in_client) override;
	flags declare_lora_assign(const inf_lora_adapter& in_adapter);
	flags declare_lora_remove(const inf_lora_adapter& in_adapter);
	flags start_lora_operation();
	flags initialize(
		InfModelTextToText* in_model, 
		const U32& in_context_length, 
		const mbase::string& in_context_id,
		const U32& in_batch_size,
		const U32& in_thread_count,
		const U32& in_batch_thread_count,
		const bool& in_flash_attention,
		const inf_sampling_set& in_sampler_set
	);
	flags initialize_sync(
		InfModelTextToText* in_model, 
		const U32& in_context_length, 
		const mbase::string& in_context_id,
		const U32& in_batch_size,
		const U32& in_thread_count,
		const U32& in_batch_thread_count,
		const bool& in_flash_attention,
		const inf_sampling_set& in_sampler_set
	);
	flags destroy() override;
	flags destroy_sync() override;
	GENERIC clear_token_candidates();
	GENERIC clear_samplers();
	GENERIC clear_kv_cache();
	GENERIC set_manual_caching(bool in_manual_cache, cache_mode in_cache_mode = cache_mode::AUTO_LOGIT_STORE_MODE);
	GENERIC update() override;
	GENERIC update_t() override;

	virtual GENERIC on_lora_operate(const mbase::vector<inf_lora_adapter>& out_adapters);
	virtual GENERIC on_initializing();
	virtual GENERIC on_initialize_fail(last_fail_code out_code);
	virtual GENERIC on_destroying();
	virtual GENERIC on_initialize() = 0;
	virtual GENERIC on_destroy() = 0;

	// this is an internal call, do not call it manually
	GENERIC _internal_adapter_remove(mbase::vector<inf_lora_adapter>& in_adapters_to_remove);

private:
	GENERIC _decode_cached_logits();
	GENERIC _decode_kv_locked_input();
	GENERIC _decode_input();
	GENERIC _decode_next();
	GENERIC _lora_operate();
	GENERIC _initialize_context();
	GENERIC _destroy_context();

	InfProcT2TDiagnostics mDiagnostics;
	llama_sampler* mSamplerChain;
	llama_context* mModelContext;
	llama_batch mInputBatch;
	inf_text_token_candidates mPresetCandidates;
	inf_text_token_vector mTokenizedInput;
	inf_text_token_vector mGeneratedTokenVector;
	inf_text_token_vector mLogitTokenVector;
	mbase::vector<inf_lora_adapter> mDeclaredAdapters;
	mbase::vector<inf_lora_adapter> mRemoveAdapters;
	mbase::vector<inf_lora_adapter> mAssignedAdapters;
	inf_sampling_set mSamplerDescriptions;
	lora_adapter_map mLoraMap;
	U32 mContextCursor; // -----> if it exceeds the context size, stop generating
	U32 mBatchSize;
	U32 mThreadCount;
	U32 mBatchProcessThreadCount;
	U32 mProcessedBatchLength;
	U32 mLogitStartIndex;
	U32 mPromptStartIndex;
	processor_signal mInputSignal;
	processor_signal mDecodeSignal;
	processor_signal mInputKvLockedSignal;
	processor_signal mLoraOperationSignal;
	finish_state mFinishState;
	last_fail_code mLastFailCode;	
	bool mFlashAttention;
	bool mIsInitializeFailed;
	bool mIsManualCaching;
	decode_behavior_description mDecodeBehavior;
	cache_mode mCacheMode;
};

MBASE_END

#endif // MBASE_INF_T2T_PROCESSOR_H