#ifndef MBASE_MCP_SERVER_STDIO_H
#define MBASE_MCP_SERVER_STDIO_H

#include <mbase/mcp/mcp_server_base.h> // McpServerBase
#include <mbase/mcp/mcp_server_client_stdio.h> // McpServerStdioClient

MBASE_BEGIN

class MBASE_API McpServerStdio : public mbase::McpServerBase {
public:
    McpServerStdio(const mbase::string& in_server_name, const mbase::string& in_version_string);
    ~McpServerStdio();
    GENERIC update_t() override;

private:
    mbase::McpServerStdioClient mSingleStdioClient;
};

MBASE_END

#endif // MBASE_MCP_SERVER_STDIO_H