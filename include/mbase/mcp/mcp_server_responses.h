#ifndef MBASE_MCP_SERVER_RESPONSES_H
#define MBASE_MCP_SERVER_RESPONSES_H

#include <mbase/mcp/mcp_common.h>

MBASE_BEGIN

enum class mcp_response_type {
    TEXT,
    IMAGE,
    BINARY
};

struct McpResponseTextResource {
    mbase::string mUri;
    mbase::string mMimeType;
    mbase::string mText;
};

struct McpResponseBinaryResource {
    mbase::string mUri;
    mbase::string mMimeType;
    mbase::string mBlob;
};

struct McpResponseTextPrompt {
    mbase::string mRole = "user";
    mbase::string mType = "text";
    mbase::string mText;
};

struct McpResponseImagePrompt {
    mbase::string mRole = "user";
    mbase::string mType = "image";
    mbase::string mMimeType; // "image/png" ...
    mbase::string mData; // base64-encoded-image-data
};

struct McpResponseAudioPrompt {
    mbase::string mRole = "user";
    mbase::string mType = "audio";
    mbase::string mMimeType; // audio/wav etc.
    mbase::string mData; // base-64-encoded-audio-data
};

struct McpResponseTextTool {
    mbase::string mType = "text";
    mbase::string mText;
};

struct McpResponseImageTool {
    mbase::string mType = "image";
    mbase::string mMimeType; // "image/png" ...
    mbase::string mData; // base64-encoded-image-data
};

struct McpResponseAudioTool {
    mbase::string mType = "audio";
    mbase::string mMimeType; // "audio/wav" ...
    mbase::string mData; // base64-encoded-audio-data
};

struct McpPromptCompilationResult {
    mbase::vector<mbase::string> mValues;
    I32 mTotal = 0;
    bool mHasMore = false;
};

using McpResponseResource = std::variant<McpResponseTextResource, McpResponseBinaryResource>;
using McpResponsePrompt = std::variant<McpResponseTextPrompt, McpResponseImagePrompt, McpResponseAudioPrompt>;
using McpResponseTool = std::variant<McpResponseTextTool, McpResponseImageTool, McpResponseAudioTool>;

MBASE_END

#endif // MBASE_MCP_SERVER_RESPONSES_H