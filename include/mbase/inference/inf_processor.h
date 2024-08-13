#ifndef MBASE_INF_PROCESSOR_H
#define MBASE_INF_PROCESSOR_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <mbase/string.h>
#include <mbase/synchronization.h>
#include <mbase/behaviors.h>
#include <mbase/framework/timer_loop.h>
#include <mbase/framework/timers.h>
#include <mutex>
#include <llama.h>

MBASE_BEGIN

static const U32 gProcessorMinimumTokenCount = 32;

class InfModel;
class InfClient;
class InfProcessor;

class MBASE_API InfProcessor : public mbase::non_copyable {
public:
	using inf_token = llama_token;
	using size_type = SIZE_T;
	using client_list = mbase::list<InfClient*>;
	using logic_handlers = mbase::vector<InfClient*>;

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
		INF_PROC_WARN_UNABLE_TO_TOKENIZE_INPUT
	};

	InfProcessor();
	~InfProcessor();

	bool is_registered() const;
	flags get_context_size(U32& out_size);
	flags get_client_count(U32& out_size);
	flags get_max_clients(U32& out_size);
	flags get_process_thread_count(U32& out_count);
	flags get_max_batch_size(U32& out_size);
	InfModel* get_processed_model();
	flags destroy();
	flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, mbase::vector<inf_token>& out_tokens);
	flags register_client(CBYTEBUFFER in_data, size_type in_size, InfClient& out_client, U32 in_token_limit = 256);
	flags register_client(const mbase::string& in_string, InfClient& out_client, U32 in_token_limit = 256);
	flags register_client(const mbase::vector<inf_token>& in_data, InfClient& out_client, U32 in_token_limit = 256);
	flags register_client(InfClient& out_client, U32 in_token_limit = 256);
	flags unregister_client(InfClient& in_client);
	GENERIC set_max_client_count(U32 in_max_clients);
	GENERIC update();
	GENERIC update_t();

protected:
	logic_handlers mProcessedHandlers;
	client_list mRegisteredClients;
	mbase::vector<llama_token_data> mPresetCandidates;
	InfModel* mProcessedModel;
	llama_context* mModelContext;
	U32 mProcessorId;
	U32 mContextIdCounter;
	U32 mMaxClients;
	U32 mRegisteredBatchSize;
	mbase::mutex mClientsMutex;
	mbase::mutex mProcHandlerMutex;

	// mThreadCount === from llama_n_threads(llama_context)
	// mBatchThreadCount === from llama_n_threads_batch(llama_context)
};

MBASE_END

#endif // !MBASE_INF_PROCESSOR_H
