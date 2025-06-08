#ifndef MBASE_MCP_CLIENT_BASE_H
#define MBASE_MCP_CLIENT_BASE_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/mcp/mcp_notifications.h> // notification objects
#include <mbase/mcp/mcp_server_to_client_requests.h> // McpSamplingRequestObject, McpSamplingResult

MBASE_BEGIN

class McpServerStateBase;

// _t denotes that it is called on transport thread
// in order to prevent race conditions, use proper synchronization techniques
class MBASE_API McpClientBase {
public:
    McpClientBase(
        const mbase::string& in_client_name,
        const mbase::string& in_client_version,
        const bool& in_sampling_supported = false
    );

    const bool& is_sampling_supported() const noexcept;
    const mbase::unordered_map<mbase::string, mbase::string>& get_roots() const noexcept;
    const mbase::string& get_client_name() const noexcept;
    const mbase::string& get_client_version() const noexcept;
    mbase::vector<McpServerStateBase*>& get_mcp_servers();

    // Request, notif, response callbacks for writing custom protocol methods
    // if it returns true, user will own the request
    virtual bool on_server_request_t(McpServerStateBase* in_server, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params);
    virtual GENERIC on_server_notification_t(McpServerStateBase* in_server, const mbase::string& in_method, const mbase::Json& in_params);
    virtual GENERIC on_server_response_t(McpServerStateBase* in_server, const mbase::string& in_msgid, const mbase::Json& in_params);

    // Notifications
    virtual GENERIC on_log_message_t(McpServerStateBase* in_server, const mbase::McpNotificationLogMessage& in_log_message);
    virtual GENERIC on_cancellation_t(McpServerStateBase* in_server, const mbase::McpNotificationCancellationIdStr& in_cancellation);
    virtual GENERIC on_prompt_list_changed_t(McpServerStateBase* in_server);
    virtual GENERIC on_resource_list_changed_t(McpServerStateBase* in_server);
    virtual GENERIC on_tool_list_changed_t(McpServerStateBase* in_server);
    virtual GENERIC on_resource_updated_t(McpServerStateBase* in_server, const mbase::string& in_uri);
    virtual GENERIC on_progress_notification_t(McpServerStateBase* in_server, const mbase::McpNotificationProgress& in_progress_notif);

    // Sampling request callbacks
    // firstly, the on_sampling_request_t is called on the transport thread
    // secondly, the on_sampling_request is called on the application thread
    // if the user returns false on on_sampling_request, the sampling rejected response will return to the server
    GENERIC on_sampling_request_t(McpServerStateBase* in_server, const mbase::McpSamplingRequestObject& in_sampling_request);
    virtual bool on_sampling_request(McpServerStateBase* in_server, mbase::McpSamplingRequestObject&& in_sampling_request);

    GENERIC add_root(const mbase::string& in_uri, const mbase::string& in_name);
    GENERIC remove_root(const mbase::string& in_uri);
    GENERIC register_mcp_server(McpServerStateBase* in_server);
    GENERIC unregister_mcp_server(McpServerStateBase* in_server);
    GENERIC send_sampling_result(const mbase::McpSamplingRequestObject& in_sampling_request, const mbase::McpSamplingResult& in_result);
    virtual GENERIC update();
private:
    GENERIC default_update_method();

    mbase::unordered_map<mbase::string, mbase::string> mRoots; // key: uri, val: name
    mbase::vector<McpServerStateBase*> mMcpServersList;
    mbase::vector<McpSamplingRequestObject> mSamplingRequestsList;
    mbase::mutex mSamplingRequestsSync;
    mbase::string mClientName;
    mbase::string mClientVersion;
    bool mIsSamplingSupported;
};

MBASE_END

#endif // MBASE_MCP_CLIENT_BASE_H