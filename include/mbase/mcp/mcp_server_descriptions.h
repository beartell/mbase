#ifndef MBASE_MCP_SERVER_DESCRIPTIONS_H
#define MBASE_MCP_SERVER_DESCRIPTIONS_H

#include <mbase/mcp/mcp_server_arguments.h>

MBASE_BEGIN

struct McpResourceDescription {
    mbase::string mUri;
    mbase::string mName;
    mbase::string mDescription; // Optional
    mbase::string mMimeType; // Optional
    mbase::SIZE_T mSize = 0; // Optional
};

struct McpPromptDescription {
    mbase::string mName;
    mbase::string mDescription; // Optional
    mbase::vector<mbase::McpPromptArgument> mArguments; // Optional
};

struct McpToolDescription {
    mbase::string mName;
    mbase::string mDescription; // Optional
    mbase::vector<mbase::McpToolArgument> mArguments; // Optional
};

MBASE_END

#endif // MBASE_MCP_SERVER_DESCRIPTIONS_H