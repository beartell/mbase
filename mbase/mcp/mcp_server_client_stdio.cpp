#include <mbase/mcp/mcp_server_client_stdio.h>
#include <mbase/mcp/mcp_server_stdio.h>
#include <mbase/io_file.h>

MBASE_BEGIN

McpServerStdioClient::McpServerStdioClient(McpServerBase* in_server_instance) : McpServerClient(in_server_instance)
{
}

McpServerStdioClient::~McpServerStdioClient()
{
}

GENERIC McpServerStdioClient::send_mcp_payload(const mbase::string& in_payload)
{
    mStdioMutex.acquire();
    gStdout.write_data(in_payload.c_str(), in_payload.size());
    mStdioMutex.release();
}

GENERIC McpServerStdioClient::update_t()
{
    I32 bytesRead = 1;
    while(bytesRead > 0 && mServerInstance->is_processor_running())
    {
        char totalMcpRequest[MBASE_MCP_STDIO_BUFFER_LENGTH] = {0};
        bytesRead = gStdin.read_available_data(totalMcpRequest, MBASE_MCP_STDIO_BUFFER_LENGTH - 1);

        mbase::string mcpRequestString(totalMcpRequest, bytesRead);
        mbase::vector<mbase::string> mcpMessages;
        mcpRequestString.split("\n", mcpMessages);
        for(const mbase::string& mcpRequest : mcpMessages)
        {
            this->read_mcp_payload(mcpRequest);
        }
    }
}

MBASE_END