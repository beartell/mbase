#include <mbase/mcp/mcp_server_features.h>
#include <mbase/mcp/mcp_server_client_state.h>

MBASE_BEGIN

const McpToolDescription& McpToolFeature::get_tool_description() const noexcept 
{ 
    return mToolDescription; 
}

mcp_server_tool_cb McpToolFeature::get_tool_cb() const noexcept 
{ 
    return mToolCb; 
}

GENERIC McpToolFeature::update_tool_description(const McpToolDescription& in_desc, mcp_server_tool_cb in_cb) noexcept 
{
    mToolDescription = in_desc; 
    mToolCb = in_cb;
}

const McpPromptDescription& McpPromptFeature::get_prompt_description() const noexcept
{ 
    return mPromptDescription; 
}

mcp_server_prompt_cb McpPromptFeature::get_prompt_cb() const noexcept 
{ 
    return mPromptCb; 
}

GENERIC McpPromptFeature::update_prompt_description(const McpPromptDescription& in_desc, mcp_server_prompt_cb in_cb) noexcept
{
    mPromptDescription = in_desc;
    mPromptCb = in_cb;
}

const McpResourceDescription& McpResourceFeature::get_resource_description() const noexcept
{ 
    return mResourceDescription; 
}

mcp_server_resource_cb McpResourceFeature::get_resource_cb() const noexcept
{ 
    return mResourceCb; 
}

GENERIC McpResourceFeature::update_resource_description(const McpResourceDescription& in_desc, mcp_server_resource_cb in_cb) noexcept
{
    mResourceDescription = in_desc;
    mResourceCb = in_cb;
}

MBASE_END