#ifndef MBASE_INF_PROCESSOR_H
#define MBASE_INF_PROCESSOR_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <mbase/string.h>
#include <mbase/synchronization.h>
#include <mbase/behaviors.h>
#include <mbase/inference/inf_model.h>
#include <mutex>
#include <llama.h>

MBASE_BEGIN

static const U32 gProcessorMinimumTokenCount = 32;
static const U32 gProcessorMinimumInactivityThreshold = 16; // in seconds

class InfClient;
class InfProcessor;
class InfInactiveClientCleaner;

class MBASE_API InfInactiveClientCleaner : public mbase::time_interval {
public:
	InfInactiveClientCleaner(InfProcessor& in_host);
	GENERIC on_call(user_data in_data) override;

private:
	InfProcessor* mHostProcessor;
};

class MBASE_API InfProcessor : public mbase::non_copyable {
public:
	using inf_token = llama_token;
	using size_type = SIZE_T;
	using client_list = mbase::list<InfClient*>;
	using logic_handlers = mbase::vector<InfClient*>;
	using iterator = typename client_list::iterator;
	using const_iterator = typename client_list::const_iterator;
	using reverse_iterator = typename client_list::reverse_iterator;
	using const_reverse_iterator = typename client_list::const_reverse_iterator;

	friend class InfModel;

	enum class flags : U8 {
		INF_PROC_SUCCESS,
		INF_PROC_ERR_UNREGISTERED_PROCESSOR,
		INF_PROC_ERR_ALREADY_INITIALIZED,
		INF_PROC_ERR_ALREADY_PROCESSING,
		INF_PROC_ERR_INVALID_TOKEN_INPUT,
		INF_PROC_ERR_INVALID_TOKEN_SIZE,
		INF_PROC_ERR_CLIENT_ALREADY_REGISTERED,
		INF_PROC_ERR_MODEL_IS_NOT_INITIALIZED,
		INF_PROC_ERR_BELONGS_TO_ANOTHER_PROCESSOR,
		INF_PROC_ERR_CLIENT_LIMIT_REACHED,
		INF_PROC_ERR_EXCEED_TOKEN_LIMIT,
		INF_PROC_ERR_CONTEXT_IS_FULL,
		INF_PROC_ERR_TOKEN_LIMIT_IS_TOO_LOW,
		INF_PROC_ERR_HALTED,
		INF_PROC_WARN_UNABLE_TO_TOKENIZE_INPUT
	};

	InfProcessor();
	~InfProcessor();

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

	bool is_registered() const;
	bool is_running() const;
	flags get_context_size(U32& out_size);
	flags get_client_count(U32& out_size);
	flags get_max_clients(U32& out_size);
	flags get_process_thread_count(U32& out_count);
	flags get_max_batch_size(U32& out_size);
	InfModel* get_processed_model();
	U32 get_inactivity_threshold();
	InfInactiveClientCleaner* get_client_cleaner();

	flags destroy();
	flags _destroy(InfModel::iterator& _out_it);
	flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, mbase::vector<inf_token>& out_tokens);
	flags register_client(CBYTEBUFFER in_data, size_type in_size, InfClient& out_client, U32 in_token_limit = 256);
	flags register_client(const mbase::string& in_string, InfClient& out_client, U32 in_token_limit = 256);
	flags register_client(const mbase::vector<inf_token>& in_data, InfClient& out_client, U32 in_token_limit = 256);
	flags register_client(InfClient& out_client, U32 in_token_limit = 256);
	flags unregister_client(InfClient& in_client);
	GENERIC set_max_client_count(U32 in_max_clients);
	GENERIC set_inactivity_threshold(U32 in_threshold);
	GENERIC update_inactivity();
	GENERIC halt();
	GENERIC resume();
	GENERIC update();
	GENERIC update_t();

protected:
	logic_handlers mLogicHandlers;
	client_list mRegisteredClients;
	mbase::vector<llama_token_data> mPresetCandidates;
	InfModel* mProcessedModel;
	llama_context* mModelContext;
	U32 mProcessorId;
	U32 mContextIdCounter;
	U32 mMaxClients;
	U32 mRegisteredBatchSize;
	U32 mInactivityThreshold;
	mbase::mutex mClientsMutex;
	mbase::mutex mProcHandlerMutex;
	bool mIsRunning;
	InfInactiveClientCleaner mClientCleaner;

	// mThreadCount === from llama_n_threads(llama_context)
	// mBatchThreadCount === from llama_n_threads_batch(llama_context)
};

MBASE_END

#endif // !MBASE_INF_PROCESSOR_H
