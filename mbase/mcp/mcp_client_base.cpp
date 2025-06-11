#include <mbase/mcp/mcp_client_base.h>
#include <mbase/mcp/mcp_client_server_state.h>
#include <mbase/mcp/mcp_packet_parsing.h>

MBASE_BEGIN

McpClientBase::McpClientBase(
    const mbase::string& in_client_name,
    const mbase::string& in_client_version,
    const bool& in_sampling_supported
)
{
    mClientName = in_client_name;
    mClientVersion = in_client_version;
    mIsSamplingSupported = in_sampling_supported;
}

const bool& McpClientBase::is_sampling_supported() const noexcept
{
    return mIsSamplingSupported;
}

const mbase::unordered_map<mbase::string, mbase::string>& McpClientBase::get_roots() const noexcept
{
    return mRoots;
}

const mbase::string& McpClientBase::get_client_name() const noexcept
{
    return mClientName;
}

const mbase::string& McpClientBase::get_client_version() const noexcept
{
    return mClientVersion;
}

mbase::vector<McpServerStateBase*>& McpClientBase::get_mcp_servers()
{
    return mMcpServersList;
}

bool McpClientBase::on_server_request_t([[maybe_unused]] McpServerStateBase* in_server, [[maybe_unused]] const mbase::Json& in_msgid, [[maybe_unused]] const mbase::string& in_method, [[maybe_unused]] const mbase::Json& in_params)
{
    return false;
}

GENERIC McpClientBase::on_server_notification_t([[maybe_unused]] McpServerStateBase* in_server, [[maybe_unused]] const mbase::string& in_method, [[maybe_unused]] const mbase::Json& in_params)
{

}

GENERIC McpClientBase::on_server_response_t([[maybe_unused]] McpServerStateBase* in_server, [[maybe_unused]] const mbase::string& in_msgid, [[maybe_unused]] const mbase::Json& in_params)
{

}

GENERIC McpClientBase::on_log_message_t([[maybe_unused]] McpServerStateBase* in_server, [[maybe_unused]] const mbase::McpNotificationLogMessage& in_log_message)
{

}

GENERIC McpClientBase::on_cancellation_t([[maybe_unused]] McpServerStateBase* in_server, [[maybe_unused]] const mbase::McpNotificationCancellationIdStr& in_cancellation)
{

}

GENERIC McpClientBase::on_prompt_list_changed_t([[maybe_unused]] McpServerStateBase* in_server)
{

}

GENERIC McpClientBase::on_resource_list_changed_t([[maybe_unused]] McpServerStateBase* in_server)
{

}

GENERIC McpClientBase::on_tool_list_changed_t([[maybe_unused]] McpServerStateBase* in_server)
{

}

GENERIC McpClientBase::on_resource_updated_t([[maybe_unused]] McpServerStateBase* in_server, [[maybe_unused]] const mbase::string& in_uri)
{

}

GENERIC McpClientBase::on_progress_notification_t([[maybe_unused]] McpServerStateBase* in_server, [[maybe_unused]] const mbase::McpNotificationProgress& in_progress_notif)
{

}

GENERIC McpClientBase::on_sampling_request_t([[maybe_unused]] McpServerStateBase* in_server, const mbase::McpSamplingRequestObject& in_sampling_request)
{
    mSamplingRequestsSync.acquire();
    mSamplingRequestsList.push_back(in_sampling_request);
    mSamplingRequestsSync.release();
}

bool McpClientBase::on_sampling_request([[maybe_unused]] McpServerStateBase* in_server, mbase::McpSamplingRequestObject&& in_sampling_request)
{
    mbase::McpSamplingResult samplingResult;
    samplingResult.mModel = "qwen2";
    samplingResult.mStopReason = "EOT";
    samplingResult.mTextContent = "Hey, how can I help you today?";
    samplingResult.mRole = "assistant";
    samplingResult.mContentType = mbase::mcp_sampling_content_type::TEXT;
    this->send_sampling_result(in_sampling_request, samplingResult);
    return true;
}

GENERIC McpClientBase::add_root(const mbase::string& in_uri, const mbase::string& in_name)
{
    if(!in_uri.size()){return;}
    mRoots[in_uri] = in_name;
    for(McpServerStateBase* currentServer : mMcpServersList)
    {
        currentServer->send_mcp_payload(mbase::mcp_generate_notification("notifications/roots/list_changed"));
    }
}

GENERIC McpClientBase::remove_root(const mbase::string& in_uri)
{
    if(mRoots.find(in_uri) != mRoots.end())
    {
        mRoots.erase(in_uri);
    }

    for(McpServerStateBase* currentServer : mMcpServersList)
    {
        currentServer->send_mcp_payload(mbase::mcp_generate_notification("notifications/roots/list_changed"));
    }
}

GENERIC McpClientBase::register_mcp_server(McpServerStateBase* in_server)
{
    mMcpServersList.push_back(in_server);
}

GENERIC McpClientBase::unregister_mcp_server(McpServerStateBase* in_server)
{
    mbase::vector<McpServerStateBase*>::iterator It = mbase::find(mMcpServersList.begin(), mMcpServersList.end(), in_server);
    if(It != mMcpServersList.end())
    {
        mMcpServersList.erase(It);
    }
}

GENERIC McpClientBase::send_sampling_result(const mbase::McpSamplingRequestObject& in_sampling_request, const mbase::McpSamplingResult& in_result)
{
    mbase::Json msgId = in_sampling_request.mRequestId;
    mbase::Json resultJson;
    resultJson["role"] = in_result.mRole;
    resultJson["model"] = in_result.mModel;
    resultJson["stopReason"] = in_result.mStopReason;

    if(in_result.mContentType == mbase::mcp_sampling_content_type::TEXT)
    {
        resultJson["content"]["type"] = "text";
        resultJson["content"]["text"] = in_result.mTextContent;
    }
    else if(in_result.mContentType == mbase::mcp_sampling_content_type::IMAGE)
    {
        resultJson["content"]["type"] = "image";
        resultJson["content"]["data"] = in_result.mBase64Content;
        resultJson["content"]["mimeType"] = in_result.mMimeType;
    }
    else if(in_result.mContentType == mbase::mcp_sampling_content_type::AUDIO)
    {
        resultJson["content"]["type"] = "audio";
        resultJson["content"]["data"] = in_result.mBase64Content;
        resultJson["content"]["mimeType"] = in_result.mMimeType;
    }
    in_sampling_request.requestOwner->send_mcp_payload(mbase::mcp_generate_response(msgId, resultJson));
}

GENERIC McpClientBase::update()
{
    this->default_update_method();
}

GENERIC McpClientBase::default_update_method()
{
    for(McpServerStateBase* currentServer : mMcpServersList)
    {
        currentServer->update();
    }

    if(mSamplingRequestsList.size())
    {
        mSamplingRequestsSync.acquire();
        mbase::vector<McpSamplingRequestObject> tmpSamplingRequests = std::move(mSamplingRequestsList);
        mSamplingRequestsList = mbase::vector<McpSamplingRequestObject>();
        mSamplingRequestsSync.release();

        for(McpSamplingRequestObject& currentSamplingRequest : tmpSamplingRequests)
        {
            mbase::Json msgId = currentSamplingRequest.mRequestId;
            mbase::McpServerStateBase* ownerServer = currentSamplingRequest.requestOwner;
            if(!this->on_sampling_request(currentSamplingRequest.requestOwner, std::move(currentSamplingRequest)))
            {
                ownerServer->send_mcp_payload(mbase::mcp_generate_error_message(msgId, -1, "User rejected sampling request"));
            }
        }
    }
}

MBASE_END