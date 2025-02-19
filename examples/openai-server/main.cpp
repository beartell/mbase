#include "model.h"
#include "processor.h"
#include "client.h"
#include "openai_errors.h"
#include <mbase/argument_get_value.h>
#include <mbase/filesystem.h>
#include "global_state.h"

#define MBASE_OPENAI_SERVER_VERSION "v0.1.0"
void print_usage();
void print_usage()
{
    printf("========================================\n");
    printf("#Program name:      mbase_openai_server\n");
    printf("#Version:           %s\n", MBASE_OPENAI_SERVER_VERSION);
    printf("#Type:              Example, Utility\n");
    printf("#Further docs: \n");
    printf("***** DESCRIPTION *****\n");
    printf("An Openai API compatible HTTP/HTTPS server for serving LLMs.\n");
    printf("This program provides chat completion API For TextToText models and embeddings API For embedder models.\n\n");
    printf("The mbase_openai_server can host either single or multiple models and serve its clients simultaneusly\n");
    printf("which is specified by the key processor_count in the provided JSON description file.\n\n");
    printf("In order to properly use the mbase_openai_server program, you should supply a json file, describing\n");
    printf("the behavior of the openai server program.\n\n");
    printf("Along with the program description json, you can specify the hostname(default=127.0.0.1) to listen to\n");
    printf("and the port(default=8080). The specified hostname and port must be configured properly so that\n");
    printf("the application may listen.\n");
    printf("========================================\n\n");
    printf("Usage: mbase_openai_server *[option [value]]\n");
    printf("       mbase_openai_server --hostname \"127.0.0.1\" -jsdesc description.json\n");
    printf("       mbase_openai_server --hostname \"127.0.0.1\" --port 8080 -jsdesc description.json\n");
    printf("       mbase_openai_server --hostname \"127.0.0.1\" --port 8080 --ssl-pub public_key_file --ssl-key private_key_file -jsdesc description.json\n");
    printf("Options: \n\n");
    printf("--help                          Print usage.\n");
    printf("-v, --version                   Shows program version.\n");
    printf("--api-key <str>                 API key to be checked by the server.\n");
    printf("-h, --hostname <str>            Hostname to listen to (default=127.0.0.1).\n");
    printf("-p, --port <int>                Port to assign to (default=8080).\n");
    printf("--ssl-public <str>              SSL public file for HTTPS support.\n");
    printf("--ssl-key <str>                 SSL private key file for HTTPS support.\n");
    printf("-jsdesc <str>                   JSON description file for the openai server program.\n\n");
}

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
        mbase::string modelName = tmpModel->get_model_name();
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
        mbase::string tmpModelName = tmpModel->get_model_name();
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
        mbase::string tmpModelName = tmpModel->get_model_name();
        if(tmpModelName == requestedModel)
        {
            if(tmpModel->is_embedding_model())
            {
                // Trying to call chat completions api on embedder model
                mbase::sendOpenaiError(
                    in_req,
                    in_resp,
                    "The specified model is not available or invalid.",
                    "invalid_request_error",
                    "invalid_model"
                );
                return;
            }
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

void embeddingsHandler(const httplib::Request& in_req, httplib::Response& in_resp)
{
    /* /v1/embeddings */
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
    if(!jsonObject["model"].isString())
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
        mbase::string tmpModelName = tmpModel->get_model_name();
        if(tmpModelName == requestedModel)
        {
            if(!tmpModel->is_embedding_model())
            {
                // Trying to call embedder api on t2t model
                mbase::sendOpenaiError(
                    in_req,
                    in_resp,
                    "The specified model is not available or invalid.",
                    "invalid_request_error",
                    "invalid_model"
                );
                return;
            }
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

    mbase::OpenaiEmbedderProcessor* embedderProcesor = NULL;
    if(!activeModel->acquire_processor(embedderProcesor))
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

    // we acquired the processor
    mbase::vector<mbase::inf_text_token_vector> tokVec;
    if(jsonObject["input"].isString())
    {
        if(jsonObject["input"].getString().size())
        {
            mbase::inf_text_token_vector generatedTokens;
            if(embedderProcesor->tokenize_input(jsonObject["input"].getString(), generatedTokens) == mbase::OpenaiEmbedderProcessor::flags::INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT)
            {
                activeModel->release_processor(embedderProcesor);
                mbase::sendOpenaiError(
                    in_req,
                    in_resp,
                    "The server had an error while processing your request.",
                    "server_error",
                    "server_error"
                );
                return;
            }

            if(generatedTokens.size() > embedderProcesor->get_context_size())
            {
                activeModel->release_processor(embedderProcesor);
                mbase::sendOpenaiError(
                    in_req,
                    in_resp,
                    "The server had an error while processing your request.",
                    "server_error",
                    "server_error"
                );
                return;
            }

            if(generatedTokens.size())
            {
                tokVec.push_back(generatedTokens);
            }
        }
    }
    else if(jsonObject["input"].isArray())
    {
        for(mbase::Json& n : jsonObject["input"].getArray())
        {
            if(!n.isString())
            {
                activeModel->release_processor(embedderProcesor);
                mbase::sendOpenaiError(
                    in_req,
                    in_resp,
                    "Input field must be either string or array of strings.",
                    "invalid_request_error",
                    "invalid_message_structure"
                );
                return;
            }
            else
            {
                if(n.getString().size())
                {
                    mbase::inf_text_token_vector generatedTokens;
                    if(embedderProcesor->tokenize_input(n.getString(), generatedTokens) == mbase::OpenaiEmbedderProcessor::flags::INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT)
                    {
                        activeModel->release_processor(embedderProcesor);
                        mbase::sendOpenaiError(
                            in_req,
                            in_resp,
                            "The server had an error while processing your request.",
                            "server_error",
                            "server_error"
                        );
                        return;
                    }

                    if(generatedTokens.size() > embedderProcesor->get_context_size())
                    {
                        activeModel->release_processor(embedderProcesor);
                        mbase::sendOpenaiError(
                            in_req,
                            in_resp,
                            "The server had an error while processing your request.",
                            "server_error",
                            "server_error"
                        );
                        return;
                    }

                    if(generatedTokens.size())
                    {
                        tokVec.push_back(generatedTokens);
                    }
                }
            }
        }
    }
    else
    {
        // invalid message format
        activeModel->release_processor(embedderProcesor);
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            "Input field must be either string or array of strings.",
            "invalid_request_error",
            "invalid_message_structure"
        );
        return;
    }

    if(tokVec.size())
    {
        mbase::OpenaiEmbedderClient* embedderClient = static_cast<mbase::OpenaiEmbedderClient*>(embedderProcesor->get_assigned_client());
        embedderClient->set_embedder_input(embedderProcesor, in_resp, tokVec);
        while(embedderClient->is_processing())
        {
            mbase::sleep(2);
        }
        activeModel->release_processor(embedderProcesor);
    }
    else
    {
        activeModel->release_processor(embedderProcesor);
        mbase::sendOpenaiError(
            in_req,
            in_resp,
            "Unable to process input.",
            "invalid_request_error",
            "invalid_message_structure"
        );
        return;
    }
}

void server_start()
{
    httplib::Server* svr = NULL;
    if(gProgramData.keyFileSet)
    {
        svr = new httplib::SSLServer(gProgramData.publicKeyFile.c_str(), gProgramData.privateKeyFile.c_str());
    }

    else
    {
        svr = new httplib::Server;
    }

    svr->Get("/v1/models", modelListHandler);
    svr->Get("/v1/models/:model", modelRetrieveHandler);
    svr->Post("/chat/completions", chatCompletionHandler);
    svr->Post("/v1/chat/completions", chatCompletionHandler);
    svr->Post("/v1/embeddings", embeddingsHandler);

    std::string httpHost(gProgramData.hostName.c_str(), gProgramData.hostName.size());
    svr->listen(httpHost, gProgramData.listenPort);
    gProgramData.diagnostic.log(mbase::PcDiagnostics::flags::LOGTYPE_ERROR, mbase::PcDiagnostics::flags::LOGIMPORTANCE_FATAL, "Unable to listen on host: %s:%d", gProgramData.hostName.c_str(), gProgramData.listenPort);
    gProgramData.serverListening = false;
}

void apply_json_desc(const mbase::string& in_json_string)
{
    std::pair<mbase::Json::Status, mbase::Json> parsedJson = mbase::Json::parse(in_json_string);
    
    if(parsedJson.first != mbase::Json::Status::success)
    {
        printf("ERR: JSON parse failed\n");
        exit(0);
    }

    gProgramData.jsonDescription = mbase::Json::parse(in_json_string).second;
    mbase::Json& tmpJson = gProgramData.jsonDescription;

    if(!tmpJson.isArray())
    {
        // top level entry must be array
    }

    for(mbase::Json& modelObject : tmpJson.getArray())
    {
        // TODO, CHECK MODEL DUPLICATE
        mbase::wstring modelPath;
        mbase::string systemPromptString;
        uint32_t processorCount = 4;
        uint32_t threadCount = 8;
        uint32_t batchThreadCount = 8;
        uint32_t contextLength = 4096;
        uint32_t batchLength = 512;
        uint32_t gpuLayers = 999;
        if(modelObject["model_path"].isString())
        {
            modelPath = mbase::from_utf8(modelObject["model_path"].getString());
        }

        if(modelObject["processor_count"].isLong())
        {
            processorCount = modelObject["processor_count"].getLong();
        }

        if(modelObject["thread_count"].isLong())
        {
            threadCount = modelObject["thread_count"].getLong();
        }

        if(modelObject["batch_thread_count"].isLong())
        {
            batchThreadCount = modelObject["batch_thread_count"].getLong();
        }

        if(modelObject["context_length"].isLong())
        {
            contextLength = modelObject["context_length"].getLong();
        }

        if(modelObject["batch_length"].isLong())
        {
            batchLength = modelObject["batch_length"].getLong();
        }

        if(modelObject["gpu_layers"].isLong())
        {
            gpuLayers = modelObject["gpu_layers"].getLong();
        }

        if(!mbase::is_file_valid(modelPath))
        {
            printf("ERR: Cant open file: %s\n", modelObject["model_path"].getString().c_str());
            exit(1);
        }

        if(modelObject["fsys"].isString())
        {
            systemPromptString = mbase::read_file_as_string(mbase::from_utf8(modelObject["fsys"].getString()));
        }

        mbase::OpenaiModel* newModel = new mbase::OpenaiModel;
        if(newModel->initialize_model_sync(modelPath, 99999999, gpuLayers) != mbase::OpenaiModel::flags::INF_MODEL_INFO_UPDATE_REQUIRED)
        {
            wprintf(L"ERR: Unable to start initializing model: %s\n", modelObject["model_path"].getString().c_str());
            exit(1);
        }

        mbase::inf_sampling_set samplersList;

        if(modelObject["samplers"].isObject())
        {
            mbase::Json &samplerObject = modelObject["samplers"];
            mbase::InfSamplerDescription isd;
            if(samplerObject["top_k"].isLong())
            {
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TOP_K;
                isd.mTopK = samplerObject["top_k"].getLong();
                samplersList.insert(isd);
            }
            
            if(samplerObject["top_p"].isFloat())
            {
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TOP_P;
                isd.mTopP = samplerObject["top_p"].getFloat();
                samplersList.insert(isd);
            }

            if(samplerObject["min_p"].isFloat())
            {
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::MIN_P;
                isd.mMinP = samplerObject["min_p"].getFloat();
                samplersList.insert(isd);
            }

            if(samplerObject["temp"].isFloat())
            {
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TEMP;
                isd.mTemp = samplerObject["temp"].getFloat();
                samplersList.insert(isd);
            }

            if(samplerObject["mirostat_v2"].isObject())
            {
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::MIROSTAT_V2;
                if(samplerObject["mirostat_v2"]["tau"].isFloat() && samplerObject["mirostat_v2"]["eta"].isFloat())
                {
                    isd.mMiroV2.mTau = samplerObject["mirostat_v2"]["tau"].getFloat();
                    isd.mMiroV2.mEta = samplerObject["mirostat_v2"]["eta"].getFloat();
                    samplersList.insert(isd);
                }
            }

            if(samplerObject["repetition"].isObject())
            {
                isd.mSamplerType = mbase::InfSamplerDescription::SAMPLER::REPETITION;
                isd.mRepetition.mPenaltyLinefeed = true;
                isd.mRepetition.mPenaltyEos = false;

                if(samplerObject["repetition"]["penalty_n"].isLong() && samplerObject["repetition"]["penalty_repeat"].isFloat())
                {
                    isd.mRepetition.mPenaltyN = samplerObject["repetition"]["penalty_n"].getLong();
                    isd.mRepetition.mRepeatPenalty = samplerObject["repetition"]["penalty_repeat"].getFloat();
                    samplersList.insert(isd);
                }
            }
        }

        newModel->update();

        if(newModel->is_embedding_model())
        {
            newModel->initialize_embedder_processors(
                processorCount,
                contextLength,
                threadCount
            );
        }

        else
        {
            newModel->initialize_t2t_processors(
                processorCount,
                threadCount,
                batchThreadCount,
                contextLength,
                batchLength,
                samplersList
            );
        }

        while(!newModel->is_init_finished())
        {
            newModel->update();
            mbase::sleep(5);
        }

        gProgramData.diagnostic.print_logs();
        gProgramData.diagnostic.flush_logs();

        if(!newModel->is_embedding_model() && systemPromptString.size())
        {
            // means we should cache the system prompt
            gProgramData.diagnostic.log_stdout(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Caching the system prompt on processors. This may take a while...");
            for(mbase::OpenaiModel::context_processor_list::iterator It = newModel->get_registered_processors().begin(); It != newModel->get_registered_processors().end(); ++It)
            {
                mbase::OpenaiTextToTextProcessor* t2tProc = reinterpret_cast<mbase::OpenaiTextToTextProcessor*>(It->mSubject);
                mbase::inf_text_token_vector tokVec;
                if(t2tProc->tokenize_input(systemPromptString.c_str(), systemPromptString.size(), tokVec) == mbase::OpenaiTextToTextProcessor::flags::INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT)
                {
                    gProgramData.diagnostic.log_stdout(mbase::PcDiagnostics::flags::LOGTYPE_ERROR, mbase::PcDiagnostics::flags::LOGIMPORTANCE_FATAL, "System prompt tokenization failed. This is happens if the system prompt contains a character outside the model's vocabulary");
                }

                if(tokVec.size() > t2tProc->get_context_size())
                {
                    gProgramData.diagnostic.log_stdout(mbase::PcDiagnostics::flags::LOGTYPE_ERROR, mbase::PcDiagnostics::flags::LOGIMPORTANCE_FATAL, "Tokenized system prompt exceeds context size: given(%d), context(%d)", tokVec.size(), t2tProc->get_context_size());
                }

                t2tProc->execute_input_sync(tokVec, true);
                t2tProc->update();
            }
            gProgramData.diagnostic.log_stdout(mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "System prompt successfully cached.");
        }

        gProgramData.programModels.push_back(newModel);
    }
}

int main(int argc, char** argv)
{
    mbase::vector<mbase::InfDeviceDescription> deviceDesc = mbase::inf_query_devices();
    if(argc < 2)
    {
        print_usage();
        return 0;
    }

    mbase::string jsonFile;

    for(int i= 0; i < argc; i++)
    {
        mbase::string argumentString = argv[i];

        if(argumentString == "-v" || argumentString == "--version")
        {
            printf("MBASE Openai server %s\n", MBASE_OPENAI_SERVER_VERSION);
            return 0;
        }

        else if(argumentString == "-h" || argumentString == "--help")
        {
            print_usage();
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
            gProgramData.customPortSet = true;
            mbase::argument_get<int>::value(i, argc, argv, gProgramData.listenPort);
        }

        else if(argumentString == "--ssl-public")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gProgramData.publicKeyFile);
        }

        else if(argumentString == "--ssl-key")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gProgramData.privateKeyFile);
        }

        else if(argumentString == "-jsdesc")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, jsonFile);
        }
    }
    
    if(gProgramData.publicKeyFile.size() || gProgramData.privateKeyFile.size())
    {
        if(!mbase::is_file_valid(mbase::from_utf8(gProgramData.publicKeyFile)))
        {
            printf("ERR: Public key file invalid: %s\n", gProgramData.publicKeyFile.c_str());
        }

        if(!mbase::is_file_valid(mbase::from_utf8(gProgramData.privateKeyFile)))
        {
            printf("ERR: Private key file invalid: %s\n", gProgramData.privateKeyFile.c_str());
        }

        gProgramData.keyFileSet = true;
    }

    if(!jsonFile.size())
    {
        printf("ERR: Missing JSON description file\n");
        return 1;
    }

    mbase::wstring fileLocation = mbase::from_utf8(jsonFile);
    if(!mbase::is_file_valid(fileLocation))
    {
        printf("ERR: Json file can't be opened\n");
        return 1;
    }
    jsonFile = mbase::read_file_as_string(fileLocation);
    apply_json_desc(jsonFile);


    printf("** Hosted Model Information **\n");
    for(mbase::vector<mbase::OpenaiModel*>::iterator It = gProgramData.programModels.begin(); It != gProgramData.programModels.end(); ++It)
    {
        mbase::OpenaiModel* tmpModel = *It;
        mbase::string outName = tmpModel->get_model_name();
        printf("- Model name: %s\n", outName.c_str());
        printf("- Processor count: %d\n", tmpModel->get_registered_processors().size());
        if(tmpModel->is_embedding_model())
        {
            mbase::OpenaiEmbedderProcessor* embedderProc = static_cast<mbase::OpenaiEmbedderProcessor*>(tmpModel->get_registered_processors().front().mSubject);
            printf("- Embedder model: true\n");
            printf("- Context length: %d\n", embedderProc->get_context_size());
        }
        else
        {
            mbase::OpenaiTextToTextProcessor* t2tProc = static_cast<mbase::OpenaiTextToTextProcessor*>(tmpModel->get_registered_processors().front().mSubject);
            printf("- Embedder model: false\n");
            printf("- Context length: %d\n", t2tProc->get_context_size());
            printf("- Batch thread count: %d\n", t2tProc->get_batch_thread_count());
            printf("- Gen thread count: %d\n", t2tProc->get_thread_count());
        }
        printf("\n");
    }

    if(gProgramData.keyFileSet)
    {
        if(!gProgramData.customPortSet)
        {
            gProgramData.listenPort = 443;
        }
        printf("HTTPS ON\n");
        printf("HTTPS Listening on: %s:%d\n", gProgramData.hostName.c_str(), gProgramData.listenPort);
    }
    else
    {
        printf("HTTPS OFF\n");
        printf("HTTP Listening on: %s:%d\n", gProgramData.hostName.c_str(), gProgramData.listenPort);
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

    gProgramData.diagnostic.print_logs();    
}