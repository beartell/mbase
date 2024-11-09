#ifndef MBASE_INF_EMBEDDER_H
#define MBASE_INF_EMBEDDER_H

#include <mbase/common.h>
#include <mbase/inference/inf_processor.h>

MBASE_BEGIN

class InfModelTextToText;

class MBASE_API InfEmbedderProcessor : public mbase::InfProcessorBase {
public:

    enum class init_fail_code {
        MODEL_NOT_INITIALIZED,
        NOT_ENOUGH_MEMORY,
        INVALID_MODEL_TYPE
    };

    InfEmbedderProcessor();
    ~InfEmbedderProcessor();

    init_fail_code get_last_fail_code() const;
    bool is_available() const;
    bool signal_init_method() const;
	bool signal_destroy_method() const;
	bool signal_init_fail_method() const;
    bool signal_embedding_process() const;
    bool signal_embedding_vector_generated() const;
    const U32& get_embedding_length();
    const U32& get_max_token_length();
    InfClientTextToText* get_assigned_client();

    flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens);
    flags execute_input(const inf_text_token_vector& in_tokens, bool in_abandon = false);
    flags set_inference_client(InfClientTextToText* in_client);
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
    GENERIC release_inference_client();
    GENERIC update() override;
    GENERIC update_t() override;

    virtual GENERIC on_initializing();
	virtual GENERIC on_initialize_fail(init_fail_code out_code);
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
    inf_text_token_vector mTokenizedInput;
    inf_embedding_vector mEmbeddingVector;
    processor_signal mEmbeddingSignal;
    processor_signal mVectorGenerated;
    processor_signal mInitializeMethodSignal;
    processor_signal mDestroyMethodSignal;
    processor_signal mInitializeFailSignal;
    InfClientTextToText* mAssignedClient;
    init_fail_code mInitFailCode;
};

MBASE_END

#endif