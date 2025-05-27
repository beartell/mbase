#include <mbase/mcp/mcp_server_stdio.h>
#include <mbase/mcp/mcp_packet_parsing.h>

MBASE_BEGIN

McpServerStdio::McpServerStdio(const mbase::string& in_server_name, const mbase::string& in_version_string) : McpServerBase(in_server_name, in_version_string, mcp_transport_method::STDIO), mSingleStdioClient(this)
{
    mSingleStdioClient.start_processor();
}

bool McpServerStdio::is_server_running() const noexcept
{
    return mSingleStdioClient.is_processor_running();
}

GENERIC McpServerStdio::update_t()
{
}

MBASE_END