#ifndef MBASE_MCP_NOTIFICATIONS_H
#define MBASE_MCP_NOTIFICATIONS_H

#include <mbase/mcp/mcp_common.h>

MBASE_BEGIN

enum class mcp_server_notification : I32 {
    MESSAGE,
    PROGRESS,
    CANCELLED,
    PROMPTS_LIST_CHANGED,
    RESOURCES_LIST_CHANGED,
    RESOURCE_UPDATED,
    TOOLS_LIST_CHANGED
};

struct McpNotificationLogMessage {
    // Spec: https://modelcontextprotocol.io/specification/2024-11-05/server/utilities/logging
    mbase::mcp_log_levels mLevel;
    mbase::string mLogger;
    mbase::string mError;
    mbase::Json mDetails;
};

struct McpNotificationProgress {
    // Spec: https://modelcontextprotocol.io/specification/2024-11-05/basic/utilities/progress

    mbase::string mProgressToken;
    mbase::F64 mProgress = 0;
    mbase::F64 mProgressTotal = 100;
    mbase::string mProgressMessage;
};

struct McpNotificationCancellation {
    // Spec: https://modelcontextprotocol.io/specification/2024-11-05/basic/utilities/cancellation

    mbase::Json mRequestId;
    mbase::string mReason;
};

struct McpNotificationCancellationIdStr{
    mbase::string mIdString;
    mbase::string mReason;
};

struct McpNotificationResourceUpdated{
    // Spec: https://modelcontextprotocol.io/specification/2024-11-05/server/resources#subscriptions

    mbase::string mUri;
};

MBASE_END

#endif // MBASE_MCP_NOTIFICATIONS_H