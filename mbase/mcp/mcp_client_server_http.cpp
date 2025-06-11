#include <mbase/mcp/mcp_client_server_http.h>
#include <cpp-httplib/httplib.h>

MBASE_BEGIN

McpClientServerHttp::McpClientServerHttp(const McpServerHttpInit& in_init): McpServerStateBase(mbase::mcp_transport_method::HTTP_STREAMBLE)
{
    set_hostname(in_init.mHostname);
    set_api_key(in_init.mApiKey);
    set_mcp_endpoint(in_init.mMcpEndpoint);
    add_header("Accept", "text/event-stream,application/json");
}

const mbase::string& McpClientServerHttp::get_hostname() const noexcept
{
    return mHostname;
}

const mbase::string& McpClientServerHttp::get_mcp_endpoint() const noexcept
{
    return mMcpEndpoint;
}

mbase::string McpClientServerHttp::get_header_value(const mbase::string& in_header) const noexcept
{
    auto mapIt = mHeadersMap.find(in_header);
    if(mapIt == mHeadersMap.end())
    {
        return mbase::string();
    }
    
    return mbase::string(mapIt->second.c_str(), mapIt->second.size());
}

const mbase::unordered_map<mbase::string, mbase::string>& McpClientServerHttp::get_headers() const noexcept
{
    return mHeadersMap;
}

GENERIC McpClientServerHttp::set_mcp_endpoint(const mbase::string& in_endpoint)
{
    mMcpEndpoint = in_endpoint;
    _mMcpEndpoint = std::string(mMcpEndpoint.c_str(), mMcpEndpoint.size());
}

GENERIC McpClientServerHttp::set_hostname(const mbase::string& in_hostname)
{
    mHostname = in_hostname;
    _mHostname = std::string(mHostname.c_str(), mHostname.size());
}

GENERIC McpClientServerHttp::set_api_key(const mbase::string& in_api_key)
{
    if(!in_api_key.size())
    {
        remove_header(in_api_key);
    }
    mApiKey = in_api_key;
    _mApiKey = std::string(mApiKey.c_str(), mApiKey.size());
}

GENERIC McpClientServerHttp::add_header(const mbase::string& in_header, const mbase::string& in_value)
{
    if(in_header.size())
    {
        mHeadersMap.insert({in_header, in_value});
    }
}

GENERIC McpClientServerHttp::remove_header(const mbase::string& in_header)
{
    mHeadersMap.erase(in_header);
}

GENERIC McpClientServerHttp::send_mcp_payload(const mbase::string& in_payload)
{
    mPayloadListSync.acquire();
    mPayloadList.push_back(in_payload);
    mPayloadListSync.release();
    this->start_processor();
}

GENERIC McpClientServerHttp::update_t()
{
    mPayloadListSync.acquire();
    while(mPayloadList.size())
    {
        httplib::Headers requestHeaders;
        mbase::vector<mbase::string> payloadList = std::move(mPayloadList);
        for(auto& currentHeader : mHeadersMap)
        {
            std::string headerKey(currentHeader.first.c_str(), currentHeader.first.size());
            std::string headerVal(currentHeader.second.c_str(), currentHeader.second.size());
            requestHeaders.insert({headerKey, headerVal});
        }
        mPayloadList = mbase::vector<mbase::string>();
        mPayloadListSync.release();
        httplib::Client clientInstance(_mHostname);
        if(_mApiKey.size())
        {
            clientInstance.set_bearer_token_auth(_mApiKey);
        }

        for(const mbase::string& currentPayload : payloadList)
        {
            httplib::Result requestResult = clientInstance.Post(_mMcpEndpoint, requestHeaders, currentPayload.c_str(), currentPayload.size(), "application/json");
            if(requestResult.error() != httplib::Error::Success)
            {
                continue;
            }
            if(requestResult->status)
            {
                if(requestResult->has_header("Mcp-Session-Id"))
                {
                    const std::string& sessionIdHeader = requestResult->get_header_value("Mcp-Session-Id");
                    mHeadersMap.insert({"Mcp-Session-Id", mbase::string(sessionIdHeader.c_str(), sessionIdHeader.size())});
                }
                if(requestResult->body.size())
                {
                    mbase::string _loadString(requestResult->body.c_str(), requestResult->body.size());
                    this->read_mcp_payload(_loadString);
                }
            }
        }
        mPayloadListSync.acquire();
    }
    mPayloadListSync.release();

    mIsProcessorRunning = false;
}

MBASE_END