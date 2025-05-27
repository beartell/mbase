#ifndef MBASE_MCP_CLIENT_SERVER_HTTP_H
#define MBASE_MCP_CLIENT_SERVER_HTTP_H

#include <mbase/mcp/mcp_client_server_state.h> // McpServerStateBase

MBASE_BEGIN

class MBASE_API McpClientServerHttp : public McpServerStateBase {
public:
    McpClientServerHttp(const McpServerHttpInit& in_init);

    const mbase::string& get_api_key() const noexcept;
    const mbase::string& get_hostname() const noexcept;
    const mbase::string& get_mcp_endpoint() const noexcept;
    mbase::string get_header_value(const mbase::string& in_header) const noexcept;
    const mbase::unordered_map<mbase::string, mbase::string>& get_headers() const noexcept;

    GENERIC set_mcp_endpoint(const mbase::string& in_endpoint);
    GENERIC set_hostname(const mbase::string& in_hostname);
    GENERIC set_api_key(const mbase::string& in_api_key);
    GENERIC add_header(const mbase::string& in_header, const mbase::string& in_value);
    GENERIC remove_header(const mbase::string& in_header);
    GENERIC send_mcp_payload(const mbase::string& in_payload) override;
    GENERIC update_t() override;
private:
    mbase::string mHostname;
    std::string _mHostname;
    mbase::string mApiKey;
    std::string _mApiKey;
    mbase::string mMcpEndpoint;
    std::string _mMcpEndpoint;
    mbase::unordered_map<mbase::string, mbase::string> mHeadersMap;
    mbase::vector<mbase::string> mPayloadList;
    mbase::mutex mPayloadListSync;
};

MBASE_END

#endif // MBASE_MCP_CLIENT_SERVER_HTTP_H