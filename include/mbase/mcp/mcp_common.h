#ifndef MBASE_MCP_COMMON_H
#define MBASE_MCP_COMMON_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/json/json.h>
#include <mbase/vector.h>
#include <mbase/unordered_map.h>
#include <mbase/synchronization.h> // for mbase::mutex
#include <mbase/thread.h>
#include <variant>
#include <functional>

MBASE_BEGIN

#define MBASE_MCP_SUCCESS 0
#define MBASE_MCP_PARSE_ERROR -32700
#define MBASE_MCP_INVALID_REQUEST -32600
#define MBASE_MCP_METHOD_NOT_FOUND -32601
#define MBASE_MCP_INVALID_PARAMS -32602
#define MBASE_MCP_INTERNAL_ERROR -32603
#define MBASE_MCP_TIMEOUT -32001
#define MBASE_MCP_RESOURCE_NOT_FOUND -32002

#ifndef MBASE_MCP_TIMEOUT_DEFAULT
    #define MBASE_MCP_TIMEOUT_DEFAULT 10 // in seconds
#endif

#ifndef MBASE_MCP_STDIO_BUFFER_LENGTH
    #define MBASE_MCP_STDIO_BUFFER_LENGTH (64 * 1024)
#endif

#define MBASE_MCP_DEFAULT_VERSION "2025-03-26"

static inline mbase::string gMcpVersion = "2025-03-26";

enum class mcp_log_levels {
    L_DEBUG,
    L_INFO,
    L_NOTICE,
    L_WARNING,
    L_ERROR,
    L_CRITICAL,
    L_ALERT,
    L_EMERGENCY
};

enum class feature_type : I32 {
    TOOL,
    PROMPT,
    RESOURCE
};

enum class mcp_transport_method {
    STDIO,
    HTTP_STREAMBLE,
    INTERNAL
};

struct mcp_err_format {
    I32 mCode = MBASE_MCP_SUCCESS;
    mbase::string mMessage;
    mbase::Json mData;
};

MBASE_END

#endif // MBASE_MCP_COMMON_H