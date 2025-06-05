#ifndef MBASE_MCP_CLIENT_SERVER_STATE_H
#define MBASE_MCP_CLIENT_SERVER_STATE_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/mcp/mcp_client_callbacks.h> // user provided callbacks
#include <mbase/mcp/mcp_client_arguments.h> // Message maps
#include <mbase/framework/logical_processing.h> // For asynchronous I/O
#include <mbase/framework/timer_loop.h> // For timer and rate limits

MBASE_BEGIN

class McpClientBase;
class McpServerStateBase;

class McpServerRequestTimer : public mbase::time_interval {
public:
    McpServerRequestTimer(McpServerStateBase* in_server);
    GENERIC on_call(user_data in_data) override;
private:
    McpServerStateBase* serverState = nullptr;
};

class MBASE_API McpServerStateBase : public mbase::logical_processor {
public:
    friend class McpServerRequestTimer;

    McpServerStateBase(
        mbase::mcp_transport_method in_method
    );
    const mbase::string& get_protocol_version() const noexcept;
    const mbase::string& get_server_name() const noexcept;
    const mbase::string& get_server_version() const noexcept;
    const mbase::string& get_instructions() const noexcept;
    McpClientBase* get_owner_client() noexcept;
    mbase::mcp_transport_method get_transport_method() const noexcept;

    // true if feature is supported
    bool is_server_initialized() const noexcept;
    bool is_logging_supported() const noexcept;
    bool is_resource_supported() const noexcept;
    bool is_resource_subscription_supported() const noexcept;
    bool is_prompt_supported() const noexcept;
    bool is_tooling_supported() const noexcept;

    // _t denotes that it is called on transport thread
    virtual GENERIC on_empty_processed_t();
    virtual GENERIC on_initialize_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_list_resources_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_list_prompts_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_list_tools_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_read_resource_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_prompt_get_result_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_tool_call_result_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_subscribe_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_unsubscribe_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_set_log_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_ping_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, const mbase::mcp_err_format& in_error);
    virtual GENERIC on_prompt_compilation_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);

    // feature calls
    // returns true if the operation started
    // returns false if the operation is not supported or the parameters are invalid or the server is not initialized
    bool initialize(McpClientBase* in_client, mcp_init_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
    bool list_resources(mcp_list_resources_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT, const mbase::string& in_cursor = mbase::string());
    bool list_prompts(mcp_list_prompts_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT, const mbase::string& in_cursor = mbase::string());
    bool list_tools(mcp_list_tools_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT, const mbase::string& in_cursor = mbase::string());
    bool read_resource(const mbase::string& in_uri, mcp_read_resource_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
    bool get_prompt(const mbase::string& in_prompt_name, mcp_get_prompt_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT, const McpPromptMessageMap& in_arguments = McpPromptMessageMap());
    bool tool_call(const mbase::string& in_tool_name, mcp_tool_call_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT, const McpToolMessageMap& in_arguments = McpToolMessageMap());
    bool read_resource(const mbase::string& in_uri, mcp_read_resource_cb in_cb, mbase::string& out_progress_token, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
    bool get_prompt(const mbase::string& in_prompt_name, mcp_get_prompt_cb in_cb, const I64& in_timeout, const McpPromptMessageMap& in_arguments, mbase::string& out_progress_token);
    bool tool_call(const mbase::string& in_tool_name, mcp_tool_call_cb in_cb, const I64& in_timeout, const McpToolMessageMap& in_arguments, mbase::string& out_progress_token);
    bool subscribe(const mbase::string& in_uri, mcp_empty_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
    bool unsubscribe(const mbase::string& in_uri, mcp_empty_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
    bool set_log(mcp_log_levels in_level, mcp_empty_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
    bool ping(mcp_empty_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
    bool prompt_compilation(mcp_prompt_compilation_cb in_cb, const mbase::string& in_prompt_name, const mbase::string& in_argument_name, const mbase::string& in_argument_value, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);

    GENERIC read_mcp_payload(const mbase::string& in_payload);
    virtual GENERIC send_mcp_payload(const mbase::string& in_payload) = 0;

    GENERIC update() override;
protected:
    mbase::string create_request_state(mcp_response_callback in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
    GENERIC process_mcp_message(mbase::Json& in_message);
    GENERIC process_notification_message(mbase::Json& in_message);
    GENERIC process_request_message(mbase::Json& in_message);
    GENERIC process_response_message(mbase::Json& in_message);
    GENERIC default_initialize_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    GENERIC default_list_resources_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    GENERIC default_list_prompts_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    GENERIC default_list_tools_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    GENERIC default_read_resource_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    GENERIC default_prompt_get_result_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    GENERIC default_tool_call_result_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);
    GENERIC default_subscribe_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, const mbase::mcp_err_format& in_error);
    GENERIC default_unsubscribe_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, const mbase::mcp_err_format& in_error);
    GENERIC default_set_log_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, const mbase::mcp_err_format& in_error);
    GENERIC default_ping_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, const mbase::mcp_err_format& in_error);
    GENERIC default_prompt_compilation_t(McpServerRequestState& in_request, mbase::McpServerResponseObject& in_response_object, mbase::Json& in_response, const mbase::mcp_err_format& in_error);

    mbase::string mProtocolVersion;
    mbase::string mServerName;
    mbase::string mServerVersion;
    mbase::string mInstructions; // optional instructions. see: https://modelcontextprotocol.io/specification/2025-03-26/basic/lifecycle
    bool mIsServerInitialized = false;
    bool mLoggingSupported = false;
    bool mResourceSupported = false;
    bool mResourceSubscriptionSupported = false;
    bool mPromptSupported = false;
    bool mToolingSupported = false;
    McpClientBase* mOwnerClient = nullptr;
    mbase::mcp_transport_method mTransportMethod;
    mbase::vector<McpServerResponseObject> mQueuedResponses;
    mbase::unordered_map<mbase::string, McpServerRequestState> mRequestMap;
    mbase::mutex mRequestMapSync;
    mbase::mutex mQueuedResponsesSync;
    mbase::timer_loop mTimerLoop;
    mbase::McpServerRequestTimer mRequestTimer;
};

MBASE_END

#endif // MBASE_MCP_CLIENT_SERVER_STATE_H