#ifndef MBASE_MCP_SERVER_STDIO_CLIENT_H
#define MBASE_MCP_SERVER_STDIO_CLIENT_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/mcp/mcp_server_client_state.h> // McpServerClient

MBASE_BEGIN

class MBASE_API McpServerStdioClient : public mbase::McpServerClient {
public:
    McpServerStdioClient(McpServerBase* in_server_instance);
    ~McpServerStdioClient();
    GENERIC send_mcp_payload(const mbase::string& in_payload) override;
    GENERIC update_t() override;

private:
    mbase::mutex mStdioMutex;
};

MBASE_END

#endif // MBASE_MCP_SERVER_STDIO_CLIENT_H