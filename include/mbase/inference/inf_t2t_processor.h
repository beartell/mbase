#ifndef MBASE_INF_T2T_PROCESSOR_H
#define MBASE_INF_T2T_PROCESSOR_H

#include <mbase/common.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_sampling_set.h>
#include <mbase/inference/inf_context_line.h>

MBASE_BEGIN

struct decode_behavior_description {
	U32 mTokenAtMost = 1;
	bool mHaltOnWrite = false;
};

class InfModelTextToText;

class MBASE_API InfTextToTextProcessor : public mbase::InfProcessorBase {
public:
	using inf_text_token_candidates = mbase::vector<llama_token_data>;

	enum class last_fail_code {
		MODEL_NOT_INITIALIZED,
		NOT_ENOUGH_MEMORY,
		INVALID_MODEL_TYPE
	};

	InfTextToTextProcessor();
	~InfTextToTextProcessor();

	last_fail_code get_last_fail_code() const;
	bool is_init_failed() const;
	bool is_available() const;
	bool signal_state_input_process() const;
	bool signal_state_decode_process() const;
	bool signal_input_process() const;
	bool signal_decode_process() const;
	#ifdef MBASE_INTERNAL_API
		inf_text_token_candidates& get_token_candidates();
	#endif // MBASE_INTERNAL_API
	U32 get_max_token_length();
	bool has_sampler(InfSamplerDescription::SAMPLER in_sampler_type, InfSamplerDescription& out_sampler);
	GENERIC get_available_samplers(inf_sampling_set& out_samplers);
	flags get_processor_status() const;
	flags token_to_description(const inf_text_token& in_token, inf_token_description& out_description);
	flags tokens_to_description_vector(const mbase::vector<inf_text_token>& in_tokens, mbase::vector<inf_token_description>& out_descriptions);
	flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens);
	flags tokenize_input(context_line* in_lines, size_type in_count, inf_text_token_vector& out_tokens, bool in_append_assistant_token = true);
	flags execute_input(const inf_text_token_vector& in_tokens, bool in_abandon = false);
	flags next(const decode_behavior_description& in_description);
	flags next_sync(const decode_behavior_description& in_description);
	flags set_inference_client(InfClientBase* in_client) override;
	flags initialize(
		InfModelTextToText* in_model, 
		const U32& in_context_length, 
		const mbase::string& in_context_id,
		const U32& in_batch_size,
		const U32& in_thread_count,
		const bool& in_flash_attention,
		const inf_sampling_set& in_sampler_set
	);
	flags initialize_sync(
		InfModelTextToText* in_model, 
		const U32& in_context_length, 
		const mbase::string& in_context_id,
		const U32& in_batch_size,
		const U32& in_thread_count,
		const bool& in_flash_attention,
		const inf_sampling_set& in_sampler_set
	);
	flags destroy() override;
	flags destroy_sync() override;
	GENERIC clear_token_candidates();
	GENERIC clear_samplers();
	GENERIC update() override;
	GENERIC update_t() override;

	virtual GENERIC on_initializing();
	virtual GENERIC on_initialize_fail(last_fail_code out_code);
	virtual GENERIC on_destroying();
	virtual GENERIC on_initialize() = 0;
	virtual GENERIC on_destroy() = 0;

private:	
	GENERIC _decode_input();
	GENERIC _decode_next();
	GENERIC _initialize_context();
	GENERIC _destroy_context();

	llama_sampler* mSamplerChain;
	llama_context* mModelContext;
	llama_batch mInputBatch;
	inf_text_token_candidates mPresetCandidates;
	inf_text_token_vector mTokenizedInput;
	inf_text_token_vector mGeneratedTokenVector;
	inf_sampling_set mSamplerDescriptions;
	U32 mContextCursor; // -----> if it exceeds the context size, stop generating
	U32 mBatchSize;
	U32 mThreadCount;
	processor_signal mInputSignal;
	processor_signal mDecodeSignal;
	finish_state mFinishState;
	last_fail_code mLastFailCode;	
	bool mFlashAttention;
	bool mIsInitializeFailed;
	decode_behavior_description mDecodeBehavior;
};

MBASE_END

#endif // MBASE_INF_T2T_PROCESSOR_H