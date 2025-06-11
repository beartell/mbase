==========================
Overwriting the Dispatcher
==========================

User may want to overwrite the MCP message dispatcher method i.e :code:`update` to inject custom application behavior, 
expecting to be executed every iteration(or frame):

.. code-block:: cpp
    :caption: client.cpp

    while(1)
    {
        myMcpClient.update();
        mbase::sleep(5); // in order to prevent resource exhaustion
    }

.. code-block:: cpp
    :caption: server.cpp

    while(mcpServer.is_processor_running())
    {
        mcpServer.update();
        mbase::sleep(5); // in order to prevent resource exhaustion
    }

.. important::

    See: :ref:`MCP Client Program Loop <mcp-client-quickstart-program-loop>`, :ref:`MCP Server Program Loop <mcp-server-implement-program-loop>`

----------
MCP Server
----------

The :code:`update` method is defined under the :code:`McpServerBase` class:

.. code-block:: cpp
    :caption: mcp_server_base.h

    class MBASE_API McpServerBase : public mbase::logical_processor {
    public:
        ...

        // updates the server state
        GENERIC update() override
        {
            // Should be called every frame.
        }
        ...
    private:
        ...
    };

In which the :code:`McpServerStdio`, :code:`McpServerHttpStreamableStateful` and :code:`McpServerHttpStreamableStateless` inherit from:

.. code-block:: cpp
    :caption: mcp_server_stdio.h

    class MBASE_API McpServerStdio : public mbase::McpServerBase {
        ...
    };

.. code-block:: cpp
    :caption: mcp_server_http_streamable.h

    class MBASE_API McpServerHttpStreamableStateful : public mbase::McpServerHttpBase {
        ...
    };

    class MBASE_API McpServerHttpStreamableStateless : public mbase::McpServerHttpBase {
        ...
    };

The assumption of STDIO transport will result with the following example implementation:

.. code-block:: cpp
    :caption: server.cpp

    class ExampleDerivedServer : public mbase::McpServerStdio {
    public:
        ExampleDerivedServer() : mbase::McpServerStdio("MCP Sample Server","1.0.0"){}
        void update() override
        {
            this->default_update_method();
            // your logic is here
        }
    };

----------
MCP Client
----------

The :code:`update` method is defined under the :code:`McpClientBase` class:

.. code-block:: cpp
    :caption: mcp_client_base.h

    class MBASE_API McpClientBase {
    public:
        ...
        // updates the server state
        virtual GENERIC update()
        {
            // Should be called every frame
        }
        ...
    private:
        ...
    };


