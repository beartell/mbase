#include <mbase/mcp/mcp_server_client_http_streamable.h>
#include <cpp-httplib/httplib.h>

MBASE_BEGIN

McpServerClientHttpStreamable::McpServerClientHttpStreamable(
    McpServerBase* in_server_instance, 
    const mbase::string& in_session_id
):
    mbase::McpServerClient(in_server_instance),
    mSessionId(in_session_id),
    mIsRequestProcessed(false)
{
}

McpServerClientHttpStreamable::McpServerClientHttpStreamable(
    McpServerBase* in_server_instance
): 
    mbase::McpServerClient(in_server_instance),
    mIsRequestProcessed(false)
{
    // assume it is already initialized
    // as the PR suggests(https://github.com/modelcontextprotocol/modelcontextprotocol/pull/206)
    mIsClientInitialized = true;
}

bool McpServerClientHttpStreamable::is_request_processed() const noexcept
{
    return mIsRequestProcessed;
}

const mbase::string& McpServerClientHttpStreamable::get_session_id() const noexcept
{
    return mSessionId;
}

GENERIC McpServerClientHttpStreamable::on_empty_processed_t()
{
    mIsRequestProcessed = true;
}

GENERIC McpServerClientHttpStreamable::send_mcp_payload(const mbase::string& in_payload)
{
    if(mResponse)
    {
        std::string mcpContent(in_payload.c_str(), in_payload.size());
        mResponse->set_content(mcpContent, "application/json");
        mIsRequestProcessed = true;
        mResponse = nullptr;
    }
}

GENERIC McpServerClientHttpStreamable::update_t()
{
    // nothing
}

GENERIC McpServerClientHttpStreamable::set_response_object(httplib::Response* in_response)
{
    mIsRequestProcessed = false;
    mResponse = in_response;
}

MBASE_END
