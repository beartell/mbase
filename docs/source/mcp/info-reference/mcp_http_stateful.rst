=======================
MCP HTTP Server Details
=======================

----------
MCP Server
----------

The :code:`McpServerHttpBase` class if defined as follows:

.. code-block:: cpp
    :caption: mcp_server_http_streamable.h

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

        ...
    private:
        ...
    };

In which both :code:`McpServerHttpStreamableStateful` and :code:`McpServerHttpStreamableStateless` inherit from:

.. code-block:: cpp
    :caption: mcp_server_http_streamable.h

    class MBASE_API McpServerHttpStreamableStateful : public mbase::McpServerHttpBase {
    public:
        ...
    private:
        ...
    };

    class MBASE_API McpServerHttpStreamableStateless : public mbase::McpServerHttpBase {
    public:
        ...
    private:
        ...
    };

Which implies a usage such as:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpServerHttpStreamableStateless mcpServer(
        "MCP Sample Server",
        "1.0.0",
        "127.0.0.1", // hostname
        8080 // port
    );

.. important::
    See: :ref:`mcp-server-http-init`

----------
MCP Client
----------

The :code:`McpClientServerHttp` class is defined as follows:

.. code-block:: cpp
    :caption: mcp_client_server_http.h

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
        ...
    private:
        ...
    };

In which the :code:`McpServerHttpInit` object defined as:

.. code-block:: cpp
    :caption: mcp_client_arguments.h

    struct McpServerHttpInit {
        mbase::string mHostname;
        mbase::string mMcpEndpoint = "/mcp";
        mbase::string mApiKey;
    };

Considering the definitions above, the call should look like the following:

.. code-block:: cpp
    :caption: client.cpp

    mbase::McpClientBase myMcpClient(
        "MCP Sample Client",
        "1.0.0"
    );

    mbase::McpServerHttpInit initDesc;
    initDesc.mHostname = "localhost:8080";
    
    mbase::McpClientServerHttp mcpServerState(initDesc);

.. note:: 
    See: :ref:`mcp-client-http-init`

------------------------------
Stateful HTTP Request Handling
------------------------------

TODO: Document