#include <mbase/mcp/mcp_client_server_stdio.h>

MBASE_BEGIN

McpClientServerStdio::McpClientServerStdio(const mbase::McpServerStdioInit& in_init) 
: McpServerStateBase(mbase::mcp_transport_method::STDIO), mSubProcessManager(true, in_init.mCommand, in_init.mArguments, in_init.mEnvironmentVariables)
{
}

bool McpClientServerStdio::is_subprocess_alive() const noexcept
{
    return mIsSubprocessAlive;
}

GENERIC McpClientServerStdio::send_mcp_payload(const mbase::string& in_payload)
{
    writerSync.acquire();
    mbase::io_file& writePipe = mSubProcessManager.get_write_pipe();
    if(writePipe.is_operate_ready())
    {
        writePipe.write_data(in_payload.c_str(), in_payload.size());
    }
    writerSync.release();
}

GENERIC McpClientServerStdio::update_t()
{
    while(is_processor_running() && mSubProcessManager.get_read_pipe1().is_file_open())
    {
        char readBytes[MBASE_MCP_STDIO_BUFFER_LENGTH] = {0};
        I32 byteLength = mSubProcessManager.get_read_pipe1().read_available_data(readBytes, MBASE_MCP_STDIO_BUFFER_LENGTH - 1);
        if(byteLength > 0)
        {
            mbase::string readBytesString(readBytes, byteLength);
            mbase::vector<mbase::string> seperatedPacket;
            readBytesString.split("\n", seperatedPacket);
            for(const mbase::string& currentMcpPayload : seperatedPacket)
            {
                this->read_mcp_payload(currentMcpPayload);
            }
        }
        else
        {
            break;
        }
    }
    mIsSubprocessAlive = false;
}

MBASE_END