#ifndef MBASE_OPENAI_CLIENT_H
#define MBASE_OPENAI_CLIENT_H

#include <mbase/common.h>
#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_t2t_client.h>
#include <cpp-httplib/httplib.h>

MBASE_BEGIN

class OpenaiTextToTextClient : public mbase::InfClientTextToText {
public:

    GENERIC set_http_data_sink(
        httplib::DataSink& in_data_sink, 
        httplib::Response& in_resp, 
        bool in_stream
    );

    GENERIC on_register(InfProcessorBase* out_processor) override;
    GENERIC on_unregister(InfProcessorBase* out_processor) override;
    GENERIC on_batch_processed(InfProcessorTextToText* out_processor, const U32& out_proc_batch_length, [[maybe_unused]] const bool& out_is_kv_locked) override;
    GENERIC on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) override;
    GENERIC on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state) override;

private:
    bool mStreamMod = false;
    httplib::DataSink* mDataSink = NULL;
    httplib::Response* mInResponse = NULL;
    mbase::string mClientId;
};

MBASE_END

#endif // MBASE_OPENAI_CLIENT_H