===============================
Implementing Custom MCP Methods
===============================

----------
MCP Server
----------

.. _mcp-server-custom-request-handle:

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Handling Custom Requests/Notifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Following request/notification transport callbacks are defined under the :code:`McpServerBase` class:

.. code-block:: cpp
    :caption: mcp_server_base.h

    class MBASE_API McpServerBase : public mbase::logical_processor {
    public:
        ...
        virtual bool on_client_request_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params)
        {
            /*
                Invoked when a new request is received.

                Returning false will make the MCP server behave as normal which is
                processing the request and appending it to the MCP message queue waiting for
                application thread to dispatch.

                If you return true from this method, 
                the current request's responsibility is on your own.
                Returning true also indicates that you are writing a custom request handler.

                See: TODO -> Link about custom request handling

                See: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages
            */
        }
        virtual GENERIC on_client_notification_t(mbase::McpServerClient* in_client, const mbase::string& in_method, const mbase::Json& in_params)
        {
            /*
                Invoked when a notification is sent from the client.

                See: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages
            */
        }
        ...
    };

In which the :code:`McpServerStdio`, :code:`McpServerHttpStreamableStateful` and :code:`McpServerHttpStreamableStateless` inherit from
but not defining the :code:`on_client_request_t` and :code:`on_client_notification_t`:

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

Writing a custom request/notification handling
mandates for a user to inherit from one of those classes and override their fundamental callbacks.

The assumption of STDIO transport will result with the following example implementation: 

.. code-block:: cpp
    :caption: server.cpp

    class ExampleDerivedServer : public mbase::McpServerStdio {
    public:
        ExampleDerivedServer() : mbase::McpServerStdio("MCP Sample Server","1.0.0"){}
        bool on_client_request_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params) override
        {
            return true;
        }
        void on_client_notification_t(mbase::McpServerClient* in_client, const mbase::string& in_method, const mbase::Json& in_params) override
        {
            
        }
    };

In which returning true implies the user is writing a custom request.

The workflow for writing a custom request/handling is as follows:

1. Read the method and params and write your business logic.
2. Generate an MCP response string.
3. Send the generated MCP payload.

Let's first read the method:

.. code-block:: cpp
    :caption: server.cpp

    bool on_client_request_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params) override
    {
        if(in_method == "custom_method")
        {
            return true;
        }
    }

Then, we will call the :code:`mcp_generate_response` procedure which is defined under the :code:`mcp_packet_parsing.h` file:

.. code-block:: cpp
    :caption: server.cpp

    bool on_client_request_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params) override
    {
        if(in_method == "custom_method")
        {
            mbase::Json randomData;
            randomData["example_key"] = "example_value";
            randomData["example_key_2"] = 100; // arbitrary number
            mbase::string generatedPacket = mbase::mcp_generate_response(in_msgid, randomData);
            return true;
        }
    }

Then, send the generated MCP payload:

.. code-block:: cpp
    :caption: server.cpp

    bool on_client_request_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params) override
    {
        if(in_method == "custom_method")
        {
            mbase::Json randomData;
            randomData["example_key"] = "example_value";
            randomData["example_key_2"] = 100; // arbitrary number
            mbase::string generatedPacket = mbase::mcp_generate_response(in_msgid, randomData);
            in_client->send_mcp_payload(generatedPacket);
            return true;
        }
    }

You can also send error messages by calling :code:`mcp_generate_error_message`:

.. code-block:: cpp
    :caption: server.cpp

    bool on_client_request_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params) override
    {
        if(in_method == "custom_method")
        {
            mbase::Json errorData;
            errorData["random_data"] = "data";
            mbase::string generatedPayload = mbase::mcp_generate_error_message(in_msgid, MBASE_MCP_INTERNAL_ERROR, "Example error message", errorData);
            in_client->send_mcp_payload(generatedPayload);
            return true;
        }
    }

When it comes to notification handling, sending a custom response is not possible
since the notification message doesn't have an message id associated with them.

Instead, you may do the following:

.. code-block:: cpp
    :caption: server.cpp

    void on_client_notification_t(mbase::McpServerClient* in_client, const mbase::string& in_method, const mbase::Json& in_params) override
    {
        if(in_method == "custom_notif_1")
        {
            // do stuff
        }
        else if(in_method == "custom_notif_2")
        {
            // do other stuff
        }
        else if(in_method == "custom_notif_N")
        {
            // do different stuff
        }
    }

.. _mcp-server-custom-request-generation:

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Sending Custom Requests/Notifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Writing a custom request/notification is basically generating an
MCP payload by calling one of the procedures that are defined under the :code:`mcp_packet_parsing.h` file
and sending the generated payload.

Here are the payload generation procedures:

.. code-block:: cpp
    :caption: mcp_packet_parsing.h

    mbase::string mcp_generate_notification(
        const mbase::string& in_method,
        const mbase::Json& in_params = mbase::Json()
    );

    mbase::string mcp_create_request(
        const mbase::string& in_id,
        const mbase::string& in_method,
        const mbase::Json& in_params = mbase::Json()
    );

As an example, we will send a custom request to the client during a tool feature callback:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResponseTool echo(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::Json customParams;
        customParams["param"] = "hello param";
        mbase::string generatedPayload = mbase::mcp_create_request(mbase::string::generate_uuid(), "custom_method", customParams);
        in_client_instance->send_mcp_payload(generatedPayload);

        mbase::McpResponseTextTool toolResponse;
        toolResponse.mText = "Random tool response";
        return toolResponse;
    }

Or notification:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResponseTool echo(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::Json customParams;
        customParams["param"] = "hello param";
        mbase::string generatedPayload = mbase::mcp_generate_notification("custom_notification", customParams);
        in_client_instance->send_mcp_payload(generatedPayload);

        mbase::McpResponseTextTool toolResponse;
        toolResponse.mText = "Random tool response";
        return toolResponse;
    }

----------
MCP Client
----------

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Handling Custom Requests/Notifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Following request/notification transport callbacks are defined under the :code:`McpClientBase` class:

.. code-block:: cpp
    :caption: mcp_client_base.h

    class MBASE_API McpClientBase {
    public:
        ...
        virtual bool on_server_request_t(McpServerStateBase* in_server, const mbase::Json& in_msgid, const mbase::string& in_method, const mbase::Json& in_params)
        {
            /*
                IMPORTANT: Server to client requests are only possible on STDIO and SSE transports.

                Invoked when a request is sent from the server.

                Returning false will make the MCP client behave as normal which is processing the
                request and appending it to the MCP message queue waiting for application thread
                to dispatch.

                If you return true from this method,
                the current request's responsibility is on your own.
                Returning true also indicates that you are writing a custom request handler.

                See: TODO -> Link about custom request handling

                See: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages
            */
        }
        virtual GENERIC on_server_notification_t(McpServerStateBase* in_server, const mbase::string& in_method, const mbase::Json& in_params)
        {
            /*
                Invoked when a notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages
            */
        }
        ...
    };

The workflow and concepts are same as handling requests/notifications in server.
See: :ref:`mcp-server-custom-request-handle`.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Sending Custom Responses/Notifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The workflow is same with the server.
See: :ref:`mcp-server-custom-request-generation`.