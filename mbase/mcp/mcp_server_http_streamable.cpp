#include <mbase/mcp/mcp_server_http_streamable.h>
#include <cpp-httplib/httplib.h>

MBASE_BEGIN

McpServerHttpBase::McpServerHttpBase(
    const mbase::string& in_server_name, 
    const mbase::string& in_version_string, 
    const mbase::string& in_hostname, 
    const I32& in_port,
    const mbase::string& in_api_key
):
    McpServerBase(in_server_name, in_version_string, mcp_transport_method::HTTP_STREAMBLE)
{
    svr = std::make_unique<httplib::Server>();
    mHostname = in_hostname;
    mPort = in_port;
    mApiKey = in_api_key;
}

const mbase::string& McpServerHttpBase::get_hostname() const noexcept
{
    return mHostname;
}

const mbase::string& McpServerHttpBase::get_api_key() const noexcept
{
    return mApiKey;
}

const I32& McpServerHttpBase::get_port() const noexcept
{
    return mPort;
}

bool mcp_streamable_http_validate(const httplib::Request& in_request, httplib::Response& out_response)
{
    if(!in_request.body.size())
    {
        out_response.status = 400;
        return false;
    }

    if(!in_request.has_header("Accept"))
    {
        out_response.status = 400;
        return false;
    }

    std::string _accept_header_val = in_request.get_header_value("Accept");
    mbase::string accept_header_val = "";
    for(auto& n : _accept_header_val)
    {
        // trimming white spaces
        if(n == ' ')
        {

        }
        else
        {
            accept_header_val += n;
        }
    }
    mbase::vector<mbase::string> headerVals;
    accept_header_val.split(",", headerVals);
    if(headerVals.size() != 2)
    {
        out_response.status = 400;
        return false;
    }

    if((headerVals[0] != "application/json" && headerVals[0] != "text/event-stream") || (headerVals[1] != "application/json" && headerVals[1] != "text/event-stream"))
    {
        out_response.status = 400;
        return false;
    }

    return true;
}

McpServerHttpStreamableStateful::McpServerHttpStreamableStateful(
        const mbase::string& in_server_name, 
        const mbase::string& in_version_string, 
        const mbase::string& in_hostname, 
        const I32& in_port,
        const mbase::string& in_api_key
) : McpServerHttpBase(in_server_name, in_version_string, in_hostname, in_port, in_api_key)
{
}

McpServerHttpStreamableStateful::~McpServerHttpStreamableStateful()
{
    svr->stop();
}

bool McpServerHttpStreamableStateful::is_server_running() const noexcept
{
    return this->is_processor_running();
}

GENERIC McpServerHttpStreamableStateful::update_t()
{
    svr->Get("/mcp", [&](const httplib::Request& in_request, httplib::Response& out_response){
        // sse not supported
        out_response.status = 405;
    });

    svr->Post("/mcp", [&](const httplib::Request& in_request, httplib::Response& out_response){
        // for post request types
        
        if(this->get_api_key().size())
        {
            if(!in_request.has_header("Authorization"))
            {
                out_response.status = 401;
                return;
            }
            std::string authVal = in_request.get_header_value("Authorization");
            mbase::string tmpAuthVal(authVal.c_str(), authVal.size());
            mbase::vector<mbase::string> schemeAndParam;
            tmpAuthVal.split(" ", schemeAndParam);
            if(schemeAndParam.size() != 2)
            {
                out_response.status = 401;
                return;
            }

            if(schemeAndParam[0] != "Bearer")
            {
                out_response.status = 401;
                return;
            }

            if(schemeAndParam[1] != this->get_api_key())
            {
                out_response.status = 401;
                return;
            }
        }

        if(!mcp_streamable_http_validate(in_request, out_response)){ return; }

        mbase::McpServerClientHttpStreamable* currentStreamableClient = nullptr;
        mbase::string mcpSessionId;
        mbase::string mcpPayload(in_request.body.c_str(), in_request.body.size());
        if(in_request.has_header("Mcp-Session-Id"))
        {
            const std::string& _sessId = in_request.get_header_value("Mcp-Session-Id");
            mcpSessionId = mbase::string(_sessId.c_str(), _sessId.size());
            mbase::unordered_map<mbase::string, mbase::McpServerClientHttpStreamable*>::iterator It = mStreambleClients.find(mcpSessionId);
            if(It == mStreambleClients.end())
            {
                out_response.status = 404;
                return;
            }
            currentStreamableClient = It->second;
            currentStreamableClient->acquire_synchronizer();
            while(!currentStreamableClient->is_request_processed()){ mbase::sleep(2); }
            currentStreamableClient->set_response_object(&out_response);
            currentStreamableClient->release_synchronizer();
        }
        else
        {
            mcpSessionId = mbase::string::generate_uuid();
            currentStreamableClient = new mbase::McpServerClientHttpStreamable(this, mcpSessionId);
            mStreambleClients[mcpSessionId] = currentStreamableClient;
            out_response.set_header("Mcp-Session-Id", std::string(mcpSessionId.c_str(), mcpSessionId.size()));
            currentStreamableClient->set_response_object(&out_response);
        }

        currentStreamableClient->read_mcp_payload(mcpPayload);
        while(!currentStreamableClient->is_request_processed()){ mbase::sleep(2); }
        out_response.status = 200;
    }); 
    svr->listen(this->get_hostname().c_str(), this->get_port());
    mIsProcessorRunning = false;
}

McpServerHttpStreamableStateless::McpServerHttpStreamableStateless(
    const mbase::string& in_server_name, 
    const mbase::string& in_version_string, 
    const mbase::string& in_hostname, 
    const I32& in_port,
    const mbase::string& in_api_key
) : McpServerHttpBase(in_server_name, in_version_string, in_hostname, in_port, in_api_key)
{
}

McpServerHttpStreamableStateless::~McpServerHttpStreamableStateless()
{
    svr->stop();
}

GENERIC McpServerHttpStreamableStateless::update()
{
    this->default_update_method();
}

bool McpServerHttpStreamableStateless::is_server_running() const noexcept
{
    return this->is_processor_running();
}

GENERIC McpServerHttpStreamableStateless::update_t()
{
    svr->Get("/mcp", [&](const httplib::Request& in_request, httplib::Response& out_response){
        // sse not supported
        out_response.status = 405;
    });

    svr->Post("/mcp", [&](const httplib::Request& in_request, httplib::Response& out_response){
        // for post request types
        
        if(this->get_api_key().size())
        {
            if(!in_request.has_header("Authorization"))
            {
                out_response.status = 401;
                return;
            }
            std::string authVal = in_request.get_header_value("Authorization");
            mbase::string tmpAuthVal(authVal.c_str(), authVal.size());
            mbase::vector<mbase::string> schemeAndParam;
            tmpAuthVal.split(" ", schemeAndParam);
            if(schemeAndParam.size() != 2)
            {
                out_response.status = 401;
                return;
            }

            if(schemeAndParam[0] != "Bearer")
            {
                out_response.status = 401;
                return;
            }

            if(schemeAndParam[1] != this->get_api_key())
            {
                out_response.status = 401;
                return;
            }
        }

        if(!mcp_streamable_http_validate(in_request, out_response)){ return; }

        mbase::string mcpPayload(in_request.body.c_str(), in_request.body.size());
        mbase::McpServerClientHttpStreamable currentStreamableClient(this);
        currentStreamableClient.set_response_object(&out_response);
        currentStreamableClient.read_mcp_payload(mcpPayload);
        while(!currentStreamableClient.is_request_processed()){mbase::sleep(2);}
        out_response.status = 200;
    });
    svr->listen(this->get_hostname().c_str(), this->get_port());
    mIsProcessorRunning = false;
}

MBASE_END