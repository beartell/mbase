#ifndef MBASE_SERVER_HTTP_STREAMABLE_H
#define MBASE_SERVER_HTTP_STREAMABLE_H

#include <mbase/mcp/mcp_common.h>
#include <mbase/mcp/mcp_server_base.h> // McpServerBase
#include <mbase/mcp/mcp_server_client_http_streamable.h> // McpServerHttpBase
#include <memory>

namespace httplib
{
    class Server;
}

MBASE_BEGIN

// TODO: implement the security layer and rate limits

class MBASE_API McpServerHttpBase : public mbase::McpServerBase {
public:
    McpServerHttpBase(
        const mbase::string& in_server_name, 
        const mbase::string& in_version_string, 
        const mbase::string& in_hostname, 
        const I32& in_port,
        const mbase::string& in_api_key = mbase::string(),
        const mbase::string& in_public_key_file = mbase::string(),
        const mbase::string& in_private_key_file = mbase::string()
    );

    const mbase::string& get_hostname() const noexcept;
    const mbase::string& get_api_key() const noexcept;
    const I32& get_port() const noexcept;
protected:
    std::unique_ptr<httplib::Server> svr;
    mbase::string mHostname = "localhost";
    mbase::string mApiKey;
    I32 mPort = 8000;
};

class MBASE_API McpServerHttpStreamableStateful : public mbase::McpServerHttpBase {
public:
    McpServerHttpStreamableStateful(
        const mbase::string& in_server_name, 
        const mbase::string& in_version_string, 
        const mbase::string& in_hostname, 
        const I32& in_port,
        const mbase::string& in_api_key = mbase::string(),
        const mbase::string& in_public_key_file = mbase::string(),
        const mbase::string& in_private_key_file = mbase::string()
    );
    ~McpServerHttpStreamableStateful();
    bool is_server_running() const noexcept;
    GENERIC update_t() override;
private:
    mbase::unordered_map<mbase::string, mbase::McpServerClientHttpStreamable*> mStreambleClients;
};

class MBASE_API McpServerHttpStreamableStateless : public mbase::McpServerHttpBase {
public:
    McpServerHttpStreamableStateless(
        const mbase::string& in_server_name, 
        const mbase::string& in_version_string, 
        const mbase::string& in_hostname, 
        const I32& in_port,
        const mbase::string& in_api_key = mbase::string(),
        const mbase::string& in_public_key_file = mbase::string(),
        const mbase::string& in_private_key_file = mbase::string()
    );
    ~McpServerHttpStreamableStateless();
    bool is_server_running() const noexcept;
    GENERIC update() override;
    GENERIC update_t() override;
private:
    mbase::vector<McpServerClientHttpStreamable*> mHttpClients;
};

MBASE_END

#endif // MBASE_SERVER_HTTP_STREAMABLE_H