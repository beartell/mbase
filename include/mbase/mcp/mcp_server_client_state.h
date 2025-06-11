#ifndef MBASE_MCP_SERVER_CLIENT_STATE_H
#define MBASE_MCP_SERVER_CLIENT_STATE_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/framework/logical_processing.h> // Async IO
#include <mbase/mcp/mcp_notifications.h> // Notification objects
#include <mbase/mcp/mcp_server_to_client_requests.h> // Sampling and root request objects

MBASE_BEGIN

class McpServerBase;
class McpServerClient;

class MBASE_API McpServerClient : public mbase::logical_processor {
public:
    friend class McpServerBase;

    McpServerClient(McpServerBase* in_server_instance);
    ~McpServerClient();

    mcp_log_levels get_log_level() const noexcept;
    McpServerBase* get_server_instance() noexcept;
    bool is_subscribed(const mbase::string& in_uri) const noexcept;
    bool is_initializing() const noexcept;
    bool is_initialized() const noexcept;
    bool has_roots() const noexcept;
    bool has_sampling() const noexcept;

    GENERIC on_client_init_request_t(const mbase::Json& in_msgid, mbase::Json& in_params);
    virtual GENERIC on_empty_processed_t(); // if the received message is processed but no response is generated

    GENERIC list_roots(mcp_st_list_roots_cb in_cb);
    GENERIC request_sampling(const McpSamplingRequest& in_params, mcp_st_sampling_cb in_cb);
    GENERIC subscribe(const mbase::string& in_uri);
    GENERIC unsubscribe(const mbase::string& in_uri);
    GENERIC set_progress(const I32& in_progress, const mbase::Json& in_token, const mbase::string& in_message = mbase::string());
    GENERIC set_log_level(mcp_log_levels in_log_level);
    GENERIC send_log(const McpNotificationLogMessage& in_log);
    GENERIC read_mcp_payload(const mbase::string& in_payload);
    GENERIC update() override;
    virtual GENERIC send_mcp_payload(const mbase::string& in_payload) = 0;

protected:
    GENERIC process_mcp_message(mbase::Json& in_message);
    GENERIC process_notification_message(mbase::Json& in_message);
    GENERIC process_request_message(mbase::Json& in_message);
    GENERIC process_response_message(mbase::Json& in_message);
    GENERIC default_update_method();

    McpServerBase* mServerInstance = nullptr;
    mcp_log_levels mLogLevel;
    bool mIsInitializing = false;
    bool mIsClientInitialized = false;
    bool mHasRoots = false;
    bool mHasSampling = false;
    mbase::string mClientName;
    mbase::string mClientVersion;
    mbase::string mClientProtocolVersion;
    mbase::unordered_map<mbase::string, bool> mSubscriptionMap;
    mbase::unordered_map<mbase::string, mbase::McpStClientRequestObject> mRequestObject;
    mbase::vector<mbase::McpStRootsResultObject> mRootsResultObjects;
    mbase::vector<mbase::McpStSamplingResultObject> mSamplingResultObjects;
    mbase::mutex mRootsResultObjectsSync;
    mbase::mutex mSamplingResultObjectsSync;
};

MBASE_END

#endif // MBASE_MCP_SERVER_CLIENT_STATE_H