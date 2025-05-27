#ifndef MBASE_MCP_SERVER_ARGUMENTS_H
#define MBASE_MCP_SERVER_ARGUMENTS_H

#include <mbase/mcp/mcp_common.h>

MBASE_BEGIN

enum class McpValueType {
    NUMBER,
    BOOL,
    STRING,
    ARRAY,
    JSON
};

struct McpPromptArgument {
    mbase::string mArgumentName;
    mbase::string mDescription;
    mbase::vector<mbase::string> mCompletionStrings;
    bool mIsRequired = false;
};

struct McpToolArgument {
    mbase::string mArgumentName;
    mbase::string mDescription;
    McpValueType mArgType;
    bool mIsRequired = false;
};

MBASE_END

#endif // MBASE_MCP_SERVER_ARGUMENTS_H