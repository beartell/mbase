#ifndef MBASE_MCP_CLIENT_ARGUMENTS_H
#define MBASE_MCP_CLIENT_ARGUMENTS_H

#include <mbase/mcp/mcp_common.h>

MBASE_BEGIN

using McpPromptMessageArgument = mbase::string;
using McpPromptMessageMap = mbase::unordered_map<mbase::string, McpPromptMessageArgument>;
using McpToolMessageArgument = std::variant<I64, F64, bool, mbase::string, mbase::vector<mbase::Json>, std::map<mbase::string, mbase::Json>>;
using McpToolMessageMap = mbase::unordered_map<mbase::string, McpToolMessageArgument>;

struct McpServerStdioInit {
    mbase::string mServerName;
    mbase::string mCommand;
    mbase::vector<mbase::string> mArguments = mbase::vector<mbase::string>();
    mbase::unordered_map<mbase::string, mbase::string> mEnvironmentVariables;
};

struct McpServerHttpInit {
    mbase::string mHostname;
    mbase::string mMcpEndpoint = "/mcp";
    mbase::string mApiKey;
};

MBASE_END

#endif // MBASE_MCP_CLIENT_ARGUMENTS_H