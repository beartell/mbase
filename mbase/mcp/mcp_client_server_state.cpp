#include <mbase/mcp/mcp_client_server_state.h>
#include <mbase/mcp/mcp_client_base.h>
#include <mbase/mcp/mcp_packet_parsing.h>

MBASE_BEGIN

McpServerRequestTimer::McpServerRequestTimer(McpServerStateBase* in_server) : serverState(in_server)
{
}

GENERIC McpServerRequestTimer::on_call([[maybe_unused]] user_data in_data)
{
    serverState->mRequestMapSync.acquire();
    mbase::unordered_map<mbase::string, McpServerRequestState>::iterator It = serverState->mRequestMap.begin();
    for(;It != serverState->mRequestMap.end();)
    {
        if(It->second.mTimeoutInSeconds < 0)
        {
            // infinite timeout
            continue;
        }
        if(It->second.mAttemptCount == It->second.mTimeoutInSeconds)
        {
            serverState->mQueuedResponsesSync.acquire();
            mbase::McpServerResponseObject respObject;
            respObject.responseCallback = It->second.responseCallback;
            respObject.mErrorCode = MBASE_MCP_TIMEOUT;
            It = serverState->mRequestMap.erase(It);
            serverState->mQueuedResponses.push_back(respObject);
            serverState->mQueuedResponsesSync.release();
            continue;
        }
        It->second.mAttemptCount++;
        ++It;
    }
    serverState->mRequestMapSync.release();
}

McpServerStateBase::McpServerStateBase(
    mbase::mcp_transport_method in_method
) : mRequestTimer(this)
{
    mTransportMethod = in_method;
    mRequestTimer.set_target_time(1000);
    mTimerLoop.register_timer(mRequestTimer);
}

const mbase::string& McpServerStateBase::get_protocol_version() const noexcept
{
    return mProtocolVersion;
}

const mbase::string& McpServerStateBase::get_server_name() const noexcept
{
    return mServerName;
}

const mbase::string& McpServerStateBase::get_server_version() const noexcept
{
    return mServerVersion;
}

const mbase::string& McpServerStateBase::get_instructions() const noexcept
{
    return mInstructions;
}

McpClientBase* McpServerStateBase::get_owner_client() noexcept
{
    return mOwnerClient;
}

mbase::mcp_transport_method McpServerStateBase::get_transport_method() const noexcept
{
    return mTransportMethod;
}

bool McpServerStateBase::is_server_initialized() const noexcept
{
    return mIsServerInitialized;
}

bool McpServerStateBase::is_logging_supported() const noexcept
{
    return mLoggingSupported;
}

bool McpServerStateBase::is_resource_supported() const noexcept
{
    return mResourceSupported;
}

bool McpServerStateBase::is_resource_subscription_supported() const noexcept
{
    return mResourceSubscriptionSupported;
}

bool McpServerStateBase::is_prompt_supported() const noexcept
{
    return mPromptSupported;
}

bool McpServerStateBase::is_tooling_supported() const noexcept
{
    return mToolingSupported;
}

GENERIC McpServerStateBase::on_empty_processed_t()
{

}

GENERIC McpServerStateBase::on_initialize_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_list_resources_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_list_prompts_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_list_tools_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_read_resource_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_prompt_get_result_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_tool_call_result_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_subscribe_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_unsubscribe_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_set_log_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_ping_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

GENERIC McpServerStateBase::on_prompt_compilation_t([[maybe_unused]] McpServerRequestState& in_request, [[maybe_unused]] mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
}

bool McpServerStateBase::initialize(McpClientBase* in_client, mcp_init_cb in_cb, const I64& in_timeout)
{
    if(!in_client)
    {
        return false;
    }
    mOwnerClient = in_client;
    mbase::Json methodParams;
    methodParams["protocolVersion"] = MBASE_MCP_DEFAULT_VERSION;
    methodParams["capabilities"]["roots"]["listChanged"] = true;
    if(mOwnerClient->is_sampling_supported())
    {
        methodParams["capabilities"]["sampling"].setObject();
    }
    methodParams["clientInfo"]["name"] = mOwnerClient->get_client_name();
    methodParams["clientInfo"]["version"] = mOwnerClient->get_client_version();
    
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "initialize", methodParams));
    return true;
}

bool McpServerStateBase::list_resources(mcp_list_resources_cb in_cb, const I64& in_timeout, const mbase::string& in_cursor)
{
    if(!this->is_server_initialized() || !this->is_resource_supported()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    if(in_cursor.size())
    {
        mbase::Json cursorData;
        cursorData["cursor"] = in_cursor;
        this->send_mcp_payload(mbase::mcp_create_request(messageId, "resources/list", cursorData));
    }
    else
    {
        this->send_mcp_payload(mbase::mcp_create_request(messageId, "resources/list"));
    }
    return true;
}

bool McpServerStateBase::list_prompts(mcp_list_prompts_cb in_cb, const I64& in_timeout, const mbase::string& in_cursor)
{
    if(!this->is_server_initialized() || !this->is_prompt_supported()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    if(in_cursor.size())
    {
        mbase::Json cursorData;
        cursorData["cursor"] = in_cursor;
        this->send_mcp_payload(mbase::mcp_create_request(messageId, "prompts/list", cursorData));
    }
    else
    {
        this->send_mcp_payload(mbase::mcp_create_request(messageId, "prompts/list"));
    }
    return true;
}

bool McpServerStateBase::list_tools(mcp_list_tools_cb in_cb, const I64& in_timeout, const mbase::string& in_cursor)
{
    if(!this->is_server_initialized() || !this->is_tooling_supported()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    if(in_cursor.size())
    {
        mbase::Json cursorData;
        cursorData["cursor"] = in_cursor;
        this->send_mcp_payload(mbase::mcp_create_request(messageId, "tools/list", cursorData));
    }
    else
    {
        this->send_mcp_payload(mbase::mcp_create_request(messageId, "tools/list"));
    }
    return true;
}

bool McpServerStateBase::read_resource(const mbase::string& in_uri, mcp_read_resource_cb in_cb, const I64& in_timeout)
{
    if(!this->is_server_initialized() || !this->is_resource_supported() || !in_uri.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json uriInfo;
    uriInfo["uri"] = in_uri;
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "resources/read", uriInfo));
    return true;
}

bool McpServerStateBase::get_prompt(const mbase::string& in_prompt_name, mcp_get_prompt_cb in_cb, const I64& in_timeout, const McpPromptMessageMap& in_arguments)
{
    if(!this->is_server_initialized() || !this->is_prompt_supported() || !in_prompt_name.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json promptRequest;
    promptRequest["name"] = in_prompt_name;
    promptRequest["arguments"].setObject();
    for(auto& argumentMap : in_arguments)
    {
        promptRequest["arguments"][argumentMap.first] = argumentMap.second;   
    }
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "prompts/get", promptRequest));
    return true;
}

bool McpServerStateBase::tool_call(const mbase::string& in_tool_name, mcp_tool_call_cb in_cb, const I64& in_timeout, const McpToolMessageMap& in_arguments)
{
    if(!this->is_server_initialized() || !this->is_tooling_supported() || !in_tool_name.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json toolingRequest;
    toolingRequest["name"] = in_tool_name;
    toolingRequest["arguments"].setObject();
    for(auto& argumentMap : in_arguments)
    {
        if(std::holds_alternative<I64>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first] = std::get<I64>(argumentMap.second);
        }
        else if(std::holds_alternative<F64>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first] = std::get<F64>(argumentMap.second);
        }
        else if(std::holds_alternative<bool>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first] = std::get<bool>(argumentMap.second);
        }
        else if(std::holds_alternative<mbase::string>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first] = std::get<mbase::string>(argumentMap.second);
        }
        else if(std::holds_alternative<mbase::vector<mbase::Json>>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first].setArray();
            mbase::vector<mbase::Json>& argValueArray = toolingRequest["arguments"][argumentMap.first].getArray();   
            argValueArray = std::get<mbase::vector<mbase::Json>>(argumentMap.second);
        }
        else if(std::holds_alternative<std::map<mbase::string, mbase::Json>>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first].setObject();
            std::map<mbase::string, mbase::Json>& argObjMap = toolingRequest["arguments"][argumentMap.first].getObject();
            argObjMap = std::get<std::map<mbase::string, mbase::Json>>(argumentMap.second);
        }
    }
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "tools/call", toolingRequest));
    return true;
}

bool McpServerStateBase::read_resource(const mbase::string& in_uri, mcp_read_resource_cb in_cb, mbase::string& out_progress_token, const I64& in_timeout)
{
    if(!this->is_server_initialized() || !this->is_resource_supported() || !in_uri.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json uriInfo;
    uriInfo["uri"] = in_uri;
    out_progress_token = mbase::string::generate_uuid();
    uriInfo["_meta"]["progressToken"] = out_progress_token;
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "resources/read", uriInfo));
    return true;
}

bool McpServerStateBase::get_prompt(const mbase::string& in_prompt_name, mcp_get_prompt_cb in_cb, const I64& in_timeout, const McpPromptMessageMap& in_arguments, mbase::string& out_progress_token)
{
    if(!this->is_server_initialized() || !this->is_prompt_supported() || !in_prompt_name.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json promptRequest;
    promptRequest["name"] = in_prompt_name;
    promptRequest["arguments"].setObject();
    out_progress_token = mbase::string::generate_uuid();
    promptRequest["_meta"]["progressToken"] = out_progress_token;
    for(auto& argumentMap : in_arguments)
    {
        promptRequest["arguments"][argumentMap.first] = argumentMap.second;   
    }
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "prompts/get", promptRequest));
    return true;
}

bool McpServerStateBase::tool_call(const mbase::string& in_tool_name, mcp_tool_call_cb in_cb, const I64& in_timeout, const McpToolMessageMap& in_arguments, mbase::string& out_progress_token)
{
    if(!this->is_server_initialized() || !this->is_tooling_supported() || !in_tool_name.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json toolingRequest;
    toolingRequest["name"] = in_tool_name;
    toolingRequest["arguments"].setObject();
    out_progress_token = mbase::string::generate_uuid();
    toolingRequest["_meta"]["progressToken"] = out_progress_token;
    for(auto& argumentMap : in_arguments)
    {
        if(std::holds_alternative<I64>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first] = std::get<I64>(argumentMap.second);
        }
        else if(std::holds_alternative<F64>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first] = std::get<F64>(argumentMap.second);
        }
        else if(std::holds_alternative<bool>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first] = std::get<bool>(argumentMap.second);
        }
        else if(std::holds_alternative<mbase::string>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first] = std::get<mbase::string>(argumentMap.second);
        }
        else if(std::holds_alternative<mbase::vector<mbase::Json>>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first].setArray();
            mbase::vector<mbase::Json>& argValueArray = toolingRequest["arguments"][argumentMap.first].getArray();   
            argValueArray = std::get<mbase::vector<mbase::Json>>(argumentMap.second);
        }
        else if(std::holds_alternative<std::map<mbase::string, mbase::Json>>(argumentMap.second))
        {
            toolingRequest["arguments"][argumentMap.first].setObject();
            std::map<mbase::string, mbase::Json>& argObjMap = toolingRequest["arguments"][argumentMap.first].getObject();
            argObjMap = std::get<std::map<mbase::string, mbase::Json>>(argumentMap.second);
        }
    }
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "tools/call", toolingRequest));
    return true;
}

bool McpServerStateBase::subscribe(const mbase::string& in_uri, mcp_empty_cb in_cb, const I64& in_timeout)
{
    if(!this->is_server_initialized() || !this->is_resource_subscription_supported() || !in_uri.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json subscribeRequest;
    subscribeRequest["uri"] = in_uri;
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "resources/subscribe", subscribeRequest));
    return true;
}

bool McpServerStateBase::unsubscribe(const mbase::string& in_uri, mcp_empty_cb in_cb, const I64& in_timeout)
{
    if(!this->is_server_initialized() || !this->is_resource_subscription_supported() || !in_uri.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json unsubscribeRequest;
    unsubscribeRequest["uri"] = in_uri;
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "resources/unsubscribe", unsubscribeRequest));
    return true;
}

bool McpServerStateBase::set_log(mcp_log_levels in_level, mcp_empty_cb in_cb, const I64& in_timeout)
{
    if(!this->is_server_initialized() || !this->is_logging_supported()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json logLevelRequest;
    switch (in_level)
    {
    case mcp_log_levels::L_ALERT:
        logLevelRequest["level"] = "alert";
        break;
    
    case mcp_log_levels::L_CRITICAL:
        logLevelRequest["level"] = "critical";
        break;
    
    case mcp_log_levels::L_DEBUG:
        logLevelRequest["level"] = "debug";
        break;
    
    case mcp_log_levels::L_EMERGENCY:
        logLevelRequest["level"] = "emergency";
        break;
    
    case mcp_log_levels::L_ERROR:
        logLevelRequest["level"] = "error";
        break;
    
    case mcp_log_levels::L_INFO:
        logLevelRequest["level"] = "info";
        break;
    
    case mcp_log_levels::L_NOTICE:
        logLevelRequest["level"] = "notice";
        break;
    
    case mcp_log_levels::L_WARNING:
        logLevelRequest["level"] = "warning";
        break;

    default:
        break;
    }
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "logging/setLevel", logLevelRequest));
    return true;
}

bool McpServerStateBase::ping(mcp_empty_cb in_cb, const I64& in_timeout)
{
    if(!this->is_server_initialized()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "ping"));
    return true;
}

bool McpServerStateBase::prompt_compilation(mcp_prompt_compilation_cb in_cb, const mbase::string& in_prompt_name, const mbase::string& in_argument_name, const mbase::string& in_argument_value, const I64& in_timeout)
{
    if(!this->is_server_initialized() || !in_prompt_name.size() || !in_argument_name.size()) { return false; }
    mbase::string messageId = this->create_request_state(in_cb, in_timeout);
    mbase::Json promptCompilationRequest;
    promptCompilationRequest["ref"]["type"] = "ref/prompt";
    promptCompilationRequest["ref"]["name"] = in_prompt_name;
    promptCompilationRequest["argument"]["name"] = in_argument_name;
    promptCompilationRequest["argument"]["value"] = in_argument_value;
    this->send_mcp_payload(mbase::mcp_create_request(messageId, "completion/complete", promptCompilationRequest));
    return true;
}

GENERIC McpServerStateBase::read_mcp_payload(const mbase::string& in_payload)
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

mbase::string McpServerStateBase::create_request_state(mcp_response_callback in_cb, const I64& in_timeout)
{
    mRequestMapSync.acquire();
    mbase::McpServerRequestState requestState;
    requestState.responseCallback = in_cb;
    requestState.mTimeoutInSeconds = in_timeout;
    mbase::string messageId = mbase::string::generate_uuid();
    mRequestMap[messageId] = requestState;
    mRequestMapSync.release();
    return messageId;
}

GENERIC McpServerStateBase::process_mcp_message(mbase::Json& in_message)
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

GENERIC McpServerStateBase::process_notification_message(mbase::Json& in_message)
{
    // Server sent notifications:
    //
    // Log notif: notifications/message
    // Prompt list changed: notifications/prompts/list_changed
    // Resources list changed: notifications/resources/list_changed
    // Tools list changed: notifications/tools/list_changed
    // Resource updated: notifications/resources/updated
    // Cancel notification: notifications/cancelled
    // Progress notification: notifications/progress

    const mbase::string& notificationMethod = in_message["method"].getString();
    mbase::Json& notifParams = in_message["params"];
    McpClientBase* clientInstance = this->get_owner_client();
    clientInstance->on_server_notification_t(this, notificationMethod, notifParams);
    if(notificationMethod == "notifications/message")
    {
        mbase::McpNotificationLogMessage logMessage;
        if(notifParams["level"].isString())
        {
            const mbase::string& logLevelString = notifParams["level"].getString();
            if(logLevelString == "debug")
            {
                logMessage.mLevel = mbase::mcp_log_levels::L_DEBUG;
            }
            else if(logLevelString == "info")
            {
                logMessage.mLevel = mbase::mcp_log_levels::L_INFO;
            }
            else if(logLevelString == "notice")
            {
                logMessage.mLevel = mbase::mcp_log_levels::L_NOTICE;
            }
            else if(logLevelString == "warning")
            {
                logMessage.mLevel = mbase::mcp_log_levels::L_WARNING;
            }
            else if(logLevelString == "error")
            {
                logMessage.mLevel = mbase::mcp_log_levels::L_ERROR;
            }
            else if(logLevelString == "critical")
            {
                logMessage.mLevel = mbase::mcp_log_levels::L_CRITICAL;
            }
            else if(logLevelString == "alert")
            {
                logMessage.mLevel = mbase::mcp_log_levels::L_ALERT;
            }
            else if(logLevelString == "emergency")
            {
                logMessage.mLevel = mbase::mcp_log_levels::L_EMERGENCY;
            }
        }
        
        if(notifParams["logger"].isString())
        {
            logMessage.mLogger = notifParams["logger"].getString();
        }
        
        if(notifParams["data"]["error"].isString())
        {
            logMessage.mError = notifParams["data"]["error"].getString();
        }

        if(notifParams["data"]["details"].isObject())
        {
            logMessage.mDetails = notifParams["data"]["details"];
        }
        clientInstance->on_log_message_t(this, logMessage);
    }

    else if(notificationMethod == "notifications/prompts/list_changed")
    {
        clientInstance->on_prompt_list_changed_t(this);
    }

    else if(notificationMethod == "notifications/resources/list_changed")
    {
        clientInstance->on_resource_list_changed_t(this);
    }

    else if(notificationMethod == "notifications/tools/list_changed")
    {
        clientInstance->on_tool_list_changed_t(this);
    }

    else if(notificationMethod == "notifications/resources/updated")
    {
        if(notifParams["uri"].isString())
        {
            clientInstance->on_resource_updated_t(this, notifParams["uri"].getString());
        }
    }

    else if(notificationMethod == "notifications/cancelled")
    {
        mbase::McpNotificationCancellationIdStr cancelNotif;
        if(notifParams["requestId"].isString())
        {
            cancelNotif.mIdString = notifParams["requestId"].getString();
        }

        if(notifParams["reason"].isString())
        {
            cancelNotif.mReason = notifParams["reason"].getString();
        }
        clientInstance->on_cancellation_t(this, cancelNotif);
    }

    else if(notificationMethod == "notifications/progress")
    {
        mbase::McpNotificationProgress progressNotif;
        if(!notifParams["progressToken"].isString())
        {
            on_empty_processed_t();
            return;
        }

        progressNotif.mProgressToken = notifParams["progressToken"].getString();
        if(notifParams["progress"].isNumber())
        {
            progressNotif.mProgress = notifParams["progress"].getNumber();
        }

        if(notifParams["total"].isNumber())
        {
            progressNotif.mProgressTotal = notifParams["total"].getNumber();
        }

        if(notifParams["message"].isString())
        {
            progressNotif.mProgressMessage = notifParams["message"].getString();
        }
        clientInstance->on_progress_notification_t(this, progressNotif);
    }

    else
    {
        on_empty_processed_t();
    }
}

GENERIC McpServerStateBase::process_request_message(mbase::Json& in_message)
{
    // Server sent supported requests:
    // - List roots: roots/list
    // - Sampling: sampling/createMessage
    const mbase::string& requestedMethod = in_message["method"].getString();
    mbase::Json messageId = in_message["id"];
    mbase::Json& messageParams = in_message["params"];
    McpClientBase* clientInstance = this->get_owner_client();
    if(clientInstance->on_server_request_t(this, messageId, requestedMethod, messageParams))
    {
        return;
    }

    if(requestedMethod == "roots/list")
    {
        mbase::unordered_map<mbase::string, mbase::string> clientRoots = this->get_owner_client()->get_roots();
        mbase::Json responseData;
        responseData["roots"].setArray();
        mbase::vector<mbase::Json>& rootArray = responseData["roots"].getArray();
        for(auto& rootPair : clientRoots)
        {
            mbase::Json rootObject;
            rootObject["uri"] = rootPair.first;
            rootObject["name"] = rootPair.second;
            rootArray.push_back(rootObject);
        }
        this->send_mcp_payload(mbase::mcp_generate_response(messageId, responseData));
    }

    else if(requestedMethod == "sampling/createMessage")
    {
        if(!this->get_owner_client()->is_sampling_supported())
        {
            this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_METHOD_NOT_FOUND, "Method not supported"));
            return;
        }

        mbase::McpSamplingRequestObject samplingRequestObject;
        samplingRequestObject.mRequestId = messageId;
        samplingRequestObject.requestOwner = this;
        if(messageParams["systemPrompt"].isString())
        {
            samplingRequestObject.samplingRequest.mSystemPrompt = messageParams["systemPrompt"].getString();
        }
        if(messageParams["maxTokens"].isLong())
        {
            samplingRequestObject.samplingRequest.mMaxTokens = messageParams["maxTokens"].getLong();
        }
        if(messageParams["modelPreferences"]["intelligencePriority"].isFloat())
        {
            samplingRequestObject.samplingRequest.mModelPreferences.mIntelligencePriority = messageParams["modelPreferences"]["intelligencePriority"].getFloat();
        }
        if(messageParams["modelPreferences"]["speedPriority"].isFloat())
        {
            samplingRequestObject.samplingRequest.mModelPreferences.mSpeedPriority = messageParams["modelPreferences"]["speedPriority"].getFloat();
        }

        if(messageParams["messages"].isArray())
        {
            mbase::vector<mbase::Json>& messagesArray = messageParams["messages"].getArray();
            for(mbase::Json& messageObject : messagesArray)
            {
                mbase::McpSamplingMessage samplingMessage;
                if(messageObject["role"].isString())
                {
                    samplingMessage.mRole = messageObject["role"].getString();
                }

                if(messageObject["content"]["mimeType"].isString())
                {
                    samplingMessage.mMimeType = messageObject["content"]["mimeType"].getString();
                }

                if(messageObject["content"]["data"].isString())
                {
                    samplingMessage.mBase64Data = messageObject["content"]["data"].getString();
                }   

                if(messageObject["content"]["text"].isString())
                {
                    samplingMessage.mTextContent = messageObject["content"]["text"].getString();
                }

                if(messageObject["content"]["type"].isString())
                {
                    const mbase::string& contentType = messageObject["content"]["type"].getString();
                    if(contentType == "text")
                    {
                        samplingMessage.mContentType = mbase::mcp_sampling_content_type::TEXT;
                    }
                    else if(contentType == "image")
                    {
                        samplingMessage.mContentType = mbase::mcp_sampling_content_type::IMAGE;
                    }
                    else if(contentType == "audio")
                    {
                        samplingMessage.mContentType = mbase::mcp_sampling_content_type::AUDIO;
                    }
                }
                samplingRequestObject.samplingRequest.mMessages.push_back(samplingMessage);
            }
        }
        if(messageParams["modelPreferences"]["hints"].isArray())
        {
            for(mbase::Json& hintObject : messageParams["modelPreferences"]["hints"].getArray())
            {
                if(hintObject["name"].isString())
                {
                    samplingRequestObject.samplingRequest.mModelPreferences.mModelHintsList.push_back(hintObject["name"].getString());
                }
            }
        }
        this->get_owner_client()->on_sampling_request_t(this, samplingRequestObject);
    }

    else
    {
        this->send_mcp_payload(mbase::mcp_generate_error_message(messageId, MBASE_MCP_METHOD_NOT_FOUND, "Method not found"));
    }
}

GENERIC McpServerStateBase::process_response_message(mbase::Json& in_message)
{
    McpClientBase* clientInstance = this->get_owner_client();
    if(in_message["id"].isString())
    {
        const mbase::string& requestId = in_message["id"].getString();
        clientInstance->on_server_response_t(this, in_message["id"].getString(), in_message["result"]);
        mRequestMapSync.acquire();
        mbase::unordered_map<mbase::string, McpServerRequestState>::iterator It = mRequestMap.find(requestId);
        if(It == mRequestMap.end())
        {
            mRequestMapSync.release();
            on_empty_processed_t();
            return;
        }
        McpServerRequestState requestState = std::move(It->second);
        mRequestMap.erase(It);
        mRequestMapSync.release();
        mbase::mcp_err_format errFormat;
        mbase::McpServerResponseObject responseObject;
        errFormat = mbase::mcp_extract_error_information(in_message);
        if(!in_message["result"].isObject() && errFormat.mCode == MBASE_MCP_SUCCESS)
        {
            errFormat.mCode = MBASE_MCP_INTERNAL_ERROR;
        }
        responseObject.mErrorCode = errFormat.mCode;
        responseObject.responseCallback = requestState.responseCallback;

        if(std::holds_alternative<mcp_init_cb>(responseObject.responseCallback))
        {
            this->on_initialize_t(requestState, responseObject, in_message["result"], errFormat);
            this->default_initialize_t(requestState, responseObject, in_message["result"], errFormat);
        }

        else if(std::holds_alternative<mcp_read_resource_cb>(responseObject.responseCallback))
        {
            this->on_read_resource_t(requestState, responseObject, in_message["result"], errFormat);
            this->default_read_resource_t(requestState, responseObject, in_message["result"], errFormat);
        }

        else if(std::holds_alternative<mcp_get_prompt_cb>(responseObject.responseCallback))
        {
            this->on_prompt_get_result_t(requestState, responseObject, in_message["result"], errFormat);
            this->default_prompt_get_result_t(requestState, responseObject, in_message["result"], errFormat);
        }

        else if(std::holds_alternative<mcp_tool_call_cb>(responseObject.responseCallback))
        {
            this->on_tool_call_result_t(requestState, responseObject, in_message["result"], errFormat);
            this->default_tool_call_result_t(requestState, responseObject, in_message["result"], errFormat);
        }

        else if(std::holds_alternative<mcp_list_resources_cb>(responseObject.responseCallback))
        {
            this->on_list_resources_t(requestState, responseObject, in_message["result"], errFormat);
            this->default_list_resources_t(requestState, responseObject, in_message["result"], errFormat);
        }

        else if(std::holds_alternative<mcp_list_prompts_cb>(responseObject.responseCallback))
        {
            this->on_list_prompts_t(requestState, responseObject, in_message["result"], errFormat);
            this->default_list_prompts_t(requestState, responseObject, in_message["result"], errFormat);
        }

        else if(std::holds_alternative<mcp_list_tools_cb>(responseObject.responseCallback))
        {
            this->on_list_tools_t(requestState, responseObject, in_message["result"], errFormat);
            this->default_list_tools_t(requestState, responseObject, in_message["result"], errFormat);
        }

        else if(std::holds_alternative<mcp_empty_cb>(responseObject.responseCallback))
        {
            this->on_subscribe_t(requestState, responseObject, errFormat);
            this->default_subscribe_t(requestState, responseObject, errFormat);
        }

        else if(std::holds_alternative<mcp_empty_cb>(responseObject.responseCallback))
        {
            this->on_unsubscribe_t(requestState, responseObject, errFormat);
            this->default_unsubscribe_t(requestState, responseObject, errFormat);
        }

        else if(std::holds_alternative<mcp_empty_cb>(responseObject.responseCallback))
        {
            this->on_set_log_t(requestState, responseObject, errFormat);
            this->default_set_log_t(requestState, responseObject, errFormat);
        }

        else if(std::holds_alternative<mcp_empty_cb>(responseObject.responseCallback))
        {
            this->on_ping_t(requestState, responseObject, errFormat);
            this->default_ping_t(requestState, responseObject, errFormat);
        }

        else if(std::holds_alternative<mcp_prompt_compilation_cb>(responseObject.responseCallback))
        {
            this->on_prompt_compilation_t(requestState, responseObject, in_message["result"], errFormat);
            this->default_prompt_compilation_t(requestState, responseObject, in_message["result"], errFormat);
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

GENERIC McpServerStateBase::update()
{
    mTimerLoop.run_timers();
    mQueuedResponsesSync.acquire();
    mbase::vector<McpServerResponseObject> movedResponseObject = std::move(mQueuedResponses);
    mQueuedResponses = mbase::vector<McpServerResponseObject>();
    mQueuedResponsesSync.release();
    mbase::McpClientBase* clientInstance = this->get_owner_client();
    for(mbase::McpServerResponseObject& responseObject : movedResponseObject)
    {
        const I32& errCode = responseObject.mErrorCode;
        mbase::Json& resParams = responseObject.mResponseParams;
        if(std::holds_alternative<mcp_init_cb>(responseObject.responseCallback))
        {
            if(errCode == MBASE_MCP_TIMEOUT)
            {
                std::get<mcp_init_cb>(responseObject.responseCallback)(errCode, clientInstance, this);
                continue;
            }
            if(
                !resParams["protocolVersion"].isString() || 
                !resParams["serverInfo"]["name"].isString() ||
                !resParams["serverInfo"]["version"].isString()||
                !resParams["capabilities"].isObject()
            )
            {
                std::get<mcp_init_cb>(responseObject.responseCallback)(errCode, clientInstance, this);
                continue;
            }

            const mbase::string& serverProtocolVersion = resParams["protocolVersion"].getString();

            if(serverProtocolVersion != "2024-11-05" && serverProtocolVersion != "2025-03-26")
            {
                std::get<mcp_init_cb>(responseObject.responseCallback)(errCode, clientInstance, this);
                continue;
            }

            for(auto& capabilityString : resParams["capabilities"].getObject())
            {
                if(capabilityString.first == "logging")
                {
                    mLoggingSupported = true;
                }

                else if(capabilityString.first == "prompts")
                {
                    mPromptSupported = true;
                }

                else if(capabilityString.first == "resources")
                {
                    mResourceSupported = true;
                    if(capabilityString.second["subscribe"].isBool())
                    {
                        mResourceSubscriptionSupported = capabilityString.second["subscribe"].getBool();
                    }
                }

                else if(capabilityString.first == "tools")
                {
                    mToolingSupported = true;
                }
            }

            if(resParams["instructions"].isString())
            {
                mInstructions = resParams["instructions"].getString();
            }

            mProtocolVersion = serverProtocolVersion;
            mServerName = resParams["serverInfo"]["name"].getString();
            mServerVersion = resParams["serverInfo"]["version"].getString();
            mIsServerInitialized = true;
            this->send_mcp_payload(mbase::mcp_generate_notification("notifications/initialized"));
            std::get<mcp_init_cb>(responseObject.responseCallback)(errCode, clientInstance, this);
        }

        else if(std::holds_alternative<mcp_read_resource_cb>(responseObject.responseCallback))
        {
            mbase::vector<mbase::McpResponseResource> resourceRespond;
            if(resParams["contents"].isArray())
            {
                for(mbase::Json& contentsObject : resParams["contents"].getArray())
                {
                    mbase::string uriContent;
                    mbase::string mimeType;
                    if(contentsObject["uri"].isString())
                    {
                        uriContent = contentsObject["uri"].getString();
                    }

                    if(contentsObject["mimeType"].isString())
                    {
                        mimeType = contentsObject["mimeType"].getString();
                    }

                    if(contentsObject["text"].isString())
                    {
                        mbase::McpResponseTextResource textResource;
                        textResource.mUri = std::move(uriContent);
                        textResource.mMimeType = std::move(mimeType);
                        textResource.mText = contentsObject["text"].getString();
                        resourceRespond.push_back(std::move(textResource));
                    }
                    else if(contentsObject["blob"].isString())
                    {
                        mbase::McpResponseBinaryResource binaryResource;
                        binaryResource.mUri = std::move(uriContent);
                        binaryResource.mMimeType = std::move(mimeType);
                        binaryResource.mBlob = contentsObject["blob"].getString();
                        resourceRespond.push_back(std::move(binaryResource));
                    }
                }
            }
            std::get<mcp_read_resource_cb>(responseObject.responseCallback)(errCode, clientInstance, std::move(resourceRespond));
        }
        else if(std::holds_alternative<mcp_get_prompt_cb>(responseObject.responseCallback))
        {
            // TODO: Implement embedded resource on get prompt
            mbase::string tmpDescription;
            mbase::vector<McpResponsePrompt> responsePrompts;
            if(resParams["description"].isString())
            {
                tmpDescription = resParams["description"].getString();
            }

            if(resParams["messages"].isArray())
            {
                for(mbase::Json& messageContent : resParams["messages"].getArray())
                {
                    mbase::string msgRole;
                    if(messageContent["role"].isString())
                    {
                        msgRole = messageContent["role"].getString();
                    }

                    if(messageContent["content"]["type"].isString())
                    {
                        mbase::Json& msgContent = messageContent["content"];
                        const mbase::string& contentType = messageContent["content"]["type"].getString();
                        if(contentType == "text")
                        {
                            if(msgContent["text"].isString())
                            {
                                mbase::McpResponseTextPrompt textPrompt;
                                textPrompt.mRole = msgRole;
                                textPrompt.mType = contentType;
                                textPrompt.mText = msgContent["text"].getString();
                                responsePrompts.push_back(std::move(textPrompt));
                            }
                        }
                        else if(contentType == "image")
                        {
                            if(msgContent["data"].isString() && msgContent["mimeType"].isString())
                            {
                                mbase::McpResponseImagePrompt imagePrompt;
                                imagePrompt.mRole = msgRole;
                                imagePrompt.mType = contentType;
                                imagePrompt.mMimeType = msgContent["mimeType"].getString();
                                imagePrompt.mData = msgContent["data"].getString();
                                responsePrompts.push_back(std::move(imagePrompt));
                            }
                        }
                        else if(contentType == "audio")
                        {
                            if(msgContent["data"].isString() && msgContent["mimeType"].isString())
                            {
                                mbase::McpResponseAudioPrompt audioPrompt;
                                audioPrompt.mRole = msgRole;
                                audioPrompt.mType = contentType;
                                audioPrompt.mMimeType = msgContent["mimeType"].getString();
                                audioPrompt.mData = msgContent["data"].getString();
                                responsePrompts.push_back(std::move(audioPrompt));
                            }
                        }
                        else if(contentType == "resource")
                        {
                            // TODO: Implement
                        }
                    }
                }
            }
            std::get<mcp_get_prompt_cb>(responseObject.responseCallback)(errCode, clientInstance, tmpDescription, std::move(responsePrompts));
        }
        else if(std::holds_alternative<mcp_tool_call_cb>(responseObject.responseCallback))
        {
            bool tmpIsError = false;
            mbase::vector<mbase::McpResponseTool> responseTools;
            if(resParams["isError"].isBool())
            {
                tmpIsError = resParams["isError"].getBool();
            }

            if(resParams["content"].isArray())
            {
                for(mbase::Json& messageContent : resParams["content"].getArray())
                {
                    if(messageContent["type"].isString())
                    {
                        const mbase::string& contentType = messageContent["type"].getString();
                        if(contentType == "text")
                        {
                            mbase::McpResponseTextTool textToolResponse;
                            textToolResponse.mType = contentType;
                            if(messageContent["text"].isString())
                            {
                                textToolResponse.mText = messageContent["text"].getString();
                            }
                            responseTools.push_back(std::move(textToolResponse));
                        }
                        else if(contentType == "image")
                        {
                            mbase::McpResponseImageTool imageToolResponse;
                            imageToolResponse.mType = contentType;
                            if(messageContent["mimeType"].isString() && messageContent["data"].isString())
                            {
                                imageToolResponse.mMimeType = messageContent["mimeType"].getString();
                                imageToolResponse.mData = messageContent["data"].getString();
                            }
                            responseTools.push_back(std::move(imageToolResponse));
                        }
                        else if(contentType == "audio")
                        {
                            mbase::McpResponseAudioTool audioToolResponse;
                            audioToolResponse.mType = contentType;
                            if(messageContent["mimeType"].isString() && messageContent["data"].isString())
                            {
                                audioToolResponse.mMimeType = messageContent["mimeType"].getString();
                                audioToolResponse.mData = messageContent["data"].getString();
                            }
                            responseTools.push_back(std::move(audioToolResponse));
                        }
                        else if(contentType == "resource")
                        {
                            // TODO: Implement
                        }
                    }
                }
            }

            std::get<mcp_tool_call_cb>(responseObject.responseCallback)(errCode, clientInstance, std::move(responseTools), tmpIsError);
        }
        else if(std::holds_alternative<mcp_list_resources_cb>(responseObject.responseCallback))
        {
            mbase::vector<mbase::McpResourceDescription> resourceDescriptions;
            mbase::string cursorString;
            if(resParams["nextCursor"].isString())
            {
                cursorString = resParams["nextCursor"].getString();
            }
            
            if(resParams["resources"].isArray())
            {
                for(mbase::Json& resourceObject : resParams["resources"].getArray())
                {
                    mbase::McpResourceDescription resDesc;
                    if(resourceObject["uri"].isString())
                    {
                        resDesc.mUri = resourceObject["uri"].getString();
                    }
                    
                    if(resourceObject["name"].isString())
                    {
                        resDesc.mName = resourceObject["name"].getString();
                    }

                    if(resourceObject["description"].isString())
                    {
                        resDesc.mDescription = resourceObject["description"].getString();
                    }

                    if(resourceObject["mimeType"].isString())
                    {
                        resDesc.mMimeType = resourceObject["mimeType"].getString();
                    }

                    if(resourceObject["size"].isLong())
                    {
                        resDesc.mSize = resourceObject["size"].getLong();
                    }
                    resourceDescriptions.push_back(std::move(resDesc));
                }
            }
            
            std::get<mcp_list_resources_cb>(responseObject.responseCallback)(errCode, clientInstance, std::move(resourceDescriptions), cursorString);
        }
        else if(std::holds_alternative<mcp_list_prompts_cb>(responseObject.responseCallback))
        {
            mbase::string cursorString;
            if(resParams["nextCursor"].isString())
            {
                cursorString = resParams["nextCursor"].getString();
            }
            mbase::vector<mbase::McpPromptDescription> promptDescriptions;

            if(resParams["prompts"].isArray())
            {
                for(mbase::Json& promptObject : resParams["prompts"].getArray())
                {
                    mbase::McpPromptDescription prDesc;
                    if(promptObject["name"].isString())
                    {
                        prDesc.mName = promptObject["name"].getString();
                    }

                    if(promptObject["description"].isString())
                    {
                        prDesc.mDescription = promptObject["description"].getString();
                    }

                    if(promptObject["arguments"].isArray())
                    {
                        for(mbase::Json& promptArgumentObject : promptObject["arguments"].getArray())
                        {
                            mbase::McpPromptArgument prArg;
                            if(promptArgumentObject["name"].isString())
                            {
                                prArg.mArgumentName = promptArgumentObject["name"].getString();
                            }

                            if(promptArgumentObject["description"].isString())
                            {
                                prArg.mDescription = promptArgumentObject["description"].getString();
                            }

                            if(promptArgumentObject["required"].isBool())
                            {
                                prArg.mIsRequired = promptArgumentObject["required"].getBool();
                            }
                            prDesc.mArguments.push_back(std::move(prArg));
                        }
                    }
                    promptDescriptions.push_back(std::move(prDesc));
                }
            }
            std::get<mcp_list_prompts_cb>(responseObject.responseCallback)(errCode, clientInstance, std::move(promptDescriptions), cursorString);
        }
        else if(std::holds_alternative<mcp_list_tools_cb>(responseObject.responseCallback))
        {
            mbase::string cursorString;
            if(resParams["nextCursor"].isString())
            {
                cursorString = resParams["nextCursor"].getString();
            }
            mbase::vector<mbase::McpToolDescription> toolDescriptions;
            if(resParams["tools"].isArray())
            {
                for(mbase::Json& toolObject : resParams["tools"].getArray())
                {
                    mbase::McpToolDescription toolDesc;
                    if(toolObject["name"].isString())
                    {
                        toolDesc.mName = toolObject["name"].getString();
                    }

                    if(toolObject["description"].isString())
                    {
                        toolDesc.mDescription = toolObject["description"].getString();
                    }

                    if(toolObject["inputSchema"].isObject())
                    {
                        mbase::Json& inputSchemaObject = toolObject["inputSchema"];
                        mbase::vector<mbase::string> requiredArgs;
                        if(inputSchemaObject["required"].isArray())
                        {
                            for(mbase::Json& requiredName : inputSchemaObject["required"].getArray())
                            {
                                if(requiredName.isString())
                                {
                                    requiredArgs.push_back(requiredName.getString());
                                }
                            }
                        }

                        // don't need the "type" : "object" => useless shit

                        if(inputSchemaObject["properties"].isObject())
                        {
                            for(auto& propObject : inputSchemaObject["properties"].getObject())
                            {
                                const mbase::string& argumentName = propObject.first;
                                if(propObject.second["type"].isString())
                                {
                                    mbase::McpToolArgument toolArgument;
                                    toolArgument.mArgumentName = argumentName;
                                    if(mbase::find(requiredArgs.begin(), requiredArgs.end(), argumentName) != requiredArgs.end())
                                    {
                                        toolArgument.mIsRequired = true;
                                    }
                                    const mbase::string& argumentType = propObject.second["type"].getString();
                                    
                                    if(argumentType == "string")
                                    {
                                        toolArgument.mArgType = mbase::McpValueType::STRING;
                                    }
                                    else if(argumentType == "number")
                                    {
                                        toolArgument.mArgType = mbase::McpValueType::NUMBER;
                                    }
                                    else if(argumentType == "array")
                                    {
                                        toolArgument.mArgType = mbase::McpValueType::ARRAY;
                                    }
                                    else if(argumentType == "object")
                                    {
                                        toolArgument.mArgType = mbase::McpValueType::JSON;
                                    }
                                    else if(argumentType == "boolean")
                                    {
                                        // may be incorrect
                                        toolArgument.mArgType = mbase::McpValueType::BOOL;
                                    }
                                    else
                                    {
                                        continue;
                                    }

                                    if(propObject.second["description"].isString())
                                    {
                                        toolArgument.mDescription = propObject.second["description"].getString();
                                    }
                                    toolDesc.mArguments.push_back(std::move(toolArgument));
                                }
                            }
                        }
                    }
                    toolDescriptions.push_back(std::move(toolDesc));
                }
            }
            std::get<mcp_list_tools_cb>(responseObject.responseCallback)(errCode, clientInstance, std::move(toolDescriptions), cursorString);
        }
        else if(std::holds_alternative<mcp_empty_cb>(responseObject.responseCallback))
        {
            std::get<mcp_empty_cb>(responseObject.responseCallback)(errCode, clientInstance);
        }
        else if(std::holds_alternative<mcp_empty_cb>(responseObject.responseCallback))
        {
            std::get<mcp_empty_cb>(responseObject.responseCallback)(errCode, clientInstance);
        }
        else if(std::holds_alternative<mcp_empty_cb>(responseObject.responseCallback))
        {
            std::get<mcp_empty_cb>(responseObject.responseCallback)(errCode, clientInstance);
        }
        else if(std::holds_alternative<mcp_empty_cb>(responseObject.responseCallback))
        {
            std::get<mcp_empty_cb>(responseObject.responseCallback)(errCode, clientInstance);
        }
        else if(std::holds_alternative<mcp_prompt_compilation_cb>(responseObject.responseCallback))
        {
            mbase::McpPromptCompilationResult compRes;
            if(resParams["completion"]["values"].isArray())
            {
                for(mbase::Json& complValue : resParams["completion"]["values"].getArray())
                {
                    if(complValue.isString())
                    {
                        compRes.mValues.push_back(complValue.getString());
                    }
                }
            }

            if(resParams["completion"]["total"].isLong())
            {
                compRes.mTotal = resParams["completion"]["total"].getLong();
            }

            if(resParams["completion"]["hasMore"].isBool())
            {
                compRes.mHasMore = resParams["completion"]["hasMore"].getBool();
            }
            std::get<mcp_prompt_compilation_cb>(responseObject.responseCallback)(errCode, clientInstance, compRes);
        }
    }
}

GENERIC McpServerStateBase::default_initialize_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    if(in_response.isObject()){ in_response_object.mResponseParams = std::move(in_response); }
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_list_resources_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    if(in_response.isObject()){ in_response_object.mResponseParams = std::move(in_response); }
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_list_prompts_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    if(in_response.isObject()){ in_response_object.mResponseParams = std::move(in_response); }
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_list_tools_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    if(in_response.isObject()){ in_response_object.mResponseParams = std::move(in_response); }
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_read_resource_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    if(in_response.isObject()){ in_response_object.mResponseParams = std::move(in_response); }
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_prompt_get_result_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    if(in_response.isObject()){ in_response_object.mResponseParams = std::move(in_response); }
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_tool_call_result_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    if(in_response.isObject()){ in_response_object.mResponseParams = std::move(in_response); }
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_subscribe_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_unsubscribe_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_set_log_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_ping_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}

GENERIC McpServerStateBase::default_prompt_compilation_t([[maybe_unused]] McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, [[maybe_unused]] const mbase::mcp_err_format& in_error)
{
    if(in_response.isObject()){ in_response_object.mResponseParams = std::move(in_response); }
    mbase::lock_guard responseQueueSync(mQueuedResponsesSync);
    mQueuedResponses.push_back(std::move(in_response_object));
}


MBASE_END