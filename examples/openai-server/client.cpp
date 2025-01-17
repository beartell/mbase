#include "client.h"
#include "processor.h"

MBASE_BEGIN

GENERIC OpenaiTextToTextClient::set_http_data_sink(
    httplib::DataSink& in_data_sink, 
    httplib::Response& in_resp, 
    bool in_stream
)
{
    mDataSink = &in_data_sink;
    mInResponse = &in_resp;
    mStreamMod = in_stream;
}

GENERIC OpenaiTextToTextClient::on_register(InfProcessorBase* out_processor)
{
    std::cout << "Client awaiting instructions" << std::endl;
}

GENERIC OpenaiTextToTextClient::on_unregister(InfProcessorBase* out_processor)
{

}

GENERIC OpenaiTextToTextClient::on_batch_processed(InfProcessorTextToText* out_processor, const U32& out_proc_batch_length, [[maybe_unused]] const bool& out_is_kv_locked)
{
    fflush(stdout);
    mbase::decode_behavior_description dbd;
    dbd.mHaltDelay = 1;
    dbd.mHaltOnWrite = false;
    dbd.mTokenAtMost = 1;
    out_processor->next(dbd);
}

GENERIC OpenaiTextToTextClient::on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish)
{

    mbase::inf_token_description tokDesc;
    out_processor->token_to_description(out_token[0], tokDesc);

    fflush(stdout);
    printf("%s", tokDesc.mTokenString.c_str());

    mbase::decode_behavior_description dbd;
    dbd.mHaltDelay = 1;
    dbd.mHaltOnWrite = false;
    dbd.mTokenAtMost = 1;
    out_processor->next(dbd);

}

GENERIC OpenaiTextToTextClient::on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state)
{

}

MBASE_END