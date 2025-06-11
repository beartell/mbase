#ifndef MBASE_MCP_CLIENT_SERVER_STDIO_H
#define MBASE_MCP_CLIENT_SERVER_STDIO_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/mcp/mcp_client_server_state.h> // McpServerStateBase
#include <mbase/io_file.h> // gStdin, gStdout
#include <mbase/subprocess.h> // mbase::subprocess manager

MBASE_BEGIN

class MBASE_API McpClientServerStdio : public McpServerStateBase {
public:
    McpClientServerStdio(const mbase::McpServerStdioInit& in_init);
    bool is_subprocess_alive() const noexcept;
    GENERIC send_mcp_payload(const mbase::string& in_payload) override;
    GENERIC update_t() override;
private:
    bool mIsSubprocessAlive = false;
    mbase::subprocess mSubProcessManager;
    mbase::mutex writerSync;
};

MBASE_END

#endif // MBASE_MCP_CLIENT_SERVER_STDIO_H