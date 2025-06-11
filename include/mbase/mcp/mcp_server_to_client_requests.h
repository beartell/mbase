#ifndef MBASE_MCP_SERVER_TO_CLIENT_REQUESTS_H
#define MBASE_MCP_SERVER_TO_CLIENT_REQUESTS_H

#include <mbase/mcp/mcp_common.h>

MBASE_BEGIN

class McpServerBase;
class McpServerStateBase;

enum class mcp_sampling_content_type {
    IMAGE,
    TEXT,
    AUDIO
};

struct McpSamplingMessage {
    mbase::string mRole = "user";
    mcp_sampling_content_type mContentType = mcp_sampling_content_type::TEXT;
    mbase::string mTextContent;
    mbase::string mBase64Data; // base64 encoded binary data
    mbase::string mMimeType; // mime type
};

struct McpSamplingModelPreferences {
    mbase::vector<mbase::string> mModelHintsList;
    F64 mIntelligencePriority = 0.8;
    F64 mSpeedPriority = 0.5;
};

struct McpSamplingRequest {
    mbase::vector<mbase::McpSamplingMessage> mMessages;
    McpSamplingModelPreferences mModelPreferences;
    mbase::string mSystemPrompt;
    I32 mMaxTokens = 0;
};

struct McpRootsResult {
    mbase::string mUri;
    mbase::string mName;
};

struct McpSamplingResult {
    mbase::string mRole;
    mbase::string mModel;
    mbase::string mStopReason;
    mcp_sampling_content_type mContentType = mcp_sampling_content_type::TEXT;
    mbase::string mTextContent;
    mbase::string mBase64Content;
    mbase::string mMimeType;
};

// error code, server instance, result object
using mcp_st_list_roots_cb = std::function<GENERIC(const I32&, McpServerBase*, const mbase::vector<McpRootsResult>&)>;
using mcp_st_sampling_cb = std::function<GENERIC(const I32&, McpServerBase*, const McpSamplingResult&)>;
using mcp_st_cb = std::variant<mcp_st_list_roots_cb, mcp_st_sampling_cb>;

struct McpStClientRequestObject {
    mbase::string mMessageId;
    mbase::I32 mTimeoutInSeconds = 10;
    mbase::I32 mAttemptCounter = 0;
    volatile bool mResponseWaiting = true;
    mcp_st_cb mStCallback;
};

struct McpStRootsResultObject {
    mcp_st_cb rootsResultCallback;
    mbase::vector<McpRootsResult> rootsResult;
};

struct McpStSamplingResultObject {
    mcp_st_cb samplingResultCallback;
    McpSamplingResult samplingResult;
};

struct McpSamplingRequestObject {
    mbase::Json mRequestId;
    McpServerStateBase* requestOwner = nullptr;
    McpSamplingRequest samplingRequest;
};

MBASE_END

#endif //