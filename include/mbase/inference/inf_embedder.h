#ifndef MBASE_INF_EMBEDDER_H
#define MBASE_INF_EMBEDDER_H

#include <mbase/common.h>
#include <mbase/inference/inf_processor.h>

MBASE_BEGIN

struct embedder_behavior_description {
    U32 mEmbeddingAtMost = 1;
    bool mHaltOnWrite = false;
};

class InfModelTextToText;

class MBASE_API InfEmbedderProcessor : public mbase::InfProcessorBase {
public:
    enum class last_fail_code {
        MODEL_NOT_INITIALIZED,
        NOT_ENOUGH_MEMORY,
        INVALID_MODEL_TYPE
    };

    InfEmbedderProcessor();
    ~InfEmbedderProcessor();

    last_fail_code get_last_fail_code() const;
    bool is_init_failed() const;
    bool is_available() const;
    bool signal_state_embedding_process() const;
    bool signal_embedding_process() const;
    const U32& get_embedding_length();
    const U32& get_max_token_length();

    flags get_processor_status() const;

    flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens);
    flags execute_input(const mbase::vector<inf_text_token_vector>& in_tokens, const embedder_behavior_description& in_description, bool in_abandon = false);
    flags next(const embedder_behavior_description& in_description);
    flags next_sync(const embedder_behavior_description& in_description);
    flags set_inference_client(InfClientBase* in_client);
    flags initialize(
        InfModelTextToText* in_model,
        const mbase::string& in_context_id,
        const U32& in_context_length,
        const U32& in_batch_size,
        const U32& in_thread_count
    );
    flags initialize_sync(
        InfModelTextToText* in_model,
        const mbase::string& in_context_id,
        const U32& in_context_length,
        const U32& in_batch_size,
        const U32& in_thread_count
    );
    flags destroy() override;
    flags destroy_sync() override;
    GENERIC update() override;
    GENERIC update_t() override;

    virtual GENERIC on_initializing();
	virtual GENERIC on_initialize_fail(last_fail_code out_code);
	virtual GENERIC on_destroying();
	virtual GENERIC on_initialize() = 0;
	virtual GENERIC on_destroy() = 0;

private:
    GENERIC _initialize_context();
    GENERIC _destroy_context();
    GENERIC _calculate_embeddings();

    llama_context* mModelContext;
    llama_batch mInputBatch;
    U32 mEmbeddingLength;
    U32 mBatchSize;
    U32 mThreadCount;
    U32 mEmbeddingVectorIndex;
    mbase::vector<inf_text_token_vector> mTokenizedInput;
    mbase::vector<inf_embedding_vector> mEmbeddingVector;
    processor_signal mEmbeddingSignal;
    last_fail_code mLastFailCode;
    finish_state mFinishState;
    bool mIsInitializeFailed;
    embedder_behavior_description mEmbedderBehavior;
};

MBASE_END

#endif