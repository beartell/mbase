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
	bool mIsInitialized;
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
		INF_MODEL_ERR_INVALID_CONTEXT_LENGTH,
		INF_MODEL_ERR_PROCESSOR_NOT_FOUND,
		INF_MODEL_ERR_PROCESSOR_BELONGS_TO_ANOTHER_MODEL,
		INF_MODEL_ERR_UNABLE_REGISTER_PROCESSOR,
		INF_MODEL_ERR_NOT_INITIALIZED,
		INF_MODEL_ERR_GENERIC
	};

	InfModelTextToText();
	~InfModelTextToText();

	bool signal_init_method() const;
	bool signal_destroy_method() const;
	llama_model* get_raw_model();
	flags get_special_tokens(mbase::vector<inf_token>& out_tokens);
	flags get_special_tokens(mbase::vector<mbase::string>& out_tokens);
	flags get_model_name(mbase::string& out_name);
	flags get_vocabulary_type(mbase::string& out_type);
	flags get_architecture(mbase::string& out_architecture);
	flags get_finetune_type(mbase::string& out_type);
	flags get_embedding_length(I32& out_length);
	flags get_rope_type(mbase::string& out_type);
	flags get_sys_start(mbase::string& out_start);
	flags get_sys_start(mbase::vector<inf_token>& out_tokens);
	flags get_assistant_start(mbase::string& out_start);
	flags get_assistant_start(mbase::vector<inf_token>& out_tokens);
	flags get_usr_start(mbase::string& out_start);
	flags get_usr_start(mbase::vector<inf_token>& out_tokens);
	flags get_sys_end(mbase::string& out_end);
	flags get_assistant_end(mbase::string& out_end);
	flags get_usr_end(mbase::string& out_end);
	flags get_vocab_count(I32& out_count);
	flags get_model_param_count(size_type& out_count);
	flags get_model_params(mbase::unordered_map<mbase::string, mbase::string>& out_params);
	flags get_size(size_type& out_size);
	bool is_token_eof_generation(inf_token in_token);
	flags is_token_special(const mbase::string& in_string);
	flags is_token_control(inf_token in_token);
	flags get_metadata_count(size_type& out_count);

	flags initialize_model(const mbase::string& in_path, I32 in_gpu_layers = -1);
	flags initialize_model_sync(const mbase::string& in_path, I32 in_gpu_layers = -1);
	flags destroy();
	flags destroy_sync();
	flags register_context_process(InfTextToTextProcessor* in_processor, U32 in_context_length);

	virtual GENERIC on_initialize() = 0;
	virtual GENERIC on_destroy() = 0;

	GENERIC update() override;
	GENERIC update_t() override;
	
private:
	GENERIC _initialize_model();
	GENERIC _destroy_model();

	llama_model* mModel;
	mbase::string mEndOfTokenString;
	mbase::string mUsrStart;
	mbase::unordered_map<mbase::string, mbase::string> mModelKvals;
	mbase::string mSystemStart;
	mbase::string mAssistantStart; // maybe the same if the system and assistant is the same in a program
	mbase::string mModelPath;
	llama_model_params mSuppliedParams;
	inf_token mEndOfToken;
	processor_signal mInitMethodSignal;
	processor_signal mDestroyMethodSignal;
	mbase::vector<inf_token> mSystemStartTokenized;
	mbase::vector<inf_token> mAssistantStartTokenized;
	mbase::vector<inf_token> mUserStartTokenized;
};

MBASE_END

#endif // !MBASE_INF_MODEL_H
