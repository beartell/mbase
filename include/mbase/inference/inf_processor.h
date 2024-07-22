#ifndef MBASE_INF_PROCESSOR_H
#define MBASE_INF_PROCESSOR_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <mbase/synchronization.h>
#include <llama.h>

MBASE_BEGIN

static U32 gInfProcessorIdCounter = 1;

static const U32 gInfProcessorDefaultCtxLength = 512;
static const U32 gInfProcessorDefaultBatchSize = 512;
static const U32 gInfProcessorDefaultMaxSeq = 128;
static const U32 gInfProcessorDefaultThreadCount = 16;

static const U32 gInfProcessorMinCtxLength = 64;
static const U32 gInfProcessorMinBatchSize = 64;
static const U32 gInfProcessorMinSeq = 1;
static const U32 gInfProcessorMinThreadCount = 1;

class InfModel;
class InfClient;

struct InfProcInitParams {
	U32 mContextLength = gInfProcessorDefaultCtxLength;
	U32 mBatchSize = gInfProcessorDefaultBatchSize;
	U32 mMaxSequence = gInfProcessorDefaultMaxSeq;
	U32 mThreadCount = gInfProcessorDefaultThreadCount;
};

class InfProcessor {
public:
	using inf_token = llama_token;
	using size_type = SIZE_T;
	using client_list = mbase::list<InfClient*>;
	using logic_handlers = mbase::vector<InfClient*>;

	enum class flags : U8 {
		INF_PROC_SUCCESS,
		INF_PROC_ERR_NOT_INITIALIZED,
		INF_PROC_ERR_ALREADY_INITIALIZED,
		INF_PROC_ERR_ALREADY_PROCESSING,
		INF_PROC_ERR_INVALID_TOKEN_INPUT,
		INF_PROC_ERR_INVALID_TOKEN_SIZE,
		INF_PROC_ERR_CLIENT_ALREADY_REGISTERED,
		INF_PROC_ERR_MODEL_IS_NOT_INITIALIZED,
		INF_PROC_ERR_BELONGS_TO_ANOTHER_PROCESSOR,
		INF_PROC_ERR_CLIENT_LIMIT_REACHED,
		INF_PROC_WARN_UNABLE_TO_TOKENIZE_INPUT
	};

	InfProcessor();
	~InfProcessor();

	bool is_initialized() const;
	flags get_client_count(I32& out_size);
	flags get_max_clients(I32& out_size);
	flags get_process_thread_count(I32& out_count);
	flags get_batch_size(I32& out_size);

	flags initialize(InfModel* in_model, InfProcInitParams in_params);
	flags destroy();
	flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, mbase::vector<inf_token>& out_tokens);
	flags register_client(CBYTEBUFFER in_data, size_type in_size, InfClient& out_client);
	flags register_client(const mbase::vector<inf_token>& in_data, InfClient& out_client);
	flags register_client(InfClient& out_client);
	flags unregister_client(InfClient& in_client);
	GENERIC update();
	GENERIC update(InfClient& in_client);
	GENERIC update_t();
	GENERIC update_t(InfClient& in_client);

private:
	logic_handlers mProcessedHandlers;
	logic_handlers mFinishHandlers;
	mbase::vector<llama_token_data> mPresetCandidates;
	mbase::deep_char_stream mGeneratedToken;
	client_list mRegisteredClients;
	InfModel* mProcessedModel;
	llama_context* mModelContext;
	U32 mProcessorId;
	U32 mMaxClients;
	mbase::mutex mClientsMutex;

	// mThreadCount === from llama_n_threads(llama_context)
	// mBatchThreadCount === from llama_n_threads_batch(llama_context)
};

MBASE_END

#endif // !MBASE_INF_PROCESSOR_H
