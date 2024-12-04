#include <iostream>
#include <mbase/mbase_std.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_client.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_maip_server.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/inference/inf_maip_model_description.h>
#include <mbase/argument_get_value.h>
#include <mbase/maip_client.h>
#include <mbase/json/json.h>
#include <cpp-httplib/httplib.h>
#include <vector>
#include <set>
#include <unordered_map>
#include <signal.h>
#include <string>

using namespace mbase;

class OpenAiTextToTextHostedModel : public mbase::InfModelTextToText {

public:
    GENERIC on_initialize_fail(init_fail_code out_fail_code) override{ }
	GENERIC on_initialize() override{std::cout << "Model is initialized" << std::endl; }
	GENERIC on_destroy() override{}
private:

};

class OpenAiTextToTextProcessor : public mbase::InfTextToTextProcessor {

public:
    GENERIC on_initialize() override
    {
    }
	GENERIC on_destroy() override{}
private:

};

class OpenAiTextToTextClient : public mbase::InfClientTextToText {
public:
    bool is_processing()
    {
        return mProcessing;
    }
    const mbase::string& get_total_output()
    {
        return mTotalGeneratedOutput;
    }
    GENERIC set_stream_mod(bool in_stream_mode, httplib::DataSink* in_data_sink = NULL)
    {
        if(in_stream_mode && !in_data_sink)
        {
            // trying to setup stream mod without data sink
            // interesting move there
            return;
        }
        mStreamMod = in_stream_mode;
        mDataSink = in_data_sink;
    }
    GENERIC set_http_data_sink(httplib::DataSink& in_data_sink) 
    {
        mDataSink = &in_data_sink;
    }
    GENERIC on_register(InfProcessorBase* out_processor) override{}
    GENERIC on_unregister(InfProcessorBase* out_processor) override{}
    GENERIC on_write(InfTextToTextProcessor* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) override
    {
        inf_token_description itd;
        out_processor->token_to_description(out_token[0], itd);

        printf("%s", itd.mTokenString.c_str());
        if(out_is_finish)
        {
            mProcessing = false;
        }
        else
        {
            if(mStreamMod)
            {
                mDataSink->write(itd.mTokenString.c_str(), itd.mTokenString.size());
            }
            mTotalGeneratedOutput += std::move(itd.mTokenString);
            mbase::decode_behavior_description dbd;
            dbd.mTokenAtMost = 1;
            dbd.mHaltOnWrite = false;
            out_processor->next(dbd);
        }
    }
    GENERIC on_finish(InfTextToTextProcessor* out_processor, size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override{}
private:
    bool mStreamMod = false;
    bool mProcessing = true;
    httplib::DataSink* mDataSink;
    mbase::string mTotalGeneratedOutput; // this is for handling non stream mod
};

OpenAiTextToTextHostedModel gHostedModel;

GENERIC modelListHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    // WARNING: We are hosting single model
    // An example will be implemented for multi-model hosting
    mbase::string hostedModelName;
    gHostedModel.get_model_name(hostedModelName);

    mbase::Json dataObject;
    dataObject.setArray();
    dataObject[0]["id"] = hostedModelName;
    dataObject[0]["object"] = "model";
    dataObject[0]["created"] = (time_t)time(NULL);
    dataObject[0]["owned_by"] = "MBASE Infrastructure Project";

    mbase::Json responseJson;
    responseJson["object"] = "list";
    responseJson["data"] = dataObject;

    mbase::string responseString = responseJson.toString();
    in_resp.set_content(responseString.c_str(), responseString.size(), "application/json");
}

GENERIC modelRetrieveHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    mbase::string hostedModelName;
    gHostedModel.get_model_name(hostedModelName);

    mbase::Json dataObject;
    dataObject["id"] = hostedModelName;
    dataObject["object"] = "model";
    dataObject["created"] = (time_t)time(NULL);
    dataObject["owned_by"] = "MBASE Infrastructure Project";

    mbase::string responseString = dataObject.toString();
    in_resp.set_content(responseString.c_str(), responseString.size(), "application/json");
}

GENERIC completionHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{

}

GENERIC chatCompletionHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    // curl https://api.openai.com/v1/chat/completions \
    // -H "Content-Type: application/json" \
    // -H "Authorization: Bearer $OPENAI_API_KEY" \
    // -d '{
    //     "model": "gpt-4o",
    //     "messages": [
    //     {
    //         "role": "system",
    //         "content": "You are a helpful assistant."
    //     },
    //     {
    //         "role": "user",
    //         "content": "Hello!"
    //     }
    //     ],
    //     "stream": true
    // }'
    if(!in_req.body.size())
    {
        // Invalid request, handle later
    }
    mbase::string reqBody(in_req.body.c_str(), in_req.body.size());
    std::cout << "Received request body: " << reqBody << std::endl;
    std::pair<mbase::Json::Status, mbase::Json> parseResult = mbase::Json::parse(reqBody);

    if(parseResult.first != mbase::Json::Status::success)
    {
        // Parse failed, handle later
    }

    mbase::Json jsonObject = parseResult.second;
    if(!jsonObject.contains("model") || !jsonObject.contains("message"))
    {
        // Mandatory fields are not present. Handle later...
    }

    mbase::string requestedModel = jsonObject["model"].getString();
    mbase::vector<mbase::Json> messageObject = jsonObject["messages"].getArray();
    
    OpenAiTextToTextClient openaiT2tClient;

    mbase::vector<U32> totalMessageArray;
    for(mbase::vector<mbase::Json>::iterator It = messageObject.begin(); It != messageObject.end(); ++It)
    {
        mbase::Json& messageJson = *It;
        if(!messageJson.contains("role") || !messageJson.contains("content"))
        {
            // Mandatory fiels for message array are not present. Handle later...
        }
        else
        {
            mbase::string messageRole = messageJson["role"].getString();
            mbase::string messageContent = messageJson["content"].getString();
            if(!messageContent.size()){ continue; }

            U32 outMessageId;
            if(messageRole == "system")
            {
                openaiT2tClient.add_message(messageContent, mbase::context_role::SYSTEM, outMessageId);
            }
            else if(messageRole == "assistant")
            {
                openaiT2tClient.add_message(messageContent, mbase::context_role::ASSISTANT, outMessageId);
            }
            else if(messageRole == "user")
            {
                openaiT2tClient.add_message(messageContent, mbase::context_role::USER, outMessageId);
            }
            else
            {
                // Invalid role, handle later
            }

            totalMessageArray.push_back(outMessageId);
        }
    }

    if(!totalMessageArray.size())
    {
        // Message array is empty, do not bother processing, handle later...
    }

    OpenAiTextToTextProcessor openaiProcessor;
    if(gHostedModel.register_context_process(&openaiProcessor, 4096, 512, 32, true, {}) != OpenAiTextToTextHostedModel::flags::INF_MODEL_INFO_REGISTERING_PROCESSOR)
    {
        // Registeration is not possible for some reason
        // Always this motherfucker...
        // handle later
    }

    while(!openaiProcessor.is_registered()){} // Wait for registration

    mbase::vector<mbase::context_line> tmpContextLine;
    mbase::inf_text_token_vector tokenVectorOut;
    openaiT2tClient.get_message_array(totalMessageArray.data(), totalMessageArray.size(), tmpContextLine);

    if(openaiProcessor.tokenize_input(tmpContextLine.data(), tmpContextLine.size(), tokenVectorOut) != OpenAiTextToTextProcessor::flags::INF_PROC_SUCCESS)
    {
        // tokenizer failed for some reason
        // handler later
    }
    openaiProcessor.set_inference_client(&openaiT2tClient);
    openaiProcessor.execute_input(tokenVectorOut);
    
    mbase::decode_behavior_description dbd;
    dbd.mTokenAtMost = 1;
    dbd.mHaltOnWrite = false;
    
    std::cout << (int)openaiProcessor.next(dbd) << std::endl;
    while(openaiT2tClient.is_processing())
    {

    }

    const mbase::string& totalOutput = openaiT2tClient.get_total_output();
    in_resp.set_content(totalOutput.c_str(), totalOutput.size(), "text/plain");
}

GENERIC httpServerThread()
{
    httplib::Server svr;
    svr.Get("/chunked", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_chunked_content_provider(
            "text/plain",
            [](size_t offset, httplib::DataSink &sink) {
            
            OpenAiTextToTextClient openaiT2tClient;
            OpenAiTextToTextProcessor openait2tProcessor;
            gHostedModel.register_context_process(&openait2tProcessor, 4096, 512, 32, true, {});
            
            while(!openait2tProcessor.is_registered())
            {
                
            }
            
            U32 msgId = 0;
            mbase::context_line ctxLine;
            mbase::inf_text_token_vector tokenVector;
            
            openaiT2tClient.set_http_data_sink(sink);
            openaiT2tClient.add_message("Hello, what is your name?. Answer short.", mbase::context_role::USER, msgId);
            openaiT2tClient.get_message(msgId, ctxLine);
            
            openait2tProcessor.set_inference_client(&openaiT2tClient);
            openait2tProcessor.tokenize_input(&ctxLine, 1, tokenVector);
            openait2tProcessor.execute_input(tokenVector, false);
            
            mbase::decode_behavior_description dbd;
            dbd.mTokenAtMost = 1;
            dbd.mHaltOnWrite = false;
            
            openait2tProcessor.next(dbd);
            while(openaiT2tClient.is_processing())
            {
                
            }
            sink.done();
            return true; // return 'false' if you want to cancel the process.
            }
        );
    });

    svr.Get("/v1/models", modelListHandler);
    svr.Post("/completion", completionHandler);
    svr.Post("/completions", completionHandler);
    svr.Post("/v1/completions", completionHandler);
    svr.Post("/chat/completions", chatCompletionHandler);
    svr.Post("/v1/chat/completions", chatCompletionHandler);
    svr.listen("127.0.0.1", 8080);
}

int main(int argc, char** argv)
{
    mbase::thread serverThread(httpServerThread);
    serverThread.run();
    gHostedModel.initialize_model_sync(L"/home/erdog/Downloads/Llama-3.2-1B-Instruct-Q8_0.gguf", 99999999, 99);

    while(gHostedModel.is_initialized())
    {
        gHostedModel.update();
    }

    getchar();
    return 0;
}