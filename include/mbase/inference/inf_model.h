#ifndef MBASE_INF_MODEL_H
#define MBASE_INF_MODEL_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/unordered_map.h>
#include <mbase/behaviors.h>
#include <mbase/thread.h>
#include <mbase/framework/logical_processing.h>
#include <mbase/framework/thread_pool.h>
#include <mbase/framework/timer_loop.h>
#include <mbase/inference/inf_sampling_set.h>
#include <llama.h>

MBASE_BEGIN

class InfModelBase;
class InfProcessorBase;
class InfTextToTextProcessor;

class MBASE_API InfModelBase : public mbase::logical_processor {
public:
	using size_type = SIZE_T;
	using context_processor_list = mbase::list<InfProcessorBase*>;
	using iterator = typename context_processor_list::iterator;
	using const_iterator = typename context_processor_list::const_iterator;
	using reverse_iterator = typename context_processor_list::reverse_iterator;
	using const_reverse_iterator = typename context_processor_list::const_reverse_iterator;

	InfModelBase();

	iterator begin() noexcept;
	iterator end() noexcept;
	const_iterator begin() const noexcept;
	const_iterator end() const noexcept;
	const_iterator cbegin() const noexcept;
	const_iterator cend() const noexcept;
	reverse_iterator rbegin() noexcept;
	reverse_iterator rend() noexcept;
	const_reverse_iterator crbegin() const noexcept;
	const_reverse_iterator crend() const noexcept;

	bool is_initialized() const;
	bool signal_state_initializing() const;
	bool signal_state_destroying() const;
	bool signal_initializing() const;
	bool signal_destroying() const;
protected:
	volatile bool mIsInitialized;
	processor_signal mInitializeSignal;
	processor_signal mDestroySignal;

	context_processor_list mRegisteredProcessors;
	mbase::mutex mProcessorListMutex;
	mbase::timer_loop mModelTimer;
};

class MBASE_API InfModelTextToText : public InfModelBase {
public:
	using inf_token = llama_token;

	enum class flags : U8 {
		INF_MODEL_SUCCESS,
		INF_MODEL_ERR_CANT_LOAD_MODEL,
		INF_MODEL_ERR_MISSING_MODEL,
		INF_MODEL_ERR_NO_SENTENCE,
		INF_MODEL_INFO_REGISTERING_PROCESSOR,
		INF_MODEL_INFO_INITIALIZING_MODEL,
		INF_MODEL_INFO_DESTROYING_MODEL,
		INF_MODEL_INFO_PROCESSOR_IS_BEING_DESTROYED,
		INF_MODEL_ERR_PROCESSOR_ALREADY_REGISTERED,
		INF_MODEL_ERR_INVALID_INPUT,
		INF_MODEL_ERR_MODEL_CONTEXT_FULL,
		INF_MODEL_ERR_INVALID_CONTEXT_LENGTH,
		INF_MODEL_ERR_PROCESSOR_NOT_FOUND,
		INF_MODEL_ERR_PROCESSOR_BELONGS_TO_ANOTHER_MODEL,
		INF_MODEL_ERR_UNABLE_REGISTER_PROCESSOR,
		INF_MODEL_ERR_NOT_INITIALIZED,
		INF_MODEL_ERR_GENERIC
	};

	enum class init_fail_code : U8 {
		NOT_ENOUGH_MEMORY,
		MBASE_PARAMS_DONT_MATCH,
		PATH_NOT_FOUND,
		LLAMA_SYSTEM_ERROR
	};

	InfModelTextToText();
	~InfModelTextToText();

	bool is_available(const U32& in_context_size);
	bool signal_initializing_method() const;
	bool signal_init_fail_method() const;
	bool signal_init_method() const;
	bool signal_destroy_method() const;
	bool signal_destroying_method() const;
	llama_model* get_raw_model();
	flags get_special_tokens(mbase::vector<inf_token>& out_tokens);
	flags get_special_tokens(mbase::vector<mbase::string>& out_tokens);
	flags get_model_name(mbase::string& out_name);
	flags get_architecture(mbase::string& out_architecture);
	flags get_embedding_length(U32& out_length);
	flags get_sys_start(mbase::string& out_start);
	flags get_sys_start(mbase::vector<inf_token>& out_tokens);
	flags get_assistant_start(mbase::string& out_start);
	flags get_assistant_start(mbase::vector<inf_token>& out_tokens);
	flags get_usr_start(mbase::string& out_start);
	flags get_usr_start(mbase::vector<inf_token>& out_tokens);
	flags get_sys_end(mbase::string& out_end);
	flags get_assistant_end(mbase::string& out_end);
	flags get_usr_end(mbase::string& out_end);
	flags get_eot_token(inf_token& out_token);
	flags get_lf_token(inf_token& out_token);
	flags get_vocab_count(I32& out_count);
	flags get_size(size_type& out_size);
	bool is_token_eof_generation(inf_token in_token);
	flags is_token_special(const mbase::string& in_string);
	flags is_token_control(inf_token in_token);
	U32 get_total_context_size();
	U32 get_occupied_context_size();

	static bool get_mbase_chat_template_id(const mbase::string& in_architecture, mbase::string& out_id);

	flags initialize_model(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1);
	flags initialize_model_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1);
	flags load_model();
	flags destroy();
	flags destroy_sync();
	flags register_context_process(
		InfTextToTextProcessor* in_processor, 
		const U32& in_context_length,
		U32 in_batch_size,
		U32 in_thread_count,
		U32 in_batch_thread_count,
		const bool& in_flash_attention,
		const inf_sampling_set& in_sampler_set
	);

	virtual GENERIC on_initialize_fail(init_fail_code out_fail_code);
	virtual GENERIC on_initialize() = 0;
	virtual GENERIC on_destroy() = 0;

	GENERIC update() override;
	GENERIC update_t() override;

private:
	GENERIC _initialize_model();
	GENERIC _destroy_model();
	GENERIC _get_special_tokens(mbase::vector<inf_token>& out_tokens);
	GENERIC _get_special_tokens(mbase::vector<mbase::string>& out_tokens);

	llama_model* mModel;
	mbase::string mModelName;
	mbase::string mModelArchitecture;
	mbase::string mEmbeddedSystemPrompt;
	mbase::string mUsrStart;
	mbase::string mSystemStart;
	mbase::string mAssistantStart; // maybe the same if the system and assistant is the same in a program
	mbase::string mSystemEnd;
	mbase::string mAssistantEnd;
	mbase::string mUserEnd;
	mbase::wstring mModelPath;
	llama_model_params mSuppliedParams;
	inf_token mEndOfToken;
	processor_signal mInitFailSignal;
	processor_signal mInitMethodSignal;
	processor_signal mDestroyMethodSignal;
	processor_signal mDestroyingSignal;
	mbase::vector<inf_token> mSystemStartTokenized;
	mbase::vector<inf_token> mAssistantStartTokenized;
	mbase::vector<inf_token> mUserStartTokenized;
	U64 mModelSize;
	U32 mOccupiedContext;
	U32 mTotalContextSize;
	U32 mBlockCount;
	U32 mHeadCount;
	U32 mEmbeddingLength;
	F32 mQuantizationCoefficient;
	init_fail_code mInitFailCode;
	bool mHasEmbeddedSystemPrompt;
};

// class MBASE_API InfModelImageToText : public InfModelBase{ // possibly using llava
//	...
// }

// class MBASE_API InfModelSpeechToText : public InfModelBase{ // possibly using whisper.cpp
//	...
// }

// class MBASE_API InfModelTextToImage : public InfModelBase{ // possibly using stable_diffusion.cpp
//	...
// }

// class MBASE_API InfModelTextToSpeech : public InfModelBase{ // possibly using bark.cpp
//	...
// }

// class MBASE_API InfModelEmbedding : public InfModelBase{ // Implement soon
//	...
// }


MBASE_END

#endif // !MBASE_INF_MODEL_H
