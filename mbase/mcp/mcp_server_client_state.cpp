#include <mbase/mcp/mcp_server_client_state.h>
#include <mbase/mcp/mcp_server_features.h>
#include <mbase/mcp/mcp_server_base.h>
#include <mbase/mcp/mcp_packet_parsing.h>

MBASE_BEGIN

McpServerClient::McpServerClient(
    McpServerBase* in_server_instance
)
{
    mServerInstance = in_server_instance;
}

McpServerClient::~McpServerClient()
{
}

mcp_log_levels McpServerClient::get_log_level() const noexcept
{
    return mLogLevel;
}

McpServerBase* McpServerClient::get_server_instance() noexcept
{
    return mServerInstance;
}

bool McpServerClient::is_subscribed(const mbase::string& in_uri) const noexcept
{
    mbase::unordered_map<mbase::string, bool>::const_iterator cIt = mSubscriptionMap.find(in_uri);
    if(cIt == mSubscriptionMap.end())
    {
        return false;
    }
    return true;
}

bool McpServerClient::is_initializing() const noexcept
{
    return mIsInitializing;
}

bool McpServerClient::is_initialized() const noexcept
{
    return mIsClientInitialized;
}

bool McpServerClient::has_roots() const noexcept
{
    if(mServerInstance->get_transport_method() == mbase::mcp_transport_method::HTTP_STREAMBLE) {return false;}
    return mHasRoots;
}

bool McpServerClient::has_sampling() const noexcept
{
    if(mServerInstance->get_transport_method() == mbase::mcp_transport_method::HTTP_STREAMBLE) {return false;}
    return mHasSampling;
}

GENERIC McpServerClient::on_client_init_request_t(const mbase::Json& in_msgid, mbase::Json& in_params)
{
    if(!in_params["protocolVersion"].isString() 
    || !in_params["clientInfo"].isObject())
    {
        // do not even respond with error
        on_empty_processed_t();
        return;
    }

    if(!in_params["clientInfo"]["name"].isString() || !in_params["clientInfo"]["version"].isString())
    {
        // do not respond
        on_empty_processed_t();
        return;
    }

    const mbase::string& protocolVersion = in_params["protocolVersion"].getString();

    if(protocolVersion != "2024-11-05" && protocolVersion != "2025-03-26")
    {
        mbase::Json errorData;
        errorData["supported"].setArray();
        errorData["requested"] = protocolVersion;
        errorData["supported"][0] = "2024-11-05";
        errorData["supported"][1] = "2025-03-26";

        this->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, "Unsupported protocol version", errorData));
        return;
    }

    if(!in_params["capabilities"].isObject())
    {
        this->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, "Failure to negotiate required capabilities"));
        return;
    }

    for(auto& tmpCapability : in_params["capabilities"].getObject())
    {
        if(tmpCapability.first == "roots")
        {
            mHasRoots = true;
        }

        else if(tmpCapability.first == "sampling")
        {
            mHasSampling = true;
        }
    }

    mClientName = in_params["clientInfo"]["name"].getString();
    mClientVersion = in_params["clientInfo"]["version"].getString();
    mClientProtocolVersion = protocolVersion;

    mbase::McpServerBase* serverInstance = this->get_server_instance();
    mbase::Json initPacket;
    initPacket["protocolVersion"] = MBASE_MCP_DEFAULT_VERSION;
    
    mbase::Json& serverCapabilities = initPacket["capabilities"];
    serverCapabilities["logging"].setObject();
    serverCapabilities["prompts"]["listChanged"] = true;
    serverCapabilities["resources"]["subscribe"] = true;
    serverCapabilities["resources"]["listChanged"] = true;
    serverCapabilities["tools"]["listChanged"] = true;

    initPacket["serverInfo"]["name"] = serverInstance->get_server_name();
    initPacket["serverInfo"]["version"] = serverInstance->get_server_version();
    mIsInitializing = true;
    serverInstance->register_client(this);
    this->send_mcp_payload(mbase::mcp_generate_response(in_msgid, initPacket));
}

GENERIC McpServerClient::on_empty_processed_t()
{

}

GENERIC McpServerClient::list_roots(mcp_st_list_roots_cb in_cb)
{
    if(!this->has_roots() || !this->is_initialized())
    {
        mbase::vector<mbase::McpRootsResult> rootsResult;
        in_cb(MBASE_MCP_METHOD_NOT_FOUND, get_server_instance(), rootsResult);
        return;
    }
    mbase::McpStClientRequestObject requestObject;
    requestObject.mMessageId = mbase::string::generate_uuid();
    requestObject.mStCallback = in_cb;
    mRequestObject[requestObject.mMessageId] = requestObject;
    this->send_mcp_payload(mbase::mcp_create_request(requestObject.mMessageId, "roots/list"));
}

GENERIC McpServerClient::request_sampling(const McpSamplingRequest& in_params, mcp_st_sampling_cb in_cb)
{
    if(!this->has_sampling() || !this->is_initialized())
    {
        mbase::McpSamplingResult samplingResult;
        in_cb(MBASE_MCP_METHOD_NOT_FOUND, get_server_instance(), samplingResult);
        return;
    }

    mbase::Json samplingParams;
    samplingParams["systemPrompt"] = in_params.mSystemPrompt;
    samplingParams["maxTokens"] = in_params.mMaxTokens;
    samplingParams["messages"].setArray();
    samplingParams["modelPreferences"]["hints"].setArray();
    samplingParams["modelPreferences"]["intelligencePriority"] = in_params.mModelPreferences.mIntelligencePriority;
    samplingParams["modelPreferences"]["speedPriority"] = in_params.mModelPreferences.mSpeedPriority;

    mbase::vector<mbase::Json>& messagesArray = samplingParams["messages"].getArray();
    mbase::vector<mbase::Json>& hintsArray = samplingParams["modelPreferences"]["hints"].getArray();
    
    for(const mbase::McpSamplingMessage& tmpMessage : in_params.mMessages)
    {
        mbase::Json messageContent;
        messageContent["role"] = tmpMessage.mRole;
        if(tmpMessage.mContentType == mbase::mcp_sampling_content_type::TEXT)
        {
            messageContent["content"]["type"] = "text";
            messageContent["content"]["text"] = tmpMessage.mTextContent;
        }
        else if(tmpMessage.mContentType == mbase::mcp_sampling_content_type::AUDIO)
        {
            messageContent["content"]["type"] = "audio";
            messageContent["content"]["data"] = tmpMessage.mBase64Data;
            messageContent["content"]["mimeType"] = tmpMessage.mMimeType;
        }
        else if(tmpMessage.mContentType == mbase::mcp_sampling_content_type::IMAGE)
        {
            messageContent["content"]["type"] = "image";
            messageContent["content"]["data"] = tmpMessage.mBase64Data;
            messageContent["content"]["mimeType"] = tmpMessage.mMimeType;
        }
        messagesArray.push_back(messageContent);
    }

    for(const mbase::string& modelNamePreference : in_params.mModelPreferences.mModelHintsList)
    {
        mbase::Json modelHintName;
        modelHintName["name"] = modelNamePreference;
        hintsArray.push_back(modelHintName);
    }

    mbase::McpStClientRequestObject requestObject;
    requestObject.mMessageId = mbase::string::generate_uuid();
    requestObject.mStCallback = in_cb;
    mRequestObject[requestObject.mMessageId] = requestObject;
    this->send_mcp_payload(mbase::mcp_create_request(requestObject.mMessageId, "sampling/createMessage", samplingParams));
}

GENERIC McpServerClient::subscribe(const mbase::string& in_uri)
{
    mSubscriptionMap[in_uri] = true;
}

GENERIC McpServerClient::unsubscribe(const mbase::string& in_uri)
{
    if(is_subscribed(in_uri))
    {
        mSubscriptionMap.erase(in_uri);
    }
}

GENERIC McpServerClient::set_progress(const I32& in_progress, const mbase::Json& in_token, const mbase::string& in_message)
{
    if(this->get_server_instance()->get_transport_method() == mbase::mcp_transport_method::HTTP_STREAMBLE){ return; }
    if(!is_initialized()){ return; }
    if(!in_token.isString() && !in_token.isLong()){ return; }
    mbase::Json progressParams;
    progressParams["progressToken"] = in_token;
    progressParams["progress"] = in_progress;
    progressParams["total"] = 100;
    if(in_message.size() && mClientProtocolVersion == "2025-03-26")
    {
        progressParams["message"] = in_message;
    }

    this->send_mcp_payload(mbase::mcp_generate_notification("notifications/progress", progressParams));
}

GENERIC McpServerClient::set_log_level(mcp_log_levels in_log_level)
{
    if(!mIsClientInitialized){ return; }
    mLogLevel = in_log_level;
}

GENERIC McpServerClient::send_log(const McpNotificationLogMessage& in_log)
{
    if(this->get_server_instance()->get_transport_method() == mbase::mcp_transport_method::HTTP_STREAMBLE){ return; }
    if(!mIsClientInitialized){ return; }
    mbase::Json logMessageObject;
    switch (in_log.mLevel)
    {
    case mbase::mcp_log_levels::L_DEBUG:
        logMessageObject["level"] = "debug";
        break;
    case mbase::mcp_log_levels::L_INFO:
        logMessageObject["level"] = "info";
        break;
    case mbase::mcp_log_levels::L_NOTICE:
        logMessageObject["level"] = "notice";
        break;
    case mbase::mcp_log_levels::L_WARNING:
        logMessageObject["level"] = "warning";
        break;
    case mbase::mcp_log_levels::L_ERROR:
        logMessageObject["level"] = "error";
        break;
    case mbase::mcp_log_levels::L_CRITICAL:
        logMessageObject["level"] = "critical";
        break;
    case mbase::mcp_log_levels::L_ALERT:
        logMessageObject["level"] = "alert";
        break;
    case mbase::mcp_log_levels::L_EMERGENCY:
        logMessageObject["level"] = "emergency";
        break;
    default:
        logMessageObject["level"] = "info"; // default
        break;
    }
    logMessageObject["logger"] = in_log.mLogger;
    logMessageObject["data"]["error"] = in_log.mError;
    logMessageObject["data"]["details"] = in_log.mDetails;
    this->send_mcp_payload(mbase::mcp_generate_notification("notifications/message", logMessageObject));
}

GENERIC McpServerClient::read_mcp_payload(const mbase::string& in_payload)
{
    mbase::Json mcpPacketJson;
    if(mbase::validate_mcp_request_packet(in_payload, mcpPacketJson))
    {
        // it is a request packet
        if(mcpPacketJson.isArray())
        {
            for(mbase::Json& batchJson : mcpPacketJson.getArray())
            {
                // process batches
                process_mcp_message(batchJson);
            }
            return;
        }
        process_mcp_message(mcpPacketJson);
    }
    else
    {
        if(mbase::validate_mcp_response(in_payload, mcpPacketJson))
        {
            // it is a response packet
            if(mcpPacketJson.isArray())
            {
                for(mbase::Json& batchJson : mcpPacketJson.getArray())
                {
                    // process batches
                    process_response_message(batchJson);
                    continue;
                }
                return;
            }
            process_response_message(mcpPacketJson);
        }
        else
        {
            on_empty_processed_t();
        }
    }
}

GENERIC McpServerClient::process_mcp_message(mbase::Json& in_message)
{
    if(!in_message["id"].isString() && !in_message["id"].isLong())
    {
        // means it is a notification request
        process_notification_message(in_message);
        return;
    }
    // means it is a method request
    process_request_message(in_message);
}

GENERIC McpServerClient::process_notification_message(mbase::Json& in_message)
{
    // Client sent notifications:
    // - InitializedNotification = notifications/initialized
    // - RootsListChangedNotification = notifications/roots/list_changed
    // - CancellationNotification = notifications/cancelled
    const mbase::string& notificationMethod = in_message["method"].getString();
    McpServerBase* serverInstance = this->get_server_instance();
    serverInstance->on_client_notification_t(this, notificationMethod, in_message["params"]);
    if(notificationMethod == "notifications/initialized")
    {
        if(mIsClientInitialized)
        {
            // client tries to reinitialize for some reason?
            on_empty_processed_t();
            return;
        }
        mIsClientInitialized = true;
        mIsInitializing = false;
        serverInstance->on_client_init(this);
        serverInstance->default_client_init(this);
        return;
    }

    if(mIsClientInitialized)
    {
        if(notificationMethod == "notifications/roots/list_changed")
        {
            serverInstance->on_roots_list_changed(this);
            serverInstance->default_client_root_list_changed(this);
        }

        else if(notificationMethod == "notifications/cancelled")
        {
            if(in_message["params"]["requestId"].isString() || in_message["params"]["requestId"].isLong())
            {
                if(in_message["params"]["reason"].isString())
                {
                    serverInstance->on_cancellation_t(this, in_message["params"]["requestId"], in_message["params"]["reason"].getString());
                    serverInstance->default_cancellation_t(this, in_message["params"]["requestId"], in_message["params"]["reason"].getString());
                }
                else
                {
                    serverInstance->on_cancellation_t(this, in_message["params"]["requestId"]);
                    serverInstance->default_cancellation_t(this, in_message["params"]["requestId"]);
                }
                return;
            }
            
            on_empty_processed_t();
        }

        else
        {
            on_empty_processed_t();
        }
    }
}

GENERIC McpServerClient::process_request_message(mbase::Json& in_message)
{
    // Client sent supported requests:
    // - InitializeRequest = initialize
    // - PingRequest = ping
    // - ListResourcesRequest = resources/list
    // - ListPromptsRequest = prompts/list
    // - ListToolsRequest = tools/list
    // - SubscribeRequest = resources/subscribe
    // - UnsubscribeRequest = resources/unsubscribe
    // - CompletionRequest = completion/complete
    // - SetLevelRequest = logging/setLevel
    // - ReadResourceRequest = resources/read
    // - GetPromptRequest = prompts/get
    // - CallToolRequest = tools/call
    const mbase::string& requestedMethod = in_message["method"].getString();
    mbase::Json messageId = in_message["id"];
    mbase::Json& messageParams = in_message["params"];
    McpServerBase* serverInstance = this->get_server_instance();
    if(serverInstance->on_client_request_t(this, messageId, requestedMethod, messageParams))
    {
        return;
    }
    if(mIsInitializing)
    {
        on_empty_processed_t();
        return;
    }

    if(!mIsClientInitialized && requestedMethod != "initialize")
    {
        // attempting to call a method without being initialized
        on_empty_processed_t();
        return;
    }

    if(requestedMethod == "initialize")
    {
        if(mIsClientInitialized)
        {
            mbase::Json initPacket;
            initPacket["protocolVersion"] = MBASE_MCP_DEFAULT_VERSION;
            mbase::Json& serverCapabilities = initPacket["capabilities"];
            serverCapabilities["logging"].setObject();
            serverCapabilities["prompts"]["listChanged"] = true;
            serverCapabilities["resources"]["subscribe"] = true;
            serverCapabilities["resources"]["listChanged"] = true;
            serverCapabilities["tools"]["listChanged"] = true;
            initPacket["serverInfo"]["name"] = serverInstance->get_server_name();
            initPacket["serverInfo"]["version"] = serverInstance->get_server_version();
            this->send_mcp_payload(mbase::mcp_generate_response(messageId, initPacket));
            return;
        }
        this->on_client_init_request_t(messageId, messageParams);
    }
    else if(requestedMethod == "ping")
    {
        serverInstance->on_ping_t(this, messageId);
        serverInstance->default_ping_t(this, messageId);
    }
    else if(requestedMethod == "resources/list")
    {
        if(messageParams["cursor"].isString())
        {
            serverInstance->on_list_resource_t(this, messageId, messageParams["cursor"].getString());
            serverInstance->default_list_resource_t(this, messageId, messageParams["cursor"].getString());
        }
        else
        {
            serverInstance->on_list_resource_t(this, messageId);
            serverInstance->default_list_resource_t(this, messageId);
        }
    }
    else if(requestedMethod == "prompts/list")
    {
        if(messageParams["cursor"].isString())
        {
            serverInstance->on_list_prompt_t(this, messageId, messageParams["cursor"].getString());
            serverInstance->default_list_prompt_t(this, messageId, messageParams["cursor"].getString());
        }
        else
        {
            serverInstance->on_list_prompt_t(this, messageId);
            serverInstance->default_list_prompt_t(this, messageId);
        }
    }
    else if(requestedMethod == "tools/list")
    {
        if(messageParams["cursor"].isString())
        {
            serverInstance->on_list_tool_t(this, messageId, messageParams["cursor"].getString());
            serverInstance->default_list_tool_t(this, messageId, messageParams["cursor"].getString());
        }
        else
        {
            serverInstance->on_list_tool_t(this, messageId);
            serverInstance->default_list_tool_t(this, messageId);
        }
    }
    else if(requestedMethod == "resources/subscribe")
    {
        if(!messageParams["uri"].isString())
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid params"));
            return;
        }
        this->subscribe(messageParams["uri"].getString());
        serverInstance->on_resource_subscribe(this, messageId, messageParams["uri"].getString());
        serverInstance->default_resource_subscribe_t(this, messageId, messageParams["uri"].getString());
    }
    else if(requestedMethod == "resources/unsubscribe")
    {
        if(!messageParams["uri"].isString())
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid params"));
            return;
        }

        this->unsubscribe(messageParams["uri"].getString());
        serverInstance->on_resource_unsubscribe(this, messageId, messageParams["uri"].getString());
        serverInstance->default_resource_unsubscribe_t(this, messageId, messageParams["uri"].getString());
    }
    else if(requestedMethod == "completion/complete")
    {
        if(!messageParams["ref"].isObject() || !messageParams["argument"].isObject())
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid params"));
            return;
        }

        mbase::Json& refObject = messageParams["ref"];
        mbase::Json& argumentObject = messageParams["argument"];
        if(
            !refObject["type"].isString() 
            || !refObject["name"].isString()
            || !argumentObject["name"].isString()
            || !argumentObject["value"].isString()
        )
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid params"));
            return;
        }

        if(refObject["type"].getString() != "ref/prompt")
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid params"));
            return;
        }

        serverInstance->on_prompt_compilation_t(this, messageId, refObject["name"].getString(), argumentObject["name"].getString(), argumentObject["value"].getString());
        serverInstance->default_prompt_compilation_t(this, messageId, refObject["name"].getString(), argumentObject["name"].getString(), argumentObject["value"].getString());
    }
    else if(requestedMethod == "logging/setLevel")
    {
        if(!messageParams["level"].isString())
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid params"));
            return;
        }
        
        const mbase::string& logLevelString = messageParams["level"].getString();
        if(logLevelString == "debug")
        {
            serverInstance->on_logging_set_t(this, messageId, mbase::mcp_log_levels::L_DEBUG);
            serverInstance->default_logging_set_t(this, messageId, mbase::mcp_log_levels::L_DEBUG);
        }
        else if(logLevelString == "info")
        {
            serverInstance->on_logging_set_t(this, messageId, mbase::mcp_log_levels::L_INFO);
            serverInstance->default_logging_set_t(this, messageId, mbase::mcp_log_levels::L_INFO);
        }
        else if(logLevelString == "notice")
        {
            serverInstance->on_logging_set_t(this, messageId, mbase::mcp_log_levels::L_NOTICE);
            serverInstance->default_logging_set_t(this, messageId, mbase::mcp_log_levels::L_NOTICE);
        }
        else if(logLevelString == "warning")
        {
            serverInstance->on_logging_set_t(this, messageId, mbase::mcp_log_levels::L_WARNING);
            serverInstance->default_logging_set_t(this, messageId, mbase::mcp_log_levels::L_WARNING);
        }
        else if(logLevelString == "error")
        {
            serverInstance->on_logging_set_t(this, messageId, mbase::mcp_log_levels::L_ERROR);
            serverInstance->default_logging_set_t(this, messageId, mbase::mcp_log_levels::L_ERROR);
        }
        else if(logLevelString == "critical")
        {
            serverInstance->on_logging_set_t(this, messageId, mbase::mcp_log_levels::L_CRITICAL);
            serverInstance->default_logging_set_t(this, messageId, mbase::mcp_log_levels::L_CRITICAL);
        }
        else if(logLevelString == "alert")
        {
            serverInstance->on_logging_set_t(this, messageId, mbase::mcp_log_levels::L_ALERT);
            serverInstance->default_logging_set_t(this, messageId, mbase::mcp_log_levels::L_ALERT);
        }
        else if(logLevelString == "emergency")
        {
            serverInstance->on_logging_set_t(this, messageId, mbase::mcp_log_levels::L_EMERGENCY);
            serverInstance->default_logging_set_t(this, messageId, mbase::mcp_log_levels::L_EMERGENCY);
        }
        else
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid log level"));
            return;
        }
    }
    else if(requestedMethod == "resources/read")
    {
        mbase::Json errorData;
        if(!messageParams["uri"].isString())
        {
            errorData["uri"] = "";
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_RESOURCE_NOT_FOUND, "Resource not found", errorData));
            return;
        }
        errorData["uri"] = messageParams["uri"].getString();
        mbase::McpResourceFeature* currentFeature = serverInstance->get_resource_feature(messageParams["uri"].getString());
        if(!currentFeature)
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_RESOURCE_NOT_FOUND, "Resource not found", errorData));
            return;
        }
        serverInstance->on_resource_call_t(this, messageId, messageParams["_meta"]["progressToken"], currentFeature);
        serverInstance->default_resource_call_t(this, messageId, messageParams["_meta"]["progressToken"], currentFeature);
    }
    else if(requestedMethod == "prompts/get")
    {        
        if(!messageParams["name"].isString())
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid params"));
            return;
        }

        mbase::McpPromptFeature* currentFeature = serverInstance->get_prompt_feature(messageParams["name"].getString());
        if(!currentFeature)
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_METHOD_NOT_FOUND, "Method not found"));
            return;
        }

        mbase::McpMessageMap messageArguments;
        if(messageParams["arguments"].isObject())
        {
            const std::map<mbase::string, mbase::Json>& argumentsObject = messageParams["arguments"].getObject();
            for(auto& currentArgument : argumentsObject)
            {
                if(currentArgument.second.isLong())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getLong());
                }
                else if(currentArgument.second.isFloat())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getFloat());
                }
                else if(currentArgument.second.isDouble())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getDouble());
                }
                else if(currentArgument.second.isBool())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getBool());
                }
                else if(currentArgument.second.isString())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getString());
                }
                else if(currentArgument.second.isArray())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getArray());
                }
                else if(currentArgument.second.isObject())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getObject());
                }
            }
        }
        serverInstance->on_prompt_call_t(this, messageId, messageParams["_meta"]["progressToken"], currentFeature, messageArguments);
        serverInstance->default_prompt_call_t(this, messageId, messageParams["_meta"]["progressToken"], currentFeature, messageArguments);
    }
    else if(requestedMethod == "tools/call")
    {
        if(!messageParams["name"].isString())
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_INVALID_PARAMS, "Invalid params"));
            return;
        }

        mbase::McpToolFeature* currentFeature = serverInstance->get_tool_feature(messageParams["name"].getString());
        if(!currentFeature)
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_METHOD_NOT_FOUND, "Method not found"));
            return;
        }

        mbase::McpMessageMap messageArguments;
        if(messageParams["arguments"].isObject())
        {
            const std::map<mbase::string, mbase::Json>& argumentsObject = messageParams["arguments"].getObject();
            for(auto& currentArgument : argumentsObject)
            {
                if(currentArgument.second.isLong())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getLong());
                }
                else if(currentArgument.second.isFloat())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getFloat());
                }
                else if(currentArgument.second.isDouble())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getDouble());
                }
                else if(currentArgument.second.isBool())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getBool());
                }
                else if(currentArgument.second.isString())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getString());
                }
                else if(currentArgument.second.isArray())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getArray());
                }
                else if(currentArgument.second.isObject())
                {
                    messageArguments[currentArgument.first] = mbase::McpMessageArgument(currentArgument.second.getObject());
                }
            }
        }

        serverInstance->on_tool_call_t(this, messageId, messageParams["_meta"]["progressToken"], currentFeature, messageArguments);
        serverInstance->default_tool_call_t(this, messageId, messageParams["_meta"]["progressToken"], currentFeature, messageArguments);
    }
    else
    {
        this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_METHOD_NOT_FOUND, "Method not found"));
    }
}

GENERIC McpServerClient::process_response_message(mbase::Json& in_message)
{
    if(in_message["id"].isString())
    {
        // server to client requests are always string
        const mbase::string& messageId = in_message["id"].getString();
        mbase::unordered_map<mbase::string, mbase::McpStClientRequestObject>::iterator It = mRequestObject.find(messageId);
        get_server_instance()->on_client_response_t(this, messageId, in_message["result"]);
        if(It == mRequestObject.end()){ return; }

        if(std::holds_alternative<mcp_st_list_roots_cb>(It->second.mStCallback))
        {
            mbase::McpStRootsResultObject rootsResultObject;
            rootsResultObject.rootsResultCallback = It->second.mStCallback;
            if(in_message["result"]["roots"].isArray())
            {
                for(mbase::Json& rootObject : in_message["result"]["roots"].getArray())
                {
                    mbase::McpRootsResult rootsResult;
                    if(rootObject["uri"].isString() && rootObject["name"].isString())
                    {
                        rootsResult.mUri = rootObject["uri"].getString();
                        rootsResult.mName = rootObject["name"].getString();
                        rootsResultObject.rootsResult.push_back(std::move(rootsResult));
                    }
                }
            }
            mRequestObject.erase(It);
            mRootsResultObjectsSync.acquire();
            mRootsResultObjects.push_back(std::move(rootsResultObject));
            mRootsResultObjectsSync.release();
        }
        else if(std::holds_alternative<mcp_st_sampling_cb>(It->second.mStCallback))
        {
            mbase::McpStSamplingResultObject samplingResultObject;
            samplingResultObject.samplingResultCallback = It->second.mStCallback;
            mbase::McpSamplingResult samplingResult;
            if(in_message["result"]["role"].isString())
            {
                samplingResult.mRole = in_message["result"]["role"].getString();
            }

            if(in_message["result"]["model"].isString())
            {
                samplingResult.mModel = in_message["result"]["model"].getString();
            }

            if(in_message["result"]["stopReason"].isString())
            {
                samplingResult.mStopReason = in_message["result"]["stopReason"].getString();
            }

            if(in_message["result"]["content"]["type"].isString())
            {
                const mbase::string& contentType = in_message["result"]["content"]["type"].getString();
                if(contentType == "text")
                {
                    samplingResult.mContentType = mbase::mcp_sampling_content_type::TEXT;
                    if(in_message["result"]["content"]["text"].isString())
                    {
                        samplingResult.mTextContent = in_message["result"]["content"]["text"].getString();
                    }
                }

                else if(contentType == "audio")
                {
                    samplingResult.mContentType = mbase::mcp_sampling_content_type::AUDIO;
                    if(in_message["result"]["content"]["data"].isString())
                    {
                        samplingResult.mBase64Content = in_message["result"]["content"]["data"].getString();
                    }

                    if(in_message["result"]["content"]["mimeType"].isString())
                    {
                        samplingResult.mMimeType = in_message["result"]["content"]["mimeType"].getString();
                    }
                }

                else if(contentType == "image")
                {
                    samplingResult.mContentType = mbase::mcp_sampling_content_type::IMAGE;
                    if(in_message["result"]["content"]["data"].isString())
                    {
                        samplingResult.mBase64Content = in_message["result"]["content"]["data"].getString();
                    }

                    if(in_message["result"]["content"]["mimeType"].isString())
                    {
                        samplingResult.mMimeType = in_message["result"]["content"]["mimeType"].getString();
                    }
                }
            }
            mRequestObject.erase(It);
            samplingResultObject.samplingResult = samplingResult;
            mSamplingResultObjectsSync.acquire();
            mSamplingResultObjects.push_back(samplingResultObject);
            mSamplingResultObjectsSync.release();
        }
        else
        {
            on_empty_processed_t();
        }
    }
    else
    {
        on_empty_processed_t();
    }
}

GENERIC McpServerClient::update()
{
    this->default_update_method();
}

GENERIC McpServerClient::default_update_method()
{
    if(mRootsResultObjects.size())
    {
        mRootsResultObjectsSync.acquire();
        mbase::vector<mbase::McpStRootsResultObject> rootResultObjects = std::move(mRootsResultObjects);
        mRootsResultObjects = mbase::vector<mbase::McpStRootsResultObject>();
        mRootsResultObjectsSync.release();

        for(mbase::McpStRootsResultObject& tmpObject : rootResultObjects)
        {
            std::get<mcp_st_list_roots_cb>(tmpObject.rootsResultCallback)(MBASE_MCP_SUCCESS, this->get_server_instance(), tmpObject.rootsResult);
        }
    }

    if(mSamplingResultObjects.size())
    {
        mSamplingResultObjectsSync.acquire();
        mbase::vector<mbase::McpStSamplingResultObject> samplingResultObjects = std::move(mSamplingResultObjects);
        mSamplingResultObjects = mbase::vector<mbase::McpStSamplingResultObject>();
        mSamplingResultObjectsSync.release();

        for(mbase::McpStSamplingResultObject& tmpObject : samplingResultObjects)
        {
            std::get<mcp_st_sampling_cb>(tmpObject.samplingResultCallback)(MBASE_MCP_SUCCESS, this->get_server_instance(), tmpObject.samplingResult);
        }
    }
}

MBASE_END