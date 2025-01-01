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
#include <mbase/inference/inf_device_desc.h>
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

#define MBASE_OPENAI_SERVER_VERSION "v1.2.0"
GENERIC print_usage();
GENERIC print_usage()
{
    printf("========================================\n");
    printf("#Program name:      mbase-openai-server\n");
    printf("#Version:           %s\n", MBASE_OPENAI_SERVER_VERSION);
    printf("#Type:              Example\n");
    printf("#Further docs: \n");
    printf("***** DESCRIPTION *****\n");
    printf("Openai api compatible http server for serving llms.\n");
    printf("Keep in mind that this implementation is an example that show what is possible with MBASE.\n");
    printf("The chat completion api is implemented however the plain completion and embedding api is missing for now.\n");
    printf("They will be implemented in the future.\n");
    printf("========================================\n\n");
    printf("Usage: mbase-openai-server *[<option> [<value>]]\n");
    printf("Options: \n\n");
    printf("--help                          Print usage.\n");
    printf("-v, --version                   Shows program version.\n");
    printf("--api-key <str>                 Api key.\n");
    printf("-h, --hostname <str>            Hostname to listen to (default=127.0.0.1).\n");
    printf("-p, --port <int>                Port to assign to (default=8080).\n");
    printf("-al, --access-limit <int>       Amount of clients that can access concurrently (default=-1).\n");
    printf("-m, --model-path <str>          Model file to be hosted. To host multiple models, pass this argument multiple times.\n");
    printf("-t, --thread-count <int>        Amount of threads to use for output processing (default=16).\n");
    printf("-bt, --batch-thread-count <int> Amount of threads to use for initial batch processing (default=8).\n");
    printf("-c, --context-length <int>      Total context length (default=8192).\n");
    printf("-b, --batch-length <int>        Batch length (default=4096).\n");
    printf("-gl, --gpu-layers <int>         GPU layers to offload to (default=999).\n\n");

    // printf("-tk, --top-k <int>              Top k tokens to pick from (default=20, min=1, max=<model_vocabulary_size>).\n");
    // printf("-tp, --top-p <float>            Token probability at most (default=1.0, min=0.1, max=1.0).\n");
    // printf("-mp, --min-p <float>            Token probability at least (default=0.3), min=0.1, max=1.0.\n");
    // printf("-pn, --penatly-n <int>          Apply repetition penalty on last 'n' tokens (default=64).\n");
    // printf("-pr, --penalty-repeat <float>   Discourages repeating exact tokens based on their past presence (default=1.0, min=1.0, max=2.0).\n");
    // printf("-temp, --temperature <float>    Higher values may make model go crazy (default=0.1, min=0.1, max=1.4).\n");

}

struct mbase_openai_sample_params {
    mbase::string mApiKey;
    mbase::string mHostname = "127.0.0.1"; // --hostname || -h
    I32 mPort = 8080; // --port || -p
    U32 mAccessLimit = -1;
    mbase::vector<mbase::string> mModelFiles; // --model-path || -m
    I32 mThreadCount = 16; // --thread-count || -t
    I32 mBatchThreadCount = 8; // --batch-thread-count || -bt
    I32 mContextLength = 8192; // --context-length || -c
    I32 mBatchLength = 4096; // --batch-length || -b
    I32 mGpuLayer = 999; // --gpu-layers || -gl
};

class OpenAiTextToTextHostedModel : public mbase::InfModelTextToText {
public:
    OpenAiTextToTextHostedModel()
    {
        mCreationDate = time(NULL);
    }
    time_t get_creation_date_in_epoch(){ return mCreationDate; }
    GENERIC on_initialize_fail(init_fail_code out_fail_code) override{ }
	GENERIC on_initialize() override{}
	GENERIC on_destroy() override{}
private:
    time_t mCreationDate;
};

class OpenAiTextToTextProcessor : public mbase::InfProcessorTextToText {

public:
    GENERIC on_initialize() override
    {
    }
	GENERIC on_destroy() override{}
private:

};

class OpenAiTextToTextClient : public mbase::InfClientTextToText {
public:
    OpenAiTextToTextClient(const httplib::Request& in_req, httplib::Response& in_resp, const mbase::string& in_model_name)
    {
        mInRequest = &in_req;
        mInResponse = &in_resp;
        mUsedModel = in_model_name;
        mClientId = mbase::string::generate_uuid();
    }
    bool is_processing() const
    {
        return mProcessing;
    }
    bool is_stream_mod() const
    {
        return mStreamMod;
    }
    const mbase::string& get_total_output() const 
    {
        return mTotalGeneratedOutput;
    }
    GENERIC set_stream_mod(bool in_stream_mode, httplib::DataSink* in_data_sink = NULL)
    {
        mStreamMod = in_stream_mode;
        mDataSink = in_data_sink;
    }
    GENERIC set_http_data_sink(httplib::DataSink& in_data_sink) 
    {
        mDataSink = &in_data_sink;
    }
    GENERIC on_register(InfProcessorBase* out_processor) override{}
    GENERIC on_unregister(InfProcessorBase* out_processor) override{}
    GENERIC on_batch_processed(InfProcessorTextToText* out_processor, const U32& out_proc_batch_length, [[maybe_unused]] const bool& out_is_kv_locked) override
    {

    }
    GENERIC on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) override
    {
        inf_token_description itd;
        out_processor->token_to_description(out_token[0], itd);
        
        if(out_is_finish)
        {
        }
        else
        {
            if(is_stream_mod())
            {
                if(!mDataSink->is_writable())
                {
                    // Means client is disconnected
                    mProcessing = false;
                    return;
                }
                mbase::Json activeJson;
                mbase::Json choicesArray;
                mbase::Json choiceFirstItem;

                activeJson["id"] = "chatcmpl-" + mClientId;
                activeJson["object"] = "chat.completion.chunk";
                activeJson["created"] = (time_t)time(NULL);
                activeJson["model"] = mUsedModel;
                activeJson["system_fingerprint"] = "fp_none";
                
                choiceFirstItem["index"] = 0;
                choiceFirstItem["delta"]["role"] = "assistant";
                choiceFirstItem["delta"]["content"] = itd.mTokenString;
                choiceFirstItem["logprops"] = nullptr;
                choiceFirstItem["finish_reason"] = nullptr;

                choicesArray.setArray();
                choicesArray[0] = choiceFirstItem;

                activeJson["choices"] = choicesArray;

                mbase::string completionJsonString = "data: " + activeJson.toString() + "\n\n";
                mDataSink->write(completionJsonString.c_str(), completionJsonString.size());
            }
            else
            {
                mTotalGeneratedOutput += std::move(itd.mTokenString);
            }
            
            mbase::decode_behavior_description dbd;
            dbd.mTokenAtMost = 1;
            dbd.mHaltOnWrite = false;
            out_processor->next(dbd);
        }
    }
    GENERIC on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state) override
    {  
        mbase::Json activeJson;
        mbase::Json choicesArray;
        mbase::Json choiceFirstItem;

        activeJson["id"] = "chatcmpl-" + mClientId;
        activeJson["object"] = "chat.completion";
        activeJson["created"] = (time_t)time(NULL);
        activeJson["model"] = mUsedModel;
        activeJson["system_fingerprint"] = "fp_none";

        choiceFirstItem["index"] = 0;
        choiceFirstItem["logprobs"] = nullptr;

        if(out_finish_state == InfProcessorTextToText::finish_state::FINISHED)
        {
            choiceFirstItem["finish_reason"] = "stop";
        }
        else if(out_finish_state == InfProcessorTextToText::finish_state::TOKEN_LIMIT_REACHED)
        {
            choiceFirstItem["finish_reason"] = "length";
        }
        else{
            choiceFirstItem["finish_reason"] = "system";
        }
        
        if(is_stream_mod())
        {
            activeJson["object"] = "chat.completion.chunk";
            choiceFirstItem["delta"].setObject();
            
            choicesArray.setArray();
            choicesArray[0] = choiceFirstItem;
            activeJson["choices"] = choicesArray;

            // {"id":"chatcmpl-123","object":"chat.completion.chunk","created":1694268190,"model":"gpt-4o-mini", "system_fingerprint": "fp_44709d6fcb", "choices":[{"index":0,"delta":{},"logprobs":null,"finish_reason":"stop"}]}

            mbase::string completionJsonString = "data: " + activeJson.toString() + "\n\n";
            
            mDataSink->write(completionJsonString.c_str(), completionJsonString.size());
            mDataSink->done();
        }
        else
        {
            choiceFirstItem["message"]["role"] = "assistant";
            choiceFirstItem["message"]["content"] = mTotalGeneratedOutput;
            choiceFirstItem["message"]["refusal"] = nullptr;

            choicesArray.setArray();
            choicesArray[0] = choiceFirstItem;

            activeJson["choices"] = choicesArray;
            activeJson["usage"]["prompt_tokens"] = 0; // handle later
            activeJson["usage"]["completion_tokens"] = out_total_token_size;
            activeJson["usage"]["total_tokens"] = out_total_token_size;
            activeJson["usage"]["prompt_tokens_details"]["cached_tokens"] = 0;
            activeJson["usage"]["completion_tokens_details"]["reasoning_tokens"] = 0;
            activeJson["usage"]["completion_tokens_details"]["accepted_prediction_tokens"] = 0;
            activeJson["usage"]["completion_tokens_details"]["rejected_prediction_tokens"] = 0;

            mbase::string completionJsonString = activeJson.toString();
            mInResponse->set_content(completionJsonString.c_str(), completionJsonString.size(), "application/json");
        }
        
        mProcessing = false;
    }
private:
    bool mStreamMod = false;
    bool mProcessing = true;
    httplib::DataSink* mDataSink = NULL;
    const httplib::Request* mInRequest = NULL;
    httplib::Response* mInResponse = NULL;
    mbase::string mUsedModel;
    mbase::string mTotalGeneratedOutput; // this is for handling non stream mod
    mbase::string mClientId;
};

mbase_openai_sample_params gSampleParams;
mbase::vector<OpenAiTextToTextHostedModel*> gHostedModelArray;
bool gIsServerListening = true;
mbase::mutex gContextDestructionSync;

mbase::string buildOpenaiError(
    const mbase::string& in_message,
    const mbase::string& in_type,
    const mbase::string& in_code)
{
    mbase::Json errorDescription;
    errorDescription["message"] = in_message;
    errorDescription["type"] = in_type;
    errorDescription["code"] = in_code;

    mbase::Json openaiError;
    openaiError["error"] = errorDescription;

    return openaiError.toString();
}

// returns true if authorization is successful
bool openaiAuthCheck(const httplib::Request& in_req, httplib::Response& in_resp, mbase::string& out_provided_key)
{
    if(!gSampleParams.mApiKey.size())
    {
        return true;
    }

    if(in_req.has_header("Authorization"))
    {
        std::string authToken = in_req.get_header_value("Authorization");
        mbase::string authTokenField(authToken.c_str(), authToken.size());
        mbase::vector<mbase::string> seperatedField;

        authTokenField.split(" ", seperatedField);
        if(seperatedField.size() != 2) // 'Bearer' and 'key'
        {
            return false;
        }

        mbase::string bearerString = seperatedField[0];
        out_provided_key = seperatedField[1];

        if(bearerString != "Bearer")
        {
            return false;
        }

        if(out_provided_key != gSampleParams.mApiKey)
        {
            return false;
        }

        return true;
    }
    
    return false;
}

GENERIC sendOpenaiError(
    const httplib::Request& in_req, 
    httplib::Response& in_resp,
    const mbase::string& in_message,
    const mbase::string& in_type,
    const mbase::string& in_code
)
{
    mbase::string responseString = buildOpenaiError(in_message, in_type, in_code);
    in_resp.set_content(responseString.c_str(), responseString.size(), "application/json");
}

GENERIC modelListHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    mbase::string providedKey = "";
    if(!openaiAuthCheck(in_req, in_resp, providedKey))
    {
        in_resp.status = 401;
        sendOpenaiError(
            in_req,
            in_resp,
            mbase::string::from_format("Invalid API key provided: [%s]", providedKey.c_str()),
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }

    mbase::Json dataObject;
    dataObject.setArray();

    for(I32 i = 0; i < gHostedModelArray.size(); i++)
    {
        OpenAiTextToTextHostedModel* tmpHostedModel = gHostedModelArray[i];
        mbase::string hostedModelName;
        tmpHostedModel->get_model_name(hostedModelName);
        dataObject[i]["id"] = hostedModelName;
        dataObject[i]["object"] = "model";
        dataObject[i]["created"] = tmpHostedModel->get_creation_date_in_epoch();
        dataObject[i]["owned_by"] = "MBASE Inference Infrastructure";
    }

    mbase::Json responseJson;
    responseJson["object"] = "list";
    responseJson["data"] = dataObject;

    mbase::string resultantString = responseJson.toString();
    in_resp.set_content(resultantString.c_str(), resultantString.size(), "application/json");
}

GENERIC modelRetrieveHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    mbase::string providedKey = "";
    if(!openaiAuthCheck(in_req, in_resp, providedKey))
    {
        in_resp.status = 401;
        sendOpenaiError(
            in_req,
            in_resp,
            mbase::string::from_format("Invalid API key provided: [%s]", providedKey.c_str()),
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }

    if(!in_req.path_params.at("model").size())
    {
        in_resp.status = 400;
        sendOpenaiError(
            in_req,
            in_resp,
            "Required parameter is missing: model",
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }

    mbase::string requestedModel(in_req.path_params.at("model").c_str(), in_req.path_params.at("model").size()); 

    for(mbase::vector<OpenAiTextToTextHostedModel*>::iterator It = gHostedModelArray.begin(); It != gHostedModelArray.end(); ++It)
    {
        OpenAiTextToTextHostedModel* tmpHostedModel = *It;
        mbase::string tmpModelName;
        tmpHostedModel->get_model_name(tmpModelName);
        if(requestedModel == tmpModelName)
        {
            mbase::Json dataObject;
            dataObject["id"] = tmpModelName;
            dataObject["object"] = "model";
            dataObject["created"] = tmpHostedModel->get_creation_date_in_epoch();
            dataObject["owned_by"] = "MBASE Infrastructure Project";
            
            mbase::string responseString = dataObject.toString();
            in_resp.set_content(responseString.c_str(), responseString.size(), "application/json");
            return;
        }
    }

    sendOpenaiError(
        in_req,
        in_resp,
        "The specified model is not available or invalid.",
        "invalid_request_error",
        "invalid_model"
    );
}

GENERIC chatCompletionInternal(const httplib::Request& in_req, httplib::Response& in_resp, bool is_chat_completion)
{
    mbase::string providedKey = "";
    if(!openaiAuthCheck(in_req, in_resp, providedKey))
    {
        in_resp.status = 401;
        sendOpenaiError(
            in_req,
            in_resp,
            mbase::string::from_format("Invalid API key provided: [%s]", providedKey.c_str()),
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }

    if(!in_req.body.size())
    {
        sendOpenaiError(
            in_req,
            in_resp,
            "Request body is missing",
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }

    mbase::string reqBody(in_req.body.c_str(), in_req.body.size());
    std::pair<mbase::Json::Status, mbase::Json> parseResult = mbase::Json::parse(reqBody);

    if(parseResult.first != mbase::Json::Status::success)
    {
        // Parse failed, handle later
        sendOpenaiError(
            in_req,
            in_resp,
            "Request body incorrect format",
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }

    mbase::Json jsonObject = parseResult.second;
    if(!jsonObject["model"].isString() || !jsonObject["messages"].isArray())
    {
        sendOpenaiError(
            in_req,
            in_resp,
            "Required parameter is missing: model",
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }
    
    mbase::string requestedModel = jsonObject["model"].getString();
    OpenAiTextToTextHostedModel* activeModel = NULL;

    for(mbase::vector<OpenAiTextToTextHostedModel*>::iterator It = gHostedModelArray.begin(); It != gHostedModelArray.end(); ++It)
    {
        OpenAiTextToTextHostedModel* tmpHostedModel = *It;
        mbase::string tmpModelName;
        tmpHostedModel->get_model_name(tmpModelName);
        if(requestedModel == tmpModelName)
        {
            activeModel = tmpHostedModel;
            break;
        }
    }

    if(!activeModel)
    {
        // Requested model not found,
        // Send error
        // handle later
        sendOpenaiError(
            in_req,
            in_resp,
            "The specified model is not available or invalid.",
            "invalid_request_error",
            "invalid_model"
        );
        return;
    }

    // TODO: 
    // After implementing scope_guard, 
    // remove all deletes with scope_guards

    mbase::vector<mbase::Json> messageObject = jsonObject["messages"].getArray();
    
    OpenAiTextToTextClient *openaiT2tClient = new OpenAiTextToTextClient(in_req, in_resp, requestedModel);

    mbase::vector<U32> totalMessageArray;
    for(mbase::vector<mbase::Json>::iterator It = messageObject.begin(); It != messageObject.end(); ++It)
    {
        mbase::Json& messageJson = *It;
        if(!messageJson["role"].isString() || !messageJson["content"].isString())
        {
            // Mandatory fiels for message array are not present. Handle later...
            sendOpenaiError(
                in_req,
                in_resp,
                "Each message in 'messages' must have a 'role' and 'content' field.",
                "invalid_request_error",
                "invalid_message_structure"
            );
            delete openaiT2tClient;
            return;
        }
        else
        {
            mbase::string messageRole = messageJson["role"].getString();
            mbase::string messageContent = messageJson["content"].getString();
            if(!messageContent.size())
            {
                sendOpenaiError(
                    in_req,
                    in_resp,
                    "Message's content field is empty.",
                    "invalid_request_error",
                    "invalid_message_structure"
                );
                delete openaiT2tClient;
                return;
            }

            U32 outMessageId;
            if(messageRole == "system")
            {
                openaiT2tClient->add_message(messageContent, mbase::context_role::SYSTEM, outMessageId);
            }
            else if(messageRole == "assistant")
            {
                openaiT2tClient->add_message(messageContent, mbase::context_role::ASSISTANT, outMessageId);
            }
            else if(messageRole == "user")
            {
                openaiT2tClient->add_message(messageContent, mbase::context_role::USER, outMessageId);
            }
            else
            {
                // Invalid role, handle later
                mbase::string roleErr = "Unrecognized value for '" + messageRole + "' in message.";
                sendOpenaiError(
                    in_req,
                    in_resp,
                    roleErr,
                    "invalid_request_error",
                    "invalid_role"
                );
                delete openaiT2tClient;
                return;
            }

            totalMessageArray.push_back(outMessageId);
        }
    }

    // temperature
    // mirostat
    // top_k
    // top_p
    // min_p
    //

    InfSamplerDescription topkSampler;
    InfSamplerDescription toppSampler;
    InfSamplerDescription minpSampler;
    InfSamplerDescription repetitionPenalty;

    topkSampler.mSamplerType = InfSamplerDescription::SAMPLER::TOP_K;
    topkSampler.mTopK = 40;

    toppSampler.mSamplerType = InfSamplerDescription::SAMPLER::TOP_P;
    toppSampler.mTopP = 1.0;

    minpSampler.mSamplerType = InfSamplerDescription::SAMPLER::MIN_P;
    minpSampler.mMinP = 0.3;

    repetitionPenalty.mSamplerType = InfSamplerDescription::SAMPLER::REPETITION;
    repetitionPenalty.mRepetition.mPenaltyN = 64;
    repetitionPenalty.mRepetition.mRepeatPenalty = 1.0;
    repetitionPenalty.mRepetition.mPenaltyFrequency = 0.0;
    repetitionPenalty.mRepetition.mPenaltyPresent = 0.0;

    OpenAiTextToTextProcessor* openaiProcessor = new OpenAiTextToTextProcessor;
    if(activeModel->register_context_process(openaiProcessor, gSampleParams.mContextLength, gSampleParams.mBatchLength, gSampleParams.mThreadCount, gSampleParams.mBatchThreadCount, true, {}) != OpenAiTextToTextHostedModel::flags::INF_MODEL_INFO_REGISTERING_PROCESSOR)
    {
        // Registeration is not possible for some reason
        // Always this motherfucker...
        // handle later
        sendOpenaiError(
            in_req,
            in_resp,
            "An internal server error occurred. Please try again later.",
            "api_error",
            "internal_server_error"
        );
        delete openaiProcessor;
        delete openaiT2tClient;
        return;
    }

    while(!openaiProcessor->is_registered()){} // Wait for registration

    mbase::vector<mbase::context_line> tmpContextLine;
    mbase::inf_text_token_vector tokenVectorOut;
    openaiT2tClient->get_message_array(totalMessageArray.data(), totalMessageArray.size(), tmpContextLine);

    if(openaiProcessor->tokenize_input(tmpContextLine.data(), tmpContextLine.size(), tokenVectorOut) != OpenAiTextToTextProcessor::flags::INF_PROC_SUCCESS)
    {
        // tokenizer failed for some reason
        // handler later
        sendOpenaiError(
            in_req,
            in_resp,
            "An internal server error occurred. Please try again later.",
            "api_error",
            "internal_server_error"
        );
        delete openaiProcessor;
        delete openaiT2tClient;
        return;
    }

    if(openaiProcessor->execute_input(tokenVectorOut) == OpenAiTextToTextProcessor::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT)
    {
        sendOpenaiError(
            in_req,
            in_resp,
            "The input text exceeds the maximum token limit.",
            "invalid_request_error",
            "max_tokens_exceeded"
        );
        gContextDestructionSync.acquire();
        delete openaiProcessor;
        gContextDestructionSync.release();
        delete openaiT2tClient;
        return;
    }
    
    openaiProcessor->set_inference_client(openaiT2tClient);

    if(jsonObject["stream"].isBool())
    {
        if(jsonObject["stream"].getBool())
        {
            in_resp.set_chunked_content_provider(
                "text/event-stream",
                [openaiT2tClient, openaiProcessor](size_t offset, httplib::DataSink &sink) {
                    openaiT2tClient->set_stream_mod(true, &sink);
                    mbase::decode_behavior_description dbd;
                    dbd.mTokenAtMost = 1;
                    dbd.mHaltOnWrite = false;
                    
                    openaiProcessor->next(dbd);
                    while(openaiT2tClient->is_processing())
                    {
                        mbase::sleep(2);
                    }
                    gContextDestructionSync.acquire();
                    delete openaiProcessor;
                    gContextDestructionSync.release();
                    delete openaiT2tClient;
                    return true;
                }
            );
            return;
        }
    }
    mbase::decode_behavior_description dbd;
    dbd.mTokenAtMost = 1;
    dbd.mHaltOnWrite = false;
    
    openaiProcessor->next(dbd);
    while(openaiT2tClient->is_processing())
    {
        mbase::sleep(2);
    }
    gContextDestructionSync.acquire();
    delete openaiProcessor;
    gContextDestructionSync.release();
    delete openaiT2tClient;
}

GENERIC completionHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    chatCompletionInternal(in_req, in_resp, false);
}

GENERIC chatCompletionHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    chatCompletionInternal(in_req, in_resp, true);
}

GENERIC httpServerThread()
{
    httplib::Server svr;

    svr.Get("/v1/models", modelListHandler);
    svr.Get("/v1/models/:model", modelRetrieveHandler);
    svr.Post("/completion", completionHandler);
    svr.Post("/completions", completionHandler);
    svr.Post("/v1/completions", completionHandler);
    svr.Post("/chat/completions", chatCompletionHandler);
    svr.Post("/v1/chat/completions", chatCompletionHandler);

    std::string host(gSampleParams.mHostname.c_str(), gSampleParams.mHostname.size());
    gIsServerListening = true;
    svr.listen(host, gSampleParams.mPort);
    gIsServerListening = false;
}

int main(int argc, char** argv)
{
    llama_backend_init();

    mbase::vector<InfDeviceDescription> deviceDesc = mbase::inf_query_devices();
    if(argc < 2)
    {
        print_usage();
        return 0;
    }

    for(I32 i = 0; i < argc; i++)
    {
        mbase::string argumentString = argv[i];
        if(argumentString == "-v" || argumentString == "--version")
        {
            printf("MBASE Openai server %s\n", MBASE_OPENAI_SERVER_VERSION);
            return 0;
        }

        else if(argumentString == "--api-key")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mApiKey);
        }

        else if(argumentString == "--hostname" || argumentString == "-h")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mHostname);
        }

        else if(argumentString == "--port" || argumentString == "-p")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mPort);
        }

        else if(argumentString == "--access-limit" || argumentString == "-al")
        {
            mbase::argument_get<U32>::value(i, argc, argv, gSampleParams.mAccessLimit);
        }

        else if(argumentString == "--model-path" || argumentString == "-m")
        {
            mbase::string tmpModel;
            mbase::argument_get<mbase::string>::value(i, argc, argv, tmpModel);
            if(mbase::find(gSampleParams.mModelFiles.begin(), gSampleParams.mModelFiles.end(), tmpModel) != gSampleParams.mModelFiles.end())
            {
                continue;
            }

            if(tmpModel.size())
            {
                gSampleParams.mModelFiles.push_back(tmpModel);
            }
        }

        else if(argumentString == "--thread-count" || argumentString == "-t")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mThreadCount);
        }

        else if(argumentString == "--context-length" || argumentString == "-c")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mContextLength);
        }

        else if(argumentString == "--batch-length" || argumentString == "-b")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mBatchLength);
        }

        else if(argumentString == "--gpu-layers" || argumentString == "-gl")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mGpuLayer);
        }

        else if(argumentString == "--batch-thread-count" || argumentString == "-bt")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mBatchThreadCount);
        }

        else if(argumentString == "--help")
        {
            print_usage();
            return 1;
        }
    }

    if(!gSampleParams.mModelFiles.size())
    {
        printf("ERROR: Model must be supplied\n");
        return 1;
    }

    if(!gSampleParams.mThreadCount)
    {
        printf("ERROR: Thread count can't be zero\n");
        return 1;
    }
    
    if(!gSampleParams.mBatchThreadCount)
    {
        printf("ERROR: Batch thread count can't be zero\n");
        return 1;
    }

    if(gSampleParams.mContextLength < 32)
    {
        printf("WARNING: Context length is too short %d, rounded up to: %d\n", gSampleParams.mContextLength, 32);
        gSampleParams.mContextLength = 32; 
    }

    if(gSampleParams.mBatchLength < 32)
    {
        printf("WARNING: Batch length is too short %d, rounded up to: %d\n", gSampleParams.mBatchLength, 32);
    }

    if(gSampleParams.mBatchLength > gSampleParams.mContextLength)
    {
        printf("WARNING: Batch length can't be greater than context length, clamp applied.\n");
    }

    for(mbase::vector<mbase::string>::iterator It = gSampleParams.mModelFiles.begin(); It != gSampleParams.mModelFiles.end(); ++It)
    {
        if(!mbase::is_file_valid(mbase::from_utf8(*It)))
        {
            printf("ERROR: Invalid file: %s\n", It->c_str());
            return 1;
        }
        OpenAiTextToTextHostedModel* newModel = new OpenAiTextToTextHostedModel;
        newModel->initialize_model_ex_sync(mbase::from_utf8(*It), 99999999, gSampleParams.mGpuLayer, true, true, deviceDesc);
        newModel->update();

        if(newModel->is_initialize_failed())
        {
            printf("ERROR: Failed to initialize model: %s\n", It->c_str());
            return 1;
        }

        gHostedModelArray.push_back(newModel);
    }

    printf("Active devices: \n");

    for(auto& activeDevice : deviceDesc)
    {
        printf("- %s\n", activeDevice.get_device_description().c_str());
    }

    mbase::thread serverThread(httpServerThread);
    serverThread.run();

    while(gIsServerListening)
    {
        gContextDestructionSync.acquire();
        for(mbase::vector<OpenAiTextToTextHostedModel*>::iterator It = gHostedModelArray.begin(); It != gHostedModelArray.end(); ++It)
        {
            OpenAiTextToTextHostedModel* hostedModel = *It;
            hostedModel->update();
        }
        gContextDestructionSync.release();
        mbase::sleep(2);

    }
    serverThread.join();

    printf("ERROR: Cant listen host: %s on port: %d\n", gSampleParams.mHostname.c_str(), gSampleParams.mPort);
    return 0;
}