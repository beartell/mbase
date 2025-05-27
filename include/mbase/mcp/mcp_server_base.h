#ifndef MBASE_MCP_SERVER_BASE_H
#define MBASE_MCP_SERVER_BASE_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/framework/logical_processing.h> // Async IO
#include <mbase/mcp/mcp_server_features.h> // Feature objects, Feature request object
#include <mbase/mcp/mcp_server_client_state.h> // McpServerClient

MBASE_BEGIN

class McpServerBase;
/* 
_t denotes that the method is being called on transport thread 
so make sure to protect your resources when writing a middleware
*/

class MBASE_API McpServerBase : public mbase::logical_processor {
public:
    enum class register_result : I32 {
        SUCCESS, // if the registration is succesful
        LIST_CHANGED, // if the feature is modified, list changed notification message will be sent to the client
        INVALID_DESCRIPTION_PARAMS, // if the description call is invalid
        MISSING_CALLBACK // if the callback is missing
    };

    McpServerBase(const mbase::string& in_server_name, const mbase::string& in_version_string, mbase::mcp_transport_method in_method);
    ~McpServerBase();

    mbase::McpToolFeature* get_tool_feature(const mbase::string& in_name) const noexcept;
    mbase::McpPromptFeature* get_prompt_feature(const mbase::string& in_name) const noexcept;
    mbase::McpResourceFeature* get_resource_feature(const mbase::string& in_name) const noexcept;
    mbase::mcp_transport_method get_transport_method() const noexcept;
    const mbase::string& get_server_name() const noexcept;
    const mbase::string& get_server_version() const noexcept;
    const I32& get_pagination_minimum() const noexcept;

    GENERIC set_pagination_min_content(const I32& in_pagination_min) noexcept;

    // Fundamental
    virtual bool on_client_request_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params);
    virtual GENERIC on_client_notification_t(mbase::McpServerClient* in_client, const mbase::string& in_method, const mbase::Json& in_params);
    virtual GENERIC on_client_response_t(mbase::McpServerClient* in_client, const mbase::string& in_msgid, const mbase::Json& in_params);

    // Client notifications
    virtual GENERIC on_client_init(mbase::McpServerClient* in_client);
    virtual GENERIC on_roots_list_changed(mbase::McpServerClient* in_client);

    // Utility callbacks
    virtual GENERIC on_cancellation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_reason = mbase::string());
    virtual GENERIC on_ping_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid);
    virtual GENERIC on_logging_set_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, mcp_log_levels in_log_level);
    virtual GENERIC on_resource_subscribe(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri);
    virtual GENERIC on_resource_unsubscribe(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri);
    virtual GENERIC on_prompt_compilation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_prompt, const mbase::string& in_argument_name, const mbase::string& in_argument_value); // implemented

    // List callbacks
    virtual GENERIC on_list_tool_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string()); // implemented
    virtual GENERIC on_list_prompt_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string()); // implemented
    virtual GENERIC on_list_resource_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string()); // implemented

    // Invocation callbacks
    virtual GENERIC on_tool_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpToolFeature* in_tool, McpMessageMap& in_arguments); // implemented
    virtual GENERIC on_prompt_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpPromptFeature* in_prompt, McpMessageMap& in_arguments); // implemented
    virtual GENERIC on_resource_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpResourceFeature* in_resource); // implemented

    // Feature registration
    register_result register_tool(const mbase::McpToolDescription& in_description, mcp_server_tool_cb in_cb);
    register_result register_prompt(const mbase::McpPromptDescription& in_description, mcp_server_prompt_cb in_cb);
    register_result register_resource(const mbase::McpResourceDescription& in_description, mcp_server_resource_cb in_cb);

    // Client management
    GENERIC register_client(McpServerClient* in_client);
    GENERIC unregister_client(McpServerClient* in_client);

    // internal calls, do not manually call them!
    GENERIC send_prompt_call_result(McpServerClient* in_client, const mbase::Json& in_msgid, McpPromptFeature* in_feature, mbase::vector<McpResponsePrompt>& in_result);
    GENERIC send_resource_call_result(McpServerClient* in_client, const mbase::Json& in_msgid, McpResourceFeature* in_feature, McpResponseResource& in_result);
    GENERIC send_tool_call_result(McpServerClient* in_client, const mbase::Json& in_msgid, McpToolFeature* in_feature, McpResponseTool& in_result);

    // updates the server state
    GENERIC update() override;
protected:
    friend class McpServerTimeInterval;
    friend class McpServerClient;

    // default behavior of the callbacks
    // the reason those defaults exists is that it allows user to overwrite the callbacks.
    GENERIC default_client_init(mbase::McpServerClient* in_client);
    GENERIC default_client_root_list_changed(mbase::McpServerClient* in_client);
    GENERIC default_cancellation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_reason = mbase::string());
    GENERIC default_ping_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid);
    GENERIC default_logging_set_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, mcp_log_levels in_log_level);
    GENERIC default_resource_subscribe_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri);
    GENERIC default_resource_unsubscribe_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri);
    GENERIC default_prompt_compilation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_prompt, const mbase::string& in_argument_name, const mbase::string& in_argument_value);
    GENERIC default_list_tool_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string());
    GENERIC default_list_prompt_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string());
    GENERIC default_list_resource_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string());
    GENERIC default_tool_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpToolFeature* in_tool, McpMessageMap& in_arguments);
    GENERIC default_prompt_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpPromptFeature* in_prompt, McpMessageMap& in_arguments);
    GENERIC default_resource_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpResourceFeature* in_resource);
    GENERIC default_update_method();

    GENERIC send_prompt_list_changed_notification();
    GENERIC send_resource_list_changed_notification();
    GENERIC send_tool_list_changed_notification();
    GENERIC send_resource_updated_notification(const mbase::string& in_uri);

    mbase::I32 mPaginationMin;
    mbase::vector<mbase::McpServerClient*> mConnectedClients;
    mbase::vector<mbase::McpFeatureRequest> mSyncFeatureRequests;

    mbase::unordered_map<mbase::string, mbase::McpToolFeature> mToolMap;
    mbase::unordered_map<mbase::string, mbase::McpPromptFeature> mPromptMap;
    mbase::unordered_map<mbase::string, mbase::McpResourceFeature> mResourceMap;

    mbase::mutex mClientListMutex;
    mbase::mutex mFeatureRequestVectorSync;
    mbase::string mServerName;
    mbase::string mServerVersion;
    mbase::mcp_transport_method mTransportMethod;
};

MBASE_END

#endif // MBASE_MCP_SERVER_BASE_H