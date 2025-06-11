#ifndef MBASE_MCP_SERVER_FEATURES_H
#define MBASE_MCP_SERVER_FEATURES_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/mcp/mcp_server_descriptions.h> // Description objects
#include <mbase/mcp/mcp_server_responses.h> // Response objects

MBASE_BEGIN

class McpServerClient;
class McpServerBase;
struct McpFeatureRequest;

using McpMessageArgument = std::variant<I64, F64, bool, mbase::string, mbase::vector<mbase::Json>, std::map<mbase::string, mbase::Json>>;
using McpMessageMap = mbase::unordered_map<mbase::string, McpMessageArgument>;

// Client instance, Message map, progress token
typedef McpResponseTool(*mcp_server_tool_cb)(McpServerClient*, const McpMessageMap&, const mbase::Json&);
typedef mbase::vector<McpResponsePrompt>(*mcp_server_prompt_cb)(McpServerClient*, const McpMessageMap&, const mbase::Json&);
typedef McpResponseResource(*mcp_server_resource_cb)(McpServerClient*, const mbase::Json&);

class MBASE_API McpToolFeature {
public:    
    const McpToolDescription& get_tool_description() const noexcept;
    mcp_server_tool_cb get_tool_cb() const noexcept;
    
    GENERIC update_tool_description(const McpToolDescription& in_desc, mcp_server_tool_cb in_cb) noexcept;
private:
    McpToolDescription mToolDescription;
    mcp_server_tool_cb mToolCb = nullptr;
};

class MBASE_API McpPromptFeature {
public:
    const McpPromptDescription& get_prompt_description() const noexcept;
    mcp_server_prompt_cb get_prompt_cb() const noexcept;

    GENERIC update_prompt_description(const McpPromptDescription& in_desc, mcp_server_prompt_cb in_cb) noexcept;
private:
    McpPromptDescription mPromptDescription;
    mcp_server_prompt_cb mPromptCb = nullptr;
};

class MBASE_API McpResourceFeature {
public:
    const McpResourceDescription& get_resource_description() const noexcept;
    mcp_server_resource_cb get_resource_cb() const noexcept;

    GENERIC update_resource_description(const McpResourceDescription& in_desc, mcp_server_resource_cb in_cb) noexcept;
private:
    McpResourceDescription mResourceDescription;
    mcp_server_resource_cb mResourceCb = nullptr;
};

struct McpFeatureRequest {
    feature_type mFeatureType;
    union
    {
        McpToolFeature* toolFeature = nullptr;
        McpResourceFeature* resourceFeature;
        McpPromptFeature* promptFeature;
    };
    McpServerClient* mRequestOwner = nullptr;
    McpMessageMap mMessageMap;
    mbase::Json mMessageId;
    mbase::Json mProgressId;
    bool mIsCancelled = false;
};

MBASE_END

#endif // MBASE_MCP_SERVER_FEATURES_H