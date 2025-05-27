#ifndef MBASE_MCP_CLIENT_CALLBACKS_H
#define MBASE_MCP_CLIENT_CALLBACKS_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/mcp/mcp_server_responses.h> // Response objects
#include <mbase/mcp/mcp_server_descriptions.h> // Description objects

MBASE_BEGIN

class McpClientBase;
class McpServerStateBase;

using mcp_init_cb = std::function<GENERIC(const I32&, McpClientBase*, McpServerStateBase*)>;
using mcp_read_resource_cb = std::function<GENERIC(const I32&, McpClientBase*, mbase::vector<McpResponseResource>&&)>;
using mcp_get_prompt_cb = std::function<GENERIC(const I32&, McpClientBase*, const mbase::string&, mbase::vector<McpResponsePrompt>&&)>;
using mcp_tool_call_cb = std::function<GENERIC(const I32&, McpClientBase*, mbase::vector<McpResponseTool>&&, bool /* is error? */)>; 
using mcp_list_resources_cb = std::function<GENERIC(const I32&, McpClientBase*, mbase::vector<McpResourceDescription>&&, const mbase::string&)>;
using mcp_list_prompts_cb = std::function<GENERIC(const I32&, McpClientBase*, mbase::vector<McpPromptDescription>&&, const mbase::string&)>;
using mcp_list_tools_cb = std::function<GENERIC(const I32&, McpClientBase*, mbase::vector<McpToolDescription>&&, const mbase::string&)>;
using mcp_prompt_compilation_cb = std::function<GENERIC(const I32&, McpClientBase*, const McpPromptCompilationResult&)>;
using mcp_empty_cb = std::function<GENERIC(const I32&, McpClientBase*)>;

using mcp_response_callback = std::variant<
mcp_init_cb,
mcp_read_resource_cb, 
mcp_get_prompt_cb, 
mcp_tool_call_cb, 
mcp_list_resources_cb, 
mcp_list_prompts_cb, 
mcp_list_tools_cb, 
mcp_prompt_compilation_cb, 
mcp_empty_cb>;

struct McpServerRequestState {
    mcp_response_callback responseCallback;
    mbase::string mMessageId;
    mbase::I64 mTimeoutInSeconds = 10;
    mbase::I64 mAttemptCount = 0;
};

struct McpServerResponseObject {
    mcp_response_callback responseCallback;
    mbase::Json mResponseParams;
    I32 mErrorCode = MBASE_MCP_SUCCESS;
};

MBASE_END

#endif // MBASE_MCP_CLIENT_CALLBACKS_H