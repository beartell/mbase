#include "model.h"
#include "processor.h"
#include "client.h"
#include "openai_errors.h"
#include <mbase/argument_get_value.h>
#include <mbase/filesystem.h>
#include <mbase/json/json.h>

#define MBASE_OPENAI_SERVER_VERSION "v1.2.0"
void print_usage();
void print_usage()
{
}

struct mbase_openai_program_data {
    mbase::vector<mbase::OpenaiModel*> programModels;
    mbase::string apiKey;
    mbase::string hostName = "127.0.0.1"; // --hostname || -h;
    int listenPort = 8080; // --port || -p
    mbase::Json jsonDescription; // -jsdesc
    bool serverListening = true;
};

mbase_openai_program_data gProgramData;

void modelListHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    /* /v1/models */
    mbase::string providedKey = "";
    if(!mbase::openaiAuthCheck(in_req, in_resp, gProgramData.apiKey, providedKey))
    {
        in_resp.status = 401;
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            mbase::string::from_format("Invalid API key provided: [%s]", providedKey.c_str()),
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }

    mbase::Json modelList;
    modelList.setArray();

    for(size_t i = 0; i < gProgramData.programModels.size(); i++)
    {
        mbase::OpenaiModel* tmpModel = gProgramData.programModels[i];
        mbase::string modelName;
        tmpModel->get_model_name(modelName);
        modelList[i]["id"] = modelName;
        modelList[i]["object"] = "model";
        modelList[i]["created"] = tmpModel->get_creation_date_in_epoch();
        modelList[i]["owned_by"] = "MBASE Inference Infrastructure";
    }

    mbase::Json responseJSon;
    responseJSon["object"] = "list";
    responseJSon["data"] = modelList;

    mbase::string resultantString = responseJSon.toString();
    in_resp.set_content(resultantString.c_str(), resultantString.size(), "application/json");
}

void modelRetrieveHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    /* /v1/models/:model */
    mbase::string providedKey = "";
    if(!mbase::openaiAuthCheck(in_req, in_resp, gProgramData.apiKey, providedKey))
    {
        in_resp.status = 401;
        mbase::sendOpenaiError(
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
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            "Required parameter is missing: model",
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }

    mbase::string requestedModel(in_req.path_params.at("model").c_str(), in_req.path_params.at("model").size()); 

    for(size_t i = 0; i < gProgramData.programModels.size(); i++)
    {
        mbase::OpenaiModel* tmpModel = gProgramData.programModels[i];
        mbase::string tmpModelName;
        tmpModel->get_model_name(tmpModelName);
        if(requestedModel == tmpModelName)
        {
            mbase::Json dataObject;
            dataObject["id"] = tmpModelName;
            dataObject["object"] = "model";
            dataObject["created"] = tmpModel->get_creation_date_in_epoch();
            dataObject["owned_by"] = "MBASE Infrastructure Project";
            
            mbase::string responseString = dataObject.toString();
            in_resp.set_content(responseString.c_str(), responseString.size(), "application/json");
            return;
        }
    }

    mbase::sendOpenaiError(
        in_req,
        in_resp,
        "The specified model is not available or invalid.",
        "invalid_request_error",
        "invalid_model"
    );
}

void chatCompletionHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    /* /v1/chat/completions */
    /* /chat/completions */

    printf("%s\n", in_req.body.c_str());
    mbase::string providedKey = "";
    if(!mbase::openaiAuthCheck(in_req, in_resp, gProgramData.apiKey, providedKey))
    {
        in_resp.status = 401;
        mbase::sendOpenaiError(
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
        mbase::sendOpenaiError(
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
        mbase::sendOpenaiError(
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
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            "Required parameter is missing: model",
            "invalid_request_error",
            "missing_parameter"
        );
        return;
    }
    
    mbase::string requestedModel = jsonObject["model"].getString();
    mbase::OpenaiModel* activeModel = NULL;

    for(mbase::vector<mbase::OpenaiModel*>::iterator It = gProgramData.programModels.begin(); It != gProgramData.programModels.end(); ++It)
    {
        mbase::OpenaiModel* tmpModel = *It;
        mbase::string tmpModelName;
        tmpModel->get_model_name(tmpModelName);
        if(tmpModelName == requestedModel)
        {
            activeModel = tmpModel;
            break;
        }
    }

    if(!activeModel)
    {
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            "The specified model is not available or invalid.",
            "invalid_request_error",
            "invalid_model"
        );
        return;
    }

    mbase::OpenaiTextToTextProcessor* t2tProcessor = NULL;
    if(!activeModel->acquire_processor(t2tProcessor))
    {
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            "The engine is currently overloaded. Please try again later.",
            "server_error",
            "engine_overloaded"
        );
        return;
    }

    // but we are good now
    mbase::vector<mbase::Json> messageObject = jsonObject["messages"].getArray();
    mbase::vector<mbase::context_line> totalMessageArray;
    for(mbase::vector<mbase::Json>::iterator It = messageObject.begin(); It != messageObject.end(); ++It)
    {
        mbase::Json& messageJson = * It;
        if(!messageJson["role"].isString() || !messageJson["content"].isString())
        {
            activeModel->release_processor(t2tProcessor);
            mbase::sendOpenaiError(
                in_req,
                in_resp,
                "Each message in 'messages' must have a 'role' and 'content' field.",
                "invalid_request_error",
                "invalid_message_structure"
            );
            return;
        }
        else
        {
            mbase::string messageRole = messageJson["role"].getString();
            mbase::string messageContent = messageJson["content"].getString();
            if(!messageContent.size())
            {
                activeModel->release_processor(t2tProcessor);
                mbase::sendOpenaiError(
                    in_req,
                    in_resp,
                    "Message's content field is empty.",
                    "invalid_request_error",
                    "invalid_message_structure"
                );
                return;
            }

            mbase::context_line newContextLine;

            if(messageRole == "system" || messageRole == "developer")
            {
                newContextLine.mRole = mbase::context_role::SYSTEM;
                newContextLine.mMessage = messageContent;
            }

            else if(messageRole == "assistant")
            {
                newContextLine.mRole = mbase::context_role::ASSISTANT;
                newContextLine.mMessage = messageContent;
            }

            else if(messageRole == "user")
            {
                newContextLine.mRole = mbase::context_role::USER;
                newContextLine.mMessage = messageContent;
            }
            
            else
            {
                newContextLine.mRole = mbase::context_role::NONE;
                newContextLine.mMessage = messageContent;
            }

            totalMessageArray.push_back(newContextLine);
        }
    }

    mbase::inf_text_token_vector tokenVector;
    
    if(t2tProcessor->tokenize_input(totalMessageArray.data(), totalMessageArray.size(), tokenVector) == mbase::OpenaiTextToTextProcessor::flags::INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT)
    {
        // tokenization should never fail
        // however if it happens, return unknown server error
        activeModel->release_processor(t2tProcessor);
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            "The server had an error while processing your request.",
            "server_error",
            "server_error"
        );
        return;
    }

    if(t2tProcessor->execute_input(tokenVector) != mbase::OpenaiTextToTextProcessor::flags::INF_PROC_SUCCESS)
    {
        activeModel->release_processor(t2tProcessor);
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            "The server had an error while processing your request.",
            "server_error",
            "server_error"
        );
        return;
    }

    long long genLimit = I32_MAX;
    
    if(jsonObject["max_tokens"].isLong())
    {
        genLimit = jsonObject["max_tokens"].getLong();
        if(genLimit <= 0)
        {
            activeModel->release_processor(t2tProcessor);
            mbase::sendOpenaiError(
                in_req,
                in_resp,
                "The server had an error while processing your request.",
                "server_error",
                "server_error"
            );
            return;
        }
    }

    if(jsonObject["stream"].isBool())
    {
        if(jsonObject["stream"].getBool())
        {
            in_resp.set_chunked_content_provider(
                "text/event-stream",
                [activeModel, t2tProcessor, &in_resp, genLimit](size_t offset, httplib::DataSink &sink) {
                    mbase::OpenaiTextToTextClient* t2tClient = static_cast<mbase::OpenaiTextToTextClient*>(t2tProcessor->get_assigned_client());
                    t2tClient->set_http_data_sink(&sink, in_resp, true, genLimit);
                    t2tClient->set_is_processing(true);

                    while(t2tClient->is_processing())
                    {
                        mbase::sleep(2);
                    }
                    
                    if(t2tProcessor->is_manual_caching())
                    {
                        t2tProcessor->clear_response();
                    }

                    activeModel->release_processor(t2tProcessor);

                    return true;
                }
            );
        }
        else
        {
            mbase::OpenaiTextToTextClient* t2tClient = static_cast<mbase::OpenaiTextToTextClient*>(t2tProcessor->get_assigned_client());
            t2tClient->set_http_data_sink(nullptr, in_resp, false, genLimit);
            t2tClient->set_is_processing(true);

            while(t2tClient->is_processing())
            {
                mbase::sleep(2);
            }

            activeModel->release_processor(t2tProcessor);
        }
    }

    else
    {
        mbase::OpenaiTextToTextClient* t2tClient = static_cast<mbase::OpenaiTextToTextClient*>(t2tProcessor->get_assigned_client());
        t2tClient->set_http_data_sink(nullptr, in_resp, false, genLimit);
        t2tClient->set_is_processing(true);

        while(t2tClient->is_processing())
        {
            mbase::sleep(2);
        }

        activeModel->release_processor(t2tProcessor);
    }
}

void server_start()
{
    httplib::Server svr;

    svr.Get("/v1/models", modelListHandler);
    svr.Get("/v1/models/:model", modelRetrieveHandler);
    svr.Post("/chat/completions", chatCompletionHandler);
    svr.Post("/v1/chat/completions", chatCompletionHandler);

    std::string httpHost(gProgramData.hostName.c_str(), gProgramData.hostName.size());
    svr.listen(httpHost, gProgramData.listenPort);
    gProgramData.serverListening = false;
}

void apply_json_desc(const mbase::string& in_json_string)
{
    gProgramData.jsonDescription = mbase::Json::parse(in_json_string).second;
    mbase::Json& tmpJson = gProgramData.jsonDescription;

    if(!tmpJson.isArray())
    {
        // top level entry must be array
    }

    for(mbase::Json& modelObject : tmpJson.getArray())
    {
        // TODO, CHECK MODEL DUPLICATE
        mbase::wstring modelPath = mbase::from_utf8(modelObject["model_path"].getString());
        uint32_t processorCount = modelObject["processor_count"].getLong(); 
        uint32_t threadCount = modelObject["thread_count"].getLong();
        uint32_t batchThreadCount = modelObject["batch_thread_count"].getLong();
        uint32_t contextLength = modelObject["context_length"].getLong();
        uint32_t batchLength = modelObject["batch_length"].getLong();
        uint32_t gpuLayers = modelObject["gpu_layers"].getLong();

        if(!mbase::is_file_valid(modelPath))
        {
            printf("ERR: Cant open file: %s\n", modelObject["model_path"].getString().c_str());
            exit(1);
        }

        mbase::OpenaiModel* newModel = new mbase::OpenaiModel;
        if(newModel->initialize_model_sync(modelPath, 99999999, gpuLayers) != mbase::OpenaiModel::flags::INF_MODEL_INFO_UPDATE_REQUIRED)
        {
            wprintf(L"ERR: Unable to start initializing model: %s\n", modelObject["model_path"].getString().c_str());
            exit(1);
        }

        mbase::inf_sampling_set samplerSet;
        if(modelObject["samplers"].isObject())
        {
            mbase::InfSamplerDescription isd;
            if(modelObject["samplers"]["top_k"].isLong())
            {
                isd.mTopK = modelObject["samplers"]["top_k"].getLong();
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TOP_K;
                samplerSet.insert(isd);
            }
            
            if(modelObject["samplers"]["top_p"].isFloat())
            {
                isd.mTopP = modelObject["samplers"]["top_p"].getFloat();
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TOP_P;
                samplerSet.insert(isd);
            }

            if(modelObject["samplers"]["min_p"].isFloat())
            {
                isd.mMinP = modelObject["samplers"]["min_p"].getFloat();
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::MIN_P;
                samplerSet.insert(isd);
            }

            if(modelObject["samplers"]["temperature"].isFloat())
            {
                isd.mTemp = modelObject["samplers"]["temperature"].getFloat();
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TEMP;
                samplerSet.insert(isd);
            }

            if(modelObject["samplers"]["repetition"].isObject())
            {
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::REPETITION;
                if(modelObject["samplers"]["repetition"]["penalty_n"].isLong() && modelObject["samplers"]["repetition"]["penalty_repeat"].isFloat())
                {
                    isd.mRepetition.mPenaltyN = modelObject["samplers"]["repetition"]["penalty_n"].getLong();
                    isd.mRepetition.mRepeatPenalty = modelObject["samplers"]["repetition"]["penalty_repeat"].getFloat();
                }
                samplerSet.insert(isd);
            }

            if(modelObject["samplers"]["mirostat_v2"].isObject())
            {
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::MIROSTAT_V2;
                if(modelObject["samplers"]["mirostat_v2"]["tau"].isFloat() && modelObject["samplers"]["mirostat_v2"]["eta"].isFloat())
                {
                    isd.mMiroV2.mTau = modelObject["samplers"]["mirostat_v2"]["tau"].getFloat();
                    isd.mMiroV2.mEta = modelObject["samplers"]["mirostat_v2"]["eta"].getFloat();
                }
                samplerSet.insert(isd);
            }
        }

        newModel->update();
        newModel->initialize_t2t_processors(
            processorCount,
            threadCount,
            batchThreadCount,
            contextLength,
            batchLength,
            samplerSet
        );

        gProgramData.programModels.push_back(newModel);
    }
}

int main(int argc, char** argv)
{
    mbase::vector<mbase::InfDeviceDescription> deviceDesc = mbase::inf_query_devices();

    if(argc < 2)
    {
        print_usage();
        return 1;
    }

    for(int i= 0; i < argc; i++)
    {
        mbase::string argumentString = argv[i];
        if(argumentString == "-v" || argumentString == "--version")
        {
            printf("MBASE Openai server %s\n", MBASE_OPENAI_SERVER_VERSION);
            return 0;
        }

        else if(argumentString == "--api-key")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gProgramData.apiKey);
        }

        else if(argumentString == "--hostname" || argumentString == "-h")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gProgramData.hostName);
        }

        else if(argumentString == "--port" || argumentString == "-p")
        {
            mbase::argument_get<int>::value(i, argc, argv, gProgramData.listenPort);
        }

        else if(argumentString == "-jsdesc")
        {
            mbase::string jsonFile;
            mbase::argument_get<mbase::string>::value(i, argc, argv, jsonFile);
            mbase::wstring fileLocation = mbase::from_utf8(jsonFile);

            if(!mbase::is_file_valid(fileLocation))
            {
                printf("Json file can't be opened\n");
                return 0;
            }
            jsonFile = mbase::read_file_as_string(fileLocation);
            apply_json_desc(jsonFile);
        }
    }

    mbase::thread serverThread(server_start);
    serverThread.run();

    while(gProgramData.serverListening)
    {
        mbase::sleep(2);
        for(auto& n : gProgramData.programModels)
        {
            n->update();
        }
    }
}