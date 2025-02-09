#include "client.h"
#include "processor.h"
#include "model.h"
#include <mbase/json/json.h>

MBASE_BEGIN

bool OpenaiTextToTextClient::is_processing() const
{
    return mProcessing;
}

GENERIC OpenaiTextToTextClient::set_http_data_sink(
    httplib::DataSink* in_data_sink, 
    httplib::Response& in_resp, 
    bool in_stream,
    long long in_gen_limit
)
{
    mDataSink = in_data_sink;
    mInResponse = &in_resp;
    mStreamMod = in_stream;
    mGenLimit = in_gen_limit;
    mClientId = mbase::string::generate_uuid();
}

GENERIC OpenaiTextToTextClient::set_is_processing(bool in_state)
{
    mProcessing = in_state;
}

GENERIC OpenaiTextToTextClient::on_register(InfProcessorBase* out_processor)
{
    
}

GENERIC OpenaiTextToTextClient::on_unregister(InfProcessorBase* out_processor)
{

}

GENERIC OpenaiTextToTextClient::on_batch_processed(InfProcessorTextToText* out_processor, const U32& out_proc_batch_length, [[maybe_unused]] const bool& out_is_kv_locked)
{
    if(out_is_kv_locked)
    {
        // this is for system prompt caching
        return;
    }
    mAccumulatedResponse.clear();
    mbase::decode_behavior_description dbd;
    dbd.mHaltDelay = 1;
    dbd.mHaltOnWrite = false;
    dbd.mTokenAtMost = 1;
    if(out_processor->next(dbd) != InfProcessorTextToText::flags::INF_PROC_SUCCESS)
    {
        set_is_processing(false);
    }
}

GENERIC OpenaiTextToTextClient::on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish)
{
    if(out_is_finish)
    {
        mGenCount = 0;
        return;
    }

    mbase::OpenaiTextToTextProcessor* tmpProcessor = static_cast<mbase::OpenaiTextToTextProcessor*>(out_processor);
    mbase::OpenaiModel* tmpModel = static_cast<mbase::OpenaiModel*>(tmpProcessor->get_processed_model());

    mGenCount++;
    if(mGenCount == mGenLimit)
    {
        mGenCount = 0;
        tmpProcessor->stop_processor();
        set_is_processing(false);
        return;
    }

    mbase::inf_token_description tokDesc;
    out_processor->token_to_description(out_token[0], tokDesc);
    if(mStreamMod)
    {
        // if stream mode

        if(!mDataSink->is_writable())
        {
            mAccumulatedResponse.clear();
            set_is_processing(false);
            return;
        }

        U64 currentTime = static_cast<U64>(time(NULL));
        mbase::string modelName = tmpModel->get_model_name();
        mbase::Json responseJson;
        mbase::Json choicesArray;

        responseJson["id"] = "chatcmpl-" + mClientId;
        responseJson["object"] = "chat.completion.chunk";
        responseJson["created"] = currentTime;
        responseJson["model"] = modelName;
        responseJson["system_fingerprint"] = "fp_none";

        choicesArray.setArray();
        choicesArray[0]["index"] = 0;
        choicesArray[0]["delta"]["role"] = "assistant";
        choicesArray[0]["delta"]["content"] = tokDesc.mTokenString;
        choicesArray[0]["logprops"] = nullptr;
        choicesArray[0]["finish_reason"] = nullptr;

        responseJson["choices"] = choicesArray;

        mbase::string completionJsonString = "data: " + responseJson.toString() + "\n\n";
        mDataSink->write(completionJsonString.c_str(), completionJsonString.size());
    }

    else
    {
        // if non-stream mode
        mAccumulatedResponse += std::move(tokDesc.mTokenString);
    }

    mbase::decode_behavior_description dbd;
    dbd.mHaltDelay = 1;
    dbd.mHaltOnWrite = false;
    dbd.mTokenAtMost = 1;
    if(out_processor->next(dbd) != InfProcessorTextToText::flags::INF_PROC_SUCCESS)
    {
        set_is_processing(false);
    }
}

GENERIC OpenaiTextToTextClient::on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state)
{
    mGenCount = 0;

    mbase::OpenaiTextToTextProcessor* tmpProcessor = static_cast<mbase::OpenaiTextToTextProcessor*>(out_processor);
    mbase::OpenaiModel* tmpModel = static_cast<mbase::OpenaiModel*>(tmpProcessor->get_processed_model());

    U64 currentTime = static_cast<U64>(time(NULL));
    mbase::string modelName = tmpModel->get_model_name();
    mbase::Json responseJson;
    mbase::Json choicesArray;

    responseJson["id"] = "chatcmpl-" + mClientId;
    responseJson["object"] = "chat.completion";
    responseJson["created"] = currentTime;
    responseJson["model"] = modelName;
    responseJson["system_fingerprint"] = "fp_none";

    choicesArray.setArray();
    choicesArray[0]["index"] = 0;
    choicesArray[0]["logprobs"] = nullptr;
    
    if(out_finish_state == InfProcessorTextToText::finish_state::FINISHED)
    {
        choicesArray[0]["finish_reason"] = "stop";
    }

    else if(out_finish_state == InfProcessorTextToText::finish_state::TOKEN_LIMIT_REACHED)
    {
        choicesArray[0]["finish_reason"] = "length";
    }

    else
    {
        choicesArray[0]["finish_reason"] = "system";
    }

    if(mStreamMod)
    {
        responseJson["object"] = "chat.completion.chunk";
        choicesArray[0]["delta"].setObject();

        responseJson["choices"] = choicesArray;

        mbase::string completionJsonString = "data: " + responseJson.toString() + "\n\n";

        if(mDataSink->is_writable())
        {
            mDataSink->write(completionJsonString.c_str(), completionJsonString.size());
            mDataSink->done();
        }
    }

    else
    {
        choicesArray[0]["message"]["role"] = "assistant";
        choicesArray[0]["message"]["content"] = mAccumulatedResponse;
        choicesArray[0]["message"]["refusal"] = nullptr;

        responseJson["choices"] = choicesArray;

        responseJson["usage"]["prompt_tokens"] = 0;
        responseJson["usage"]["completion_tokens"] = out_total_token_size;
        responseJson["usage"]["total_tokens"] = out_total_token_size;
        responseJson["usage"]["prompt_tokens_details"]["cached_tokens"] = 0;
        responseJson["usage"]["completion_tokens_details"]["reasoning_tokens"] = 0;
        responseJson["usage"]["completion_tokens_details"]["accepted_prediction_tokens"] = 0;
        responseJson["usage"]["completion_tokens_details"]["rejected_prediction_tokens"] = 0;

        mbase::string completionJsonString = responseJson.toString();
        mAccumulatedResponse.clear();
        mInResponse->set_content(completionJsonString.c_str(), completionJsonString.size(), "application/json");
    }

    set_is_processing(false);
}

bool OpenaiEmbedderClient::is_processing() const
{
    return mIsProcessing;
}

GENERIC OpenaiEmbedderClient::set_embedder_input(
    InfEmbedderProcessor* in_processor,
    httplib::Response& in_resp,
    const mbase::vector<inf_text_token_vector>& in_embeddings_list
)
{

    mInResponse = &in_resp;
    mEmbeddingTokensInput = in_embeddings_list;
    mTotalProcessedResponse = mbase::Json();
    mPromptIndex = 0;

    mTotalProcessedResponse.setObject();
    mTotalProcessedResponse["object"] = "list";
    mTotalProcessedResponse["data"].setArray();
    
    I32 promptTokens = 0;

    for(inf_text_token_vector& n: mEmbeddingTokensInput)
    {
        promptTokens += n.size();
    }

    mTotalProcessedResponse["usage"]["prompt_tokens"] = promptTokens;
    mTotalProcessedResponse["usage"]["total_tokens"] = promptTokens;

    InfModelTextToText* processedModel = static_cast<InfModelTextToText*>(in_processor->get_processed_model());
    mbase::string outName = processedModel->get_model_name();
    mTotalProcessedResponse["model"] = outName;
    mIsProcessing = true;

    in_processor->execute_input({mEmbeddingTokensInput.back()});
    mEmbeddingTokensInput.pop_back();
}

GENERIC OpenaiEmbedderClient::on_register(InfProcessorBase* out_processor)
{

}

GENERIC OpenaiEmbedderClient::on_unregister(InfProcessorBase* out_processor)
{

}

GENERIC OpenaiEmbedderClient::on_batch_processed(InfEmbedderProcessor* out_processor, const U32& out_proc_batch_length)
{
    // embeddings are generated
    out_processor->next();
}

GENERIC OpenaiEmbedderClient::on_write(InfEmbedderProcessor* out_processor, PTRF32 out_embeddings, const U32& out_cursor, bool out_is_finished)
{
    // embeddings are displayed
    mTotalProcessedResponse["data"][mPromptIndex]["object"] = "embedding";
    mTotalProcessedResponse["data"][mPromptIndex]["index"] = mPromptIndex;
    for(U32 i = 0; i < out_processor->get_embedding_length(); ++i)
    {
        mbase::inf_common_embd_normalize(out_embeddings, out_embeddings, out_processor->get_embedding_length());
        mTotalProcessedResponse["data"][mPromptIndex]["embedding"][i] = out_embeddings[i];
    }
}

GENERIC OpenaiEmbedderClient::on_finish(InfEmbedderProcessor* out_processor, const size_type& out_total_processed_embeddings)
{
    // embeddings display is finished

    ++mPromptIndex;
    if(!mEmbeddingTokensInput.size())
    {
        mbase::string embeddingsJsonString = mTotalProcessedResponse.toStringPretty();
        mInResponse->set_content(embeddingsJsonString.c_str(), embeddingsJsonString.size(), "application/json");
        mIsProcessing = false;
    }

    else
    {
        out_processor->execute_input({mEmbeddingTokensInput.back()});
        mEmbeddingTokensInput.pop_back();
    }
}


MBASE_END