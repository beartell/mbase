#ifndef MBASE_INF_PROCESSOR_H
#define MBASE_INF_PROCESSOR_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <mbase/string.h>
#include <mbase/synchronization.h>
#include <mbase/behaviors.h>
#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_context_line.h>
#include <mbase/inference/inf_sampling.h>
#include <mbase/framework/logical_processing.h>
#include <mutex>
#include <llama.h>

MBASE_BEGIN

static const U32 gProcessorMinimumTokenCount = 32;
static const U32 gProcessorMinimumInactivityThreshold = 16; // in seconds

class InfClientTextToText;

struct MBASE_API InfSamplerMeta {
	mbase::string mSamplerName = "";
	llama_sampler* mSamplerHandle = NULL;
	bool mIsOwnedByChain = false;
};

class MBASE_API InfProcessorBase : public mbase::logical_processor {
public:
	using size_type = SIZE_T;

	enum class flags : U8 {
		INF_PROC_SUCCESS,
		INF_PROC_ERR_UNREGISTERED_PROCESSOR,
		INF_PROC_ERR_ALREADY_INITIALIZED,
		INF_PROC_ERR_ALREADY_PROCESSING,
		INF_PROC_ERR_BELONGS_TO_ANOTHER_PROCESSOR,
		INF_PROC_ERR_MODEL_IS_NOT_INITIALIZED,
		INF_PROC_ERR_TOKEN_LIMIT_IS_TOO_LOW,
		INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT,
		INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT,
		INF_PROC_ERR_INPUT_IS_EMPTY,
		INF_PROC_ERR_MISSING_CLIENT,
		INF_PROC_ERR_SAMPLER_NAME_MISMATCH,
		INF_PROC_INFO_INITIALIZING,
		INF_PROC_INFO_DESTROYING,
		INF_PROC_INFO_HALTED,
		INF_PROC_INFO_CONTINUE,
		INF_PROC_INFO_NEED_UPDATE
	};

	InfProcessorBase();

	bool is_registered() const;
	bool is_running();
	bool signal_state_initializing() const;
	bool signal_state_destroying() const;
	bool signal_initializing() const;
	bool signal_destroying() const;

	flags get_context_size(U32& out_size);
	InfModelBase* get_processed_model();
	U32 get_inactivity_threshold();
	processor_signal& get_initialize_signal();
	processor_signal& get_destroy_signal();

	GENERIC set_inactivity_threshold(U32 in_threshold);
	GENERIC halt();
	GENERIC resume();

protected:
	//InfInactiveClientCleaner mClientCleaner;
	InfModelBase* mTargetModel_md_model;
	bool mIsRunning;
	bool mIsRegistered;
	processor_signal mInitializeSignal;
	processor_signal mDestroySignal;

	U32 mContextSize_md_model;
	U32 mProcessorId_md_model;
	U32 mInactivityThreshold;
}; // TODO: speech-to-text(whisper.cpp), text-to-text(llama.cpp), text-to-speech<EXPERIMENTAL>(bark.cpp)

class MBASE_API InfTextToTextProcessor : public mbase::InfProcessorBase {
public:
	using inf_token = llama_token;
	using inf_token_candidates = mbase::vector<llama_token_data>;

	enum class context_state {
		DECODING_INPUT,
		GENERATING_OUTPUT,
		AWAITING_FOR_INPUT,
		AWAITING_FOR_CURSOR_ALIGNMENT
	};

	enum class finish_state {
		FINISHED,
		CONTINUE,
		ABANDONED,
		TOKEN_LIMIT_REACHED
	};

	InfTextToTextProcessor();
	~InfTextToTextProcessor();

	bool is_available() const;
	bool signal_state_input_process() const;
	bool signal_state_decode_process() const;
	bool signal_input_process() const;
	bool signal_decode_process() const;
	bool signal_token_generated() const;
	bool signal_init_method() const;
	bool signal_destroy_method() const;
	#ifdef MBASE_INTERNAL_API
		inf_token_candidates& get_token_candidates();
	#endif // MBASE_INTERNAL_API
	U32 get_max_token_length();
	InfClientTextToText* get_assigned_client();
	bool has_sampler(const mbase::string& in_sampler_name, InfSamplerMeta& out_sampler);
	GENERIC get_available_samplers(mbase::vector<InfSamplerMeta>& out_samplers);
	bool has_client() const;
	flags get_processor_status() const;

	flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, mbase::vector<inf_token>& out_tokens);
	flags tokenize_input(context_line* in_lines, size_type in_count, mbase::vector<inf_token>& out_tokens, bool in_append_assistant_token = true);
	flags execute_input(const mbase::vector<inf_token>& in_tokens, bool in_abandon = false);
	flags next();
	flags set_inference_client(InfClientTextToText* in_client, bool in_reset_on_set = true);
	flags initialize(InfModelTextToText* in_model, U32 in_context_length);
	flags initialize_sync(InfModelTextToText* in_model, U32 in_context_length);
	flags destroy();
	flags destroy_sync();
	GENERIC release_inference_client();
	#ifdef MBASE_INTERNAL_API
		GENERIC clear_token_candidates();
	#endif // MBASE_INTERNAL_API
	GENERIC update() override;
	GENERIC update_t() override;
	GENERIC clear_samplers();
	flags add_sampler(const InfSamplingInput& in_sampling);

	GENERIC update_sampler_value(const mbase::string& in_sampler_name, F32 in_common_float);

	virtual GENERIC on_initialize() = 0;
	virtual GENERIC on_destroy() = 0;

private:
	GENERIC _decode_input();
	GENERIC _decode_next();
	GENERIC _initialize_context();
	GENERIC _destroy_context();

	llama_context* mModelContext;
	llama_batch mInputBatch;
	inf_token_candidates mPresetCandidates;
	inf_token mGeneratedToken;
	U32 mContextLength;
	U32 mContextCursor; // -----> if it exceeds the context size, stop generating
	mbase::vector<inf_token> mTokenizedInput;
	mbase::vector<inf_token> mPenaltyList;
	mbase::vector<InfSamplerMeta> mSamplingOrder;
	processor_signal mInputSignal;
	processor_signal mTokenGeneratedSignal;
	processor_signal mDecodeSignal;
	processor_signal mInitializeMethodSignal;
	processor_signal mDestroyMethodSignal;
	context_state mContextState;
	finish_state mFinishState;
	InfClientTextToText* mAssignedClient;
	llama_sampler* mSamplerChain;
	llama_sampler* mGreedy;
};

MBASE_END

#endif // !MBASE_INF_PROCESSOR_H
