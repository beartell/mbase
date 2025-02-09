#ifndef MBASE_OPENAI_CLIENT_H
#define MBASE_OPENAI_CLIENT_H

#include <mbase/common.h>
#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_t2t_client.h>
#include <mbase/inference/inf_embedder_client.h>
#include <cpp-httplib/httplib.h>
#include <mbase/json/json.h>

MBASE_BEGIN

class OpenaiTextToTextClient : public mbase::InfClientTextToText {
public:

    bool is_processing() const;

    GENERIC set_http_data_sink(
        httplib::DataSink* in_data_sink,
        httplib::Response& in_resp,
        bool in_stream,
        long long in_gen_limit
    );
    GENERIC set_is_processing(bool in_state);

    GENERIC on_register(InfProcessorBase* out_processor) override;
    GENERIC on_unregister(InfProcessorBase* out_processor) override;
    GENERIC on_batch_processed(InfProcessorTextToText* out_processor, const U32& out_proc_batch_length, [[maybe_unused]] const bool& out_is_kv_locked) override;
    GENERIC on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) override;
    GENERIC on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state) override;

private:
    bool mProcessing = false;
    bool mStreamMod = false;
    long long mGenLimit = 0;
    long long mGenCount = 0;
    httplib::DataSink* mDataSink = NULL;
    httplib::Response* mInResponse = NULL;
    mbase::string mClientId;
    mbase::string mAccumulatedResponse;
};

class OpenaiEmbedderClient : public mbase::InfClientEmbedder {
public:
    
    bool is_processing() const;

    GENERIC set_embedder_input(
        InfEmbedderProcessor* in_processor,
        httplib::Response& in_resp,
        const mbase::vector<inf_text_token_vector>& in_embeddings_list
    );

    GENERIC on_register(InfProcessorBase* out_processor) override;
    GENERIC on_unregister(InfProcessorBase* out_processor) override;
    GENERIC on_batch_processed(InfEmbedderProcessor* out_processor, const U32& out_proc_batch_length) override;
    GENERIC on_write(InfEmbedderProcessor* out_processor, PTRF32 out_embeddings, const U32& out_cursor, bool out_is_finished) override;
    GENERIC on_finish(InfEmbedderProcessor* out_processor, const size_type& out_total_processed_embeddings) override;

private:
    bool mIsProcessing = false;
    httplib::Response* mInResponse = NULL;
    mbase::string mClientId;
    mbase::Json mTotalProcessedResponse;
    mbase::vector<inf_text_token_vector> mEmbeddingTokensInput;
    I32 mPromptIndex;
};

MBASE_END

#endif // MBASE_OPENAI_CLIENT_H