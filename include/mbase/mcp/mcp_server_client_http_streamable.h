#ifndef MBASE_MCP_SERVER_CLIENT_HTTP_STREAMABLE_H
#define MBASE_MCP_SERVER_CLIENT_HTTP_STREAMABLE_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/mcp/mcp_server_client_state.h> // McpServerClient

namespace httplib
{
    struct Response;
}

MBASE_BEGIN

class MBASE_API McpServerClientHttpStreamable : public mbase::McpServerClient {
public:
    // this is called if the client is stateful
    McpServerClientHttpStreamable(
        McpServerBase* in_server_instance,
        const mbase::string& in_session_id
    );

    // this is called if the client is stateless
    McpServerClientHttpStreamable(
        McpServerBase* in_server_instance
    );

    bool is_request_processed() const noexcept;
    const mbase::string& get_session_id() const noexcept; // returns empty stirng if the client is stateless
    GENERIC on_empty_processed_t() override;
    GENERIC send_mcp_payload(const mbase::string& in_payload) override;
    GENERIC update_t() override;

    GENERIC set_response_object(httplib::Response* in_response);
private:
    httplib::Response* mResponse = nullptr;
    mbase::string mSessionId;
    volatile bool mIsRequestProcessed = false;
};

MBASE_END

#endif // MBASE_MCP_SERVER_CLIENT_HTTP_STREAMABLE_H