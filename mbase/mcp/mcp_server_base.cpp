#include <mbase/mcp/mcp_server_base.h>
#include <mbase/mcp/mcp_packet_parsing.h>
#include <iostream>

MBASE_BEGIN

McpServerBase::McpServerBase(const mbase::string& in_server_name, const mbase::string& in_version_string, mbase::mcp_transport_method in_method):
    mServerName(in_server_name),
    mServerVersion(in_version_string),
    mTransportMethod(in_method),
    mPaginationMin(10)
{
}

McpServerBase::~McpServerBase()
{
}

mbase::McpToolFeature* McpServerBase::get_tool_feature(const mbase::string& in_name) const noexcept
{
    mbase::unordered_map<mbase::string, mbase::McpToolFeature>::const_iterator tIt = mToolMap.find(in_name);
    if(tIt == mToolMap.end())
    {
        return nullptr;
    }
    return &tIt->second;
}

mbase::McpPromptFeature* McpServerBase::get_prompt_feature(const mbase::string& in_name) const noexcept
{
    mbase::unordered_map<mbase::string, mbase::McpPromptFeature>::const_iterator tIt = mPromptMap.find(in_name);
    if(tIt == mPromptMap.end())
    {
        return nullptr;
    }
    return &tIt->second;
}

mbase::McpResourceFeature* McpServerBase::get_resource_feature(const mbase::string& in_name) const noexcept
{
    mbase::unordered_map<mbase::string, mbase::McpResourceFeature>::const_iterator tIt = mResourceMap.find(in_name);
    if(tIt == mResourceMap.end())
    {
        return nullptr;
    }
    return &tIt->second;
}

mbase::mcp_transport_method McpServerBase::get_transport_method() const noexcept
{
    return mTransportMethod;
}

const mbase::string& McpServerBase::get_server_name() const noexcept
{
    return mServerName;
}

const mbase::string& McpServerBase::get_server_version() const noexcept
{
    return mServerVersion;
}

const I32& McpServerBase::get_pagination_minimum() const noexcept
{
    return mPaginationMin;
}

GENERIC McpServerBase::set_pagination_min_content(const I32& in_pagination_min) noexcept
{
    if(in_pagination_min < 1)
    {
        mPaginationMin = 1;
    }
    mPaginationMin = in_pagination_min;
}

bool McpServerBase::on_client_request_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params)
{
    return false;
}

GENERIC McpServerBase::on_client_notification_t(mbase::McpServerClient* in_client, const mbase::string& in_method, const mbase::Json& in_params)
{
}

GENERIC McpServerBase::on_client_response_t(mbase::McpServerClient* in_client, const mbase::string& in_msgid, const mbase::Json& in_params)
{
}

GENERIC McpServerBase::on_client_init(mbase::McpServerClient* in_client)
{   
}

GENERIC McpServerBase::on_roots_list_changed(mbase::McpServerClient* in_client)
{   
}

GENERIC McpServerBase::on_cancellation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_reason)
{
}

GENERIC McpServerBase::on_ping_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid)
{
}

GENERIC McpServerBase::on_logging_set_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, mcp_log_levels in_log_level)
{
}

GENERIC McpServerBase::on_resource_subscribe(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri)
{
}

GENERIC McpServerBase::on_resource_unsubscribe(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri)
{
}

GENERIC McpServerBase::on_prompt_compilation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_prompt, const mbase::string& in_argument_name, const mbase::string& in_argument_value)
{
}

GENERIC McpServerBase::on_list_tool_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination)
{
}

GENERIC McpServerBase::on_list_prompt_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination)
{
}

GENERIC McpServerBase::on_list_resource_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination)
{
}

GENERIC McpServerBase::on_tool_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpToolFeature* in_tool, McpMessageMap& in_arguments)
{
}

GENERIC McpServerBase::on_prompt_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpPromptFeature*in_prompt, McpMessageMap& in_arguments)
{   
}

GENERIC McpServerBase::on_resource_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpResourceFeature* in_resource)
{
}

GENERIC McpServerBase::send_prompt_call_result(McpServerClient* in_client, const mbase::Json& in_msgid, McpPromptFeature* in_feature, mbase::vector<McpResponsePrompt>& in_result)
{
    mbase::Json responseData;
    responseData["description"] = in_feature->get_prompt_description().mDescription;
    responseData["messages"].setArray();

    mbase::vector<mbase::Json>& msgArray = responseData["messages"].getArray();
    for(const McpResponsePrompt& currentPrompt : in_result)
    {
        mbase::Json messageJson;
        if(std::holds_alternative<McpResponseTextPrompt>(currentPrompt))
        {
            const McpResponseTextPrompt& textPrompt = std::get<McpResponseTextPrompt>(currentPrompt);
            messageJson["role"] = textPrompt.mRole;
            messageJson["content"]["type"] = textPrompt.mType;
            messageJson["content"]["text"] = textPrompt.mText;
        }
        else if(std::holds_alternative<McpResponseImagePrompt>(currentPrompt))
        {
            const McpResponseImagePrompt& imagePrompt = std::get<McpResponseImagePrompt>(currentPrompt);
            messageJson["role"] = imagePrompt.mRole;
            messageJson["content"]["type"] = imagePrompt.mType;
            messageJson["content"]["data"] = imagePrompt.mData;
            messageJson["content"]["mimeType"] = imagePrompt.mMimeType;
        }
        msgArray.push_back(messageJson);
    }
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, responseData));
}

GENERIC McpServerBase::send_resource_call_result(McpServerClient* in_client, const mbase::Json& in_msgid, McpResourceFeature* in_feature, McpResponseResource& in_result)
{
    mbase::Json responseData;
    responseData["contents"].setArray();
    responseData["contents"][0]["uri"] = in_feature->get_resource_description().mUri;
    responseData["contents"][0]["mimeType"] = in_feature->get_resource_description().mMimeType;
    if(std::holds_alternative<McpResponseTextResource>(in_result))
    {
        const McpResponseTextResource& textResource = std::get<McpResponseTextResource>(in_result);
        responseData["contents"][0]["text"] = textResource.mText;
    }

    else if(std::holds_alternative<McpResponseBinaryResource>(in_result))
    {
        const McpResponseBinaryResource& binaryResource = std::get<McpResponseBinaryResource>(in_result);
        responseData["contents"][0]["blob"] = binaryResource.mBlob;
    }
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, responseData));
}

GENERIC McpServerBase::send_tool_call_result(McpServerClient* in_client, const mbase::Json& in_msgid, McpToolFeature* in_feature, McpResponseTool& in_result)
{
    mbase::Json responseData;
    responseData["content"].setArray();
    responseData["isError"] = false;

    if(std::holds_alternative<McpResponseTextTool>(in_result))
    {
        const McpResponseTextTool& textToolResponse = std::get<McpResponseTextTool>(in_result);
        responseData["content"][0]["type"] = textToolResponse.mType;
        responseData["content"][0]["text"] = textToolResponse.mText;
    }
    
    else if(std::holds_alternative<McpResponseImageTool>(in_result))
    {
        const McpResponseImageTool& imageToolResponse = std::get<McpResponseImageTool>(in_result);
        responseData["content"][0]["type"] = imageToolResponse.mType;
        responseData["content"][0]["mimeType"] = imageToolResponse.mMimeType;
        responseData["content"][0]["data"] = imageToolResponse.mData;
    }
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, responseData));
}

McpServerBase::register_result McpServerBase::register_tool(const mbase::McpToolDescription& in_description, mcp_server_tool_cb in_cb)
{
    if(!in_description.mName.size())
    {
        return McpServerBase::register_result::INVALID_DESCRIPTION_PARAMS;
    }

    if(!in_cb)
    {
        return McpServerBase::register_result::MISSING_CALLBACK;
    }

    this->acquire_synchronizer();

    const mbase::string& toolName = in_description.mName;
    mbase::McpToolFeature* tmpToolFeature = this->get_tool_feature(toolName);
    register_result rgrRes = McpServerBase::register_result::LIST_CHANGED;
    if(!tmpToolFeature)
    {
        // means the tool doesn't exist
        McpToolFeature toolFeature;
        toolFeature.update_tool_description(in_description, in_cb);
        mToolMap[toolName] = toolFeature;
        rgrRes = McpServerBase::register_result::SUCCESS;
    }
    
    else
    {
        // means the tool will be modified
        tmpToolFeature->update_tool_description(in_description, in_cb);
        send_tool_list_changed_notification();
    }

    this->release_synchronizer();
    return rgrRes;
}

McpServerBase::register_result McpServerBase::register_prompt(const mbase::McpPromptDescription& in_description, mcp_server_prompt_cb in_cb)
{
    if(!in_description.mName.size())
    {
        return McpServerBase::register_result::INVALID_DESCRIPTION_PARAMS;
    }

    if(!in_cb)
    {
        return McpServerBase::register_result::MISSING_CALLBACK;
    }

    this->acquire_synchronizer();
    
    const mbase::string& promptName = in_description.mName;
    mbase::McpPromptFeature* tmpPromptFeature = this->get_prompt_feature(promptName);
    register_result rgrRes = McpServerBase::register_result::LIST_CHANGED;
    if(!tmpPromptFeature)
    {
        // means the prompt doesn't exist
        McpPromptFeature promptFeature;
        promptFeature.update_prompt_description(in_description, in_cb);
        mPromptMap[promptName] = promptFeature;
        rgrRes = McpServerBase::register_result::SUCCESS;
    }

    else
    {
        // means the prompt will be modified
        tmpPromptFeature->update_prompt_description(in_description, in_cb);
        send_prompt_list_changed_notification();
    }

    this->release_synchronizer();
    return rgrRes;
}

McpServerBase::register_result McpServerBase::register_resource(const mbase::McpResourceDescription& in_description, mcp_server_resource_cb in_cb)
{
    if(!in_description.mName.size())
    {
        return McpServerBase::register_result::INVALID_DESCRIPTION_PARAMS;
    }

    if(!in_cb)
    {
        return McpServerBase::register_result::MISSING_CALLBACK;
    }

    this->acquire_synchronizer();

    const mbase::string& resourceName = in_description.mUri;
    mbase::McpResourceFeature* tmpResourceFeature = this->get_resource_feature(resourceName);
    McpServerBase::register_result rgrRes = McpServerBase::register_result::LIST_CHANGED;

    if(!tmpResourceFeature)
    {
        // means the resource doesn't exist
        McpResourceFeature resourceFeature;
        resourceFeature.update_resource_description(in_description, in_cb);
        mResourceMap[resourceName] = resourceFeature;
        rgrRes = McpServerBase::register_result::SUCCESS;
    }

    else
    {
        // means the resource will be modified
        tmpResourceFeature->update_resource_description(in_description, in_cb);
        send_resource_list_changed_notification();
        send_resource_updated_notification(resourceName);
    }

    this->release_synchronizer();
    return rgrRes;
}

GENERIC McpServerBase::register_client(McpServerClient* in_client)
{
    mClientListMutex.acquire();
    mConnectedClients.push_back(in_client);
    mClientListMutex.release();
}

GENERIC McpServerBase::unregister_client(McpServerClient* in_client)
{
    mClientListMutex.acquire();
    for(mbase::vector<McpServerClient*>::iterator It = mConnectedClients.begin(); It != mConnectedClients.end(); ++It)
    {
        if(*It == in_client)
        {
            mConnectedClients.erase(It);
            break;
        }
    }
    mClientListMutex.release();
}

GENERIC McpServerBase::update()
{
    this->default_update_method();
}

GENERIC McpServerBase::send_prompt_list_changed_notification()
{
    if(mTransportMethod == mbase::mcp_transport_method::HTTP_STREAMBLE){ return; }

    mClientListMutex.acquire();
    for(mbase::McpServerClient* currentClient : mConnectedClients)
    {
        if(currentClient->is_initialized())
        {
            currentClient->send_mcp_payload(mbase::mcp_generate_notification("notifications/prompts/list_changed"));
        }
    }
    mClientListMutex.release();
}

GENERIC McpServerBase::send_resource_list_changed_notification()
{
    if(mTransportMethod == mbase::mcp_transport_method::HTTP_STREAMBLE){ return; }

    mClientListMutex.acquire();
    for(mbase::McpServerClient* currentClient : mConnectedClients)
    {
        if(currentClient->is_initialized())
        {
            currentClient->send_mcp_payload(mbase::mcp_generate_notification("notifications/resources/list_changed"));
        }
    }
    mClientListMutex.release();
}

GENERIC McpServerBase::send_tool_list_changed_notification()
{
    if(mTransportMethod == mbase::mcp_transport_method::HTTP_STREAMBLE){ return; }

    mClientListMutex.acquire();
    for(mbase::McpServerClient* currentClient : mConnectedClients)
    {
        if(currentClient->is_initialized())
        {
            currentClient->send_mcp_payload(mbase::mcp_generate_notification("notifications/tools/list_changed"));
        }
    }
    mClientListMutex.release();
}

GENERIC McpServerBase::send_resource_updated_notification(const mbase::string& in_uri)
{
    if(mTransportMethod == mbase::mcp_transport_method::HTTP_STREAMBLE){ return; }
    
    mClientListMutex.acquire();
    mbase::Json uriJson;
    uriJson["uri"] = in_uri;
    for(mbase::McpServerClient* currentClient : mConnectedClients)
    {
        if(currentClient->is_initialized())
        {
            if(currentClient->is_subscribed(in_uri))
            {
                currentClient->send_mcp_payload(mbase::mcp_generate_notification("notifications/resources/updated", uriJson));
            }
        }
    }
    mClientListMutex.release();
}

GENERIC McpServerBase::default_client_init(mbase::McpServerClient* in_client)
{
    in_client->on_empty_processed_t();
}

GENERIC McpServerBase::default_client_root_list_changed(mbase::McpServerClient* in_client)
{
    in_client->on_empty_processed_t();
}

GENERIC McpServerBase::default_cancellation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_reason)
{
    in_client->on_empty_processed_t();
    mbase::lock_guard featureRequestSync(mFeatureRequestVectorSync);
    for(mbase::McpFeatureRequest& featureRequest : mSyncFeatureRequests)
    {
        if(in_msgid.isString() && featureRequest.mMessageId.isString())
        {
            if(in_msgid.getString() == featureRequest.mMessageId.getString())
            {
                featureRequest.mIsCancelled = true;
                return;
            }
        }
        else if(in_msgid.isLong() && featureRequest.mMessageId.isLong())
        {
            if(in_msgid.getLong() == featureRequest.mMessageId.getLong())
            {
                featureRequest.mIsCancelled = true;
                return;
            }
        }
    }
}

GENERIC McpServerBase::default_ping_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid)
{
    mbase::Json pingObject;
    pingObject["jsonrpc"] = "2.0";
    pingObject["id"] = in_msgid;
    pingObject["result"].setObject();
    in_client->send_mcp_payload(pingObject.toString() + '\n');
}

GENERIC McpServerBase::default_logging_set_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, mcp_log_levels in_log_level)
{
    in_client->set_log_level(in_log_level);
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid));
}

GENERIC McpServerBase::default_resource_subscribe_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri)
{
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid));
}

GENERIC McpServerBase::default_resource_unsubscribe_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri)
{
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid));
}

GENERIC McpServerBase::default_prompt_compilation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_prompt, const mbase::string& in_argument_name, const mbase::string& in_argument_value)
{
    acquire_synchronizer();
    mbase::McpPromptFeature* promptFeature = this->get_prompt_feature(in_prompt);
    mbase::Json defaultCompletionResponse;
    defaultCompletionResponse["completion"]["values"].setArray();
    defaultCompletionResponse["completion"]["total"] = 0;
    defaultCompletionResponse["completion"]["hasMore"] = false;

    if(in_argument_value.size())
    {
        if(promptFeature)
        {
            const mbase::McpPromptDescription& promptDesc = promptFeature->get_prompt_description();
            for(const mbase::McpPromptArgument& promptArgument : promptDesc.mArguments)
            {
                if(in_argument_name == promptArgument.mArgumentName)
                {
                    mbase::vector<mbase::Json>& completionObjects = defaultCompletionResponse["completion"]["values"].getArray();
                    I32 totalCompletedStrings = 0;
                    for(const mbase::string& completionString : promptArgument.mCompletionStrings)
                    {
                        if(completionString.starts_with(in_argument_value.c_str()))
                        {
                            if(totalCompletedStrings < 100)
                            {
                                completionObjects.push_back(completionString);
                            }   
                            totalCompletedStrings++;
                        }
                    }
                    
                    defaultCompletionResponse["completion"]["total"] = totalCompletedStrings;

                    if(totalCompletedStrings > completionObjects.size())
                    {
                        defaultCompletionResponse["completion"]["hasMore"] = true;
                    }
                }
            }
        }
    }
    release_synchronizer();
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, defaultCompletionResponse));
}

GENERIC McpServerBase::default_list_tool_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination)
{
    mbase::Json toolObject;
    toolObject["tools"].setArray();
    I32 paginationIndex = 0;
    if(in_pagination.size())
    {
        // pagination format: lt-1, lt-2, lt-3 ... lt-n
        mbase::string paginatorString = in_pagination;
        mbase::vector<mbase::string> paginationSplit;
        paginatorString.split("-", paginationSplit);
        if(paginationSplit.size() != 2)
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, toolObject));
            return;
        }

        if(paginationSplit[0] != "lt")
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, toolObject));
            return;
        }
        if(!mbase::string::is_integer(paginationSplit[1].c_str()))
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, toolObject));
            return;
        }
        paginationIndex = paginationSplit[1].to_i32();
        if(paginationIndex < 0)
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, toolObject));
            return;
        }
        paginationIndex *= mPaginationMin;
    }

    this->acquire_synchronizer();

    mbase::unordered_map<mbase::string, mbase::McpToolFeature>::iterator toolFeatureIterator = mToolMap.begin();
    for(I32 i = 0; i < paginationIndex; i++)
    {
        if(toolFeatureIterator == mToolMap.end())
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, toolObject));
            this->release_synchronizer();
            return;
        }
        toolFeatureIterator++;
    }
    mbase::vector<mbase::Json>& toolsArray = toolObject["tools"].getArray();

    mbase::I32 contentCounter = 0;

    for(;toolFeatureIterator != mToolMap.end(); toolFeatureIterator++)
    {
        if(contentCounter == mPaginationMin)
        {
            if(!paginationIndex)
            {
                toolObject["nextCursor"] = "lt-1";
            }
            else
            {
                toolObject["nextCursor"] = mbase::string::from_format("lt-%d", (paginationIndex / mPaginationMin) + 1);
            }
            break;
        }
        mbase::Json toolDesc;
        const mbase::McpToolDescription& toolDescription = toolFeatureIterator->second.get_tool_description();
        toolDesc["name"] = toolDescription.mName;
        toolDesc["description"] = toolDescription.mDescription;
        toolDesc["inputSchema"].setObject();

        mbase::Json& inputSchema = toolDesc["inputSchema"];
        inputSchema["type"] = "object";
        inputSchema["properties"].setObject();
        inputSchema["required"].setArray();

        mbase::Json& propertyValues = inputSchema["properties"];
        mbase::vector<mbase::Json>& requiredArguments = inputSchema["required"].getArray();
        for(const mbase::McpToolArgument& toolArgument: toolDescription.mArguments)
        {
            mbase::string argumentTypeString;
            switch (toolArgument.mArgType)
            {
            case mbase::McpValueType::STRING:
                argumentTypeString = "string";
                break;
            
            case mbase::McpValueType::NUMBER:
                argumentTypeString = "number";
                break;
            
            case mbase::McpValueType::ARRAY:
                argumentTypeString = "array";
                break;

            case mbase::McpValueType::JSON:
                argumentTypeString = "object";
                break;

            case mbase::McpValueType::BOOL:
                argumentTypeString = "boolean";
                break;

            default:
                argumentTypeString = "string";
                break;
            }
            propertyValues[toolArgument.mArgumentName]["description"] = toolArgument.mDescription;
            propertyValues[toolArgument.mArgumentName]["type"] = argumentTypeString;

            if(toolArgument.mIsRequired)
            {
                requiredArguments.push_back(toolArgument.mArgumentName);
            }
        }
        ++contentCounter;
        toolsArray.push_back(toolDesc);
    }

    this->release_synchronizer();
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, toolObject));
}

GENERIC McpServerBase::default_list_prompt_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination)
{
    mbase::Json promptsObject;
    promptsObject["prompts"].setArray();
    I32 paginationIndex = 0;
    if(in_pagination.size())
    {
        // pagination format: lp-1, lp-2, lp-3 ... lp-n
        mbase::string paginatorString = in_pagination;
        mbase::vector<mbase::string> paginationSplit;
        paginatorString.split("-", paginationSplit);
        if(paginationSplit.size() != 2)
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, promptsObject));
            return;
        }

        if(paginationSplit[0] != "lp")
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, promptsObject));
            return;
        }
        if(!mbase::string::is_integer(paginationSplit[1].c_str()))
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, promptsObject));
            return;
        }
        paginationIndex = paginationSplit[1].to_i32();
        if(paginationIndex < 0)
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, promptsObject));
            return;
        }
        paginationIndex *= mPaginationMin;
    }
    this->acquire_synchronizer();
    mbase::unordered_map<mbase::string, mbase::McpPromptFeature>::iterator promptFeatureIterator = mPromptMap.begin();
    for(I32 i = 0; i < paginationIndex; i++)
    {
        if(promptFeatureIterator == mPromptMap.end())
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, promptsObject));
            this->release_synchronizer();
            return;
        }
        promptFeatureIterator++;
    }

    mbase::vector<mbase::Json>& promptsArray = promptsObject["prompts"].getArray();
    
    mbase::I32 contentCounter = 0;

    for(; promptFeatureIterator != mPromptMap.end(); promptFeatureIterator++)
    {   
        if(contentCounter == mPaginationMin)
        {
            if(!paginationIndex)
            {
                promptsObject["nextCursor"] = "lp-1";
            }
            else
            {
                promptsObject["nextCursor"] = mbase::string::from_format("lp-%d", (paginationIndex / mPaginationMin) + 1);
            }
            break;
        }
        mbase::Json promptDesc;
        const mbase::McpPromptDescription& promptDescription = promptFeatureIterator->second.get_prompt_description();
        promptDesc["name"] = promptDescription.mName;
        promptDesc["description"] = promptDescription.mDescription;
        promptDesc["arguments"].setArray();
        mbase::vector<mbase::Json>& argumentsArray = promptDesc["arguments"].getArray();
        for(const mbase::McpPromptArgument& tmpArgument: promptDescription.mArguments)
        {
            mbase::Json promptArgument;
            promptArgument["name"] = tmpArgument.mArgumentName;
            promptArgument["description"] = tmpArgument.mDescription;
            promptArgument["required"] = tmpArgument.mIsRequired;
            argumentsArray.push_back(promptArgument);
        }
        ++contentCounter;
        promptsArray.push_back(promptDesc);
    }

    this->release_synchronizer();
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, promptsObject));
}

GENERIC McpServerBase::default_list_resource_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination)
{
    mbase::Json resourcesObject;
    resourcesObject["resources"].setArray();
    I32 paginationIndex = 0;
    if(in_pagination.size())
    {
        // pagination format: lr-1, lr-2, lr-3 ... lr-n
        mbase::string paginatorString = in_pagination;
        mbase::vector<mbase::string> paginationSplit;
        paginatorString.split("-", paginationSplit);
        if(paginationSplit.size() != 2)
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, resourcesObject));
            return;
        }

        if(paginationSplit[0] != "lr")
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, resourcesObject));
            return;
        }
        if(!mbase::string::is_integer(paginationSplit[1].c_str()))
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, resourcesObject));
            return;
        }
        paginationIndex = paginationSplit[1].to_i32();
        if(paginationIndex < 0)
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, resourcesObject));
            return;
        }
        paginationIndex *= mPaginationMin;
    }
    this->acquire_synchronizer();
    mbase::unordered_map<mbase::string, mbase::McpResourceFeature>::iterator resourceFeatureIterator = mResourceMap.begin();
    for(I32 i = 0; i < paginationIndex; i++)
    {
        if(resourceFeatureIterator == mResourceMap.end())
        {
            in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, resourcesObject));
            this->release_synchronizer();
            return;
        }
        resourceFeatureIterator++;
    }

    mbase::vector<mbase::Json>& resourcesArray = resourcesObject["resources"].getArray();
    mbase::I32 contentCounter = 0;
    for(; resourceFeatureIterator != mResourceMap.end(); resourceFeatureIterator++)
    {
        if(contentCounter == mPaginationMin)
        {
            if(!paginationIndex)
            {
                resourcesObject["nextCursor"] = "lr-1";
            }
            else
            {
                resourcesObject["nextCursor"] = mbase::string::from_format("lr-%d", (paginationIndex / mPaginationMin) + 1);
            }
            break;
        }
        mbase::Json resourceDesc;
        const mbase::McpResourceDescription& resourceDescription = resourceFeatureIterator->second.get_resource_description();

        resourceDesc["uri"] = resourceDescription.mUri;
        resourceDesc["name"] = resourceDescription.mName;
        resourceDesc["description"] = resourceDescription.mDescription;
        resourceDesc["mimeType"] = resourceDescription.mMimeType;
        ++contentCounter;
        resourcesArray.push_back(resourceDesc);
    }
    this->release_synchronizer();
    in_client->send_mcp_payload(mbase::mcp_generate_response(in_msgid, resourcesObject));
}

GENERIC McpServerBase::default_tool_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpToolFeature* in_tool, McpMessageMap& in_arguments)
{
    for(const mbase::McpToolArgument tmpArgument : in_tool->get_tool_description().mArguments)
    {
        mbase::McpMessageMap::iterator It = in_arguments.find(tmpArgument.mArgumentName);
        if(It == in_arguments.end() && tmpArgument.mIsRequired)
        {
            // err
            // means required argument is missing
            mbase::string requiredArgument = "Required argument is missing: " + tmpArgument.mArgumentName;
            in_client->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, requiredArgument));
            return;
        }
        
        mbase::string argumentError = "Argument '" + tmpArgument.mArgumentName +"' must be of type ";

        switch (tmpArgument.mArgType)
        {
        case mbase::McpValueType::NUMBER:
            if(!std::holds_alternative<I64>(It->second) && !std::holds_alternative<F64>(It->second))
            {
                argumentError += "number";
                in_client->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, argumentError));
                return;
            }
            break;
        
        case mbase::McpValueType::BOOL:
            if(!std::holds_alternative<bool>(It->second))
            {
                argumentError += "boolean";
                in_client->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, argumentError));
                return;
            }
            break;

        case mbase::McpValueType::STRING:
            if(!std::holds_alternative<mbase::string>(It->second))
            {
                argumentError += "string";
                in_client->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, argumentError));
                return;
            }
            break;
        
        case mbase::McpValueType::ARRAY:
            if(!std::holds_alternative<mbase::vector<mbase::Json>>(It->second))
            {
                argumentError += "array";
                in_client->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, argumentError));
                return;
            }
            break;
        
        case mbase::McpValueType::JSON:
            if(!std::holds_alternative<std::map<mbase::string, mbase::Json>>(It->second))
            {
                argumentError += "object";
                in_client->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, argumentError));
                return;
            }
            break;
        }
    }

    mFeatureRequestVectorSync.acquire();

    mbase::McpFeatureRequest newFeatureRequest;
    newFeatureRequest.mFeatureType = mbase::feature_type::TOOL;
    newFeatureRequest.mRequestOwner = in_client;
    newFeatureRequest.mMessageId = in_msgid;
    newFeatureRequest.mProgressId = in_progress_id;
    newFeatureRequest.toolFeature = in_tool;
    newFeatureRequest.mMessageMap = std::move(in_arguments);
    mSyncFeatureRequests.push_back(std::move(newFeatureRequest));

    mFeatureRequestVectorSync.release();
}

GENERIC McpServerBase::default_prompt_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpPromptFeature* in_prompt, McpMessageMap& in_arguments)
{
    for(const mbase::McpPromptArgument tmpArgument : in_prompt->get_prompt_description().mArguments)
    {
        mbase::McpMessageMap::iterator It = in_arguments.find(tmpArgument.mArgumentName);
        if(It == in_arguments.end() && tmpArgument.mIsRequired)
        {
            // err
            // means required argument is missing
            mbase::string requiredArgument = "Required argument is missing: " + tmpArgument.mArgumentName;
            in_client->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, requiredArgument));
            return;
        }

        if(!std::holds_alternative<mbase::string>(It->second))
        {
            // err
            // prompt arguments must be string
            in_client->send_mcp_payload(mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INVALID_PARAMS, "Prompt arguments must be string"));
            return;
        }
    }

    mFeatureRequestVectorSync.acquire();

    mbase::McpFeatureRequest newFeatureRequest;
    newFeatureRequest.mFeatureType = mbase::feature_type::PROMPT;
    newFeatureRequest.mRequestOwner = in_client;
    newFeatureRequest.mMessageId = in_msgid;
    newFeatureRequest.mProgressId = in_progress_id;
    newFeatureRequest.promptFeature = in_prompt;
    newFeatureRequest.mMessageMap = std::move(in_arguments);
    mSyncFeatureRequests.push_back(std::move(newFeatureRequest));

    mFeatureRequestVectorSync.release();
}

GENERIC McpServerBase::default_resource_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpResourceFeature* in_resource)
{
    mFeatureRequestVectorSync.acquire();
    
    mbase::McpFeatureRequest newFeatureRequest;
    newFeatureRequest.mFeatureType = mbase::feature_type::RESOURCE;
    newFeatureRequest.mRequestOwner = in_client;
    newFeatureRequest.mMessageId = in_msgid;
    newFeatureRequest.mProgressId = in_progress_id;
    newFeatureRequest.resourceFeature = in_resource;
    mSyncFeatureRequests.push_back(std::move(newFeatureRequest));

    mFeatureRequestVectorSync.release();    
}

GENERIC McpServerBase::default_update_method()
{
    mClientListMutex.acquire();
    for(mbase::McpServerClient* currentClient : mConnectedClients)
    {
        currentClient->update();
    }
    mClientListMutex.release();

    if(mSyncFeatureRequests.size())
    {
        mFeatureRequestVectorSync.acquire();
        mbase::vector<McpFeatureRequest> syncFeatureRequests = std::move(mSyncFeatureRequests);
        mSyncFeatureRequests = mbase::vector<McpFeatureRequest>();
        mFeatureRequestVectorSync.release();

        for(const mbase::McpFeatureRequest& tmpRequest : syncFeatureRequests)
        {
            if(tmpRequest.mIsCancelled)
            {
                continue;
            }

            if(tmpRequest.mFeatureType == mbase::feature_type::PROMPT)
            {
                mbase::vector<McpResponsePrompt> callResult = tmpRequest.promptFeature->get_prompt_cb()(tmpRequest.mRequestOwner, tmpRequest.mMessageMap, tmpRequest.mProgressId);
                send_prompt_call_result(tmpRequest.mRequestOwner, tmpRequest.mMessageId, tmpRequest.promptFeature, callResult);
            }

            else if(tmpRequest.mFeatureType == mbase::feature_type::RESOURCE)
            {
                McpResponseResource callResult = tmpRequest.resourceFeature->get_resource_cb()(tmpRequest.mRequestOwner, tmpRequest.mProgressId);
                send_resource_call_result(tmpRequest.mRequestOwner, tmpRequest.mMessageId, tmpRequest.resourceFeature, callResult);
            }

            else if(tmpRequest.mFeatureType == mbase::feature_type::TOOL)
            {
                McpResponseTool callResult = tmpRequest.toolFeature->get_tool_cb()(tmpRequest.mRequestOwner, tmpRequest.mMessageMap, tmpRequest.mProgressId);
                send_tool_call_result(tmpRequest.mRequestOwner, tmpRequest.mMessageId, tmpRequest.toolFeature, callResult);
            }
        }
    }
}

MBASE_END