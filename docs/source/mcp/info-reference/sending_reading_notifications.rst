=============================
Sending/Reading Notifications
=============================

----------
MCP Server
----------

^^^^^^^^^^^^^^^^^^^^^^
Notification Callbacks
^^^^^^^^^^^^^^^^^^^^^^

Following notification callbacks are defined under the :code:`McpServerBase` class:

.. code-block:: cpp
    :caption: mcp_server_base.h

    class MBASE_API McpServerBase : public mbase::logical_processor {
    public:
        ...

        // Fundamental
        virtual GENERIC on_client_notification_t(mbase::McpServerClient* in_client, const mbase::string& in_method, const mbase::Json& in_params)
        {
            /*
                Invoked when a notification is sent from the client.

                See: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages
            */
        }

        // Client notifications
        virtual GENERIC on_client_init(mbase::McpServerClient* in_client)
        {
            /*
                Invoked when a "notifications/initialized" notification is sent from the client.

                See: https://modelcontextprotocol.io/specification/2025-03-26/basic/lifecycle
            */
        }
        virtual GENERIC on_roots_list_changed(mbase::McpServerClient* in_client)
        {
            /*
                Invoked when a "notifications/roots/list_changed" is sent from the client.

                See: https://modelcontextprotocol.io/specification/2025-03-26/client/roots
            */
        }
        ...
    private:
        ...
    };

^^^^^^^^^^^^^^^^^^^^
Sending Log Messages
^^^^^^^^^^^^^^^^^^^^

The :code:`send_log` method is defined under the :code:`McpServerClient` class:

.. code-block:: cpp
    :caption: mcp_server_client_state.h

    class MBASE_API McpServerClient : public mbase::logical_processor {
    public:
        ...
        GENERIC send_log(const McpNotificationLogMessage& in_log);
        ...
    };

Where, the :code:`McpNotificationLogMessage` is defined as:

.. code-block:: cpp
    :caption: mcp_notifications.h

    struct McpNotificationLogMessage {
        // Spec: https://modelcontextprotocol.io/specification/2024-11-05/server/utilities/logging
        mbase::mcp_log_levels mLevel;
        mbase::string mLogger;
        mbase::string mError;
        mbase::Json mDetails;
    };

Which implies a usage such as:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResponseTool echo(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpNotificationLogMessage logMsg;
        logMsg.mLogger = "example logger";
        logMsg.mError = "Logging a message";
        in_client_instance->send_log(logMsg);
        mbase::sleep(500);
        logMsg.mError = "Sending another log message";
        in_client_instance->send_log(logMsg);

        mbase::McpResponseTextTool toolResponse;
        toolResponse.mText = std::get<mbase::string>(in_msg_map.at("user_message"));
        return toolResponse;
    }

Or:

.. code-block:: cpp
    :caption: server.cpp

    class ExampleDerivation : public mbase::McpServerStdio {
    public:
        ...
        void on_client_init(mbase::McpServerClient* in_client) override
        {
            mbase::McpNotificationLogMessage logMsg;
            logMsg.mLogger = "init logger";
            logMsg.mError = "Init notification echo";
            in_client->send_log(logMsg);
        }
        ...
    };

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Sending Progress Notifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. important:: 
    Example is from :ref:`mcp-server-quickstart-resource`.

The :code:`set_progress` method is defined under the :code:`McpServerClient` class:

.. code-block:: cpp
    :caption: mcp_server_client_state.h

    class MBASE_API McpServerClient : public mbase::logical_processor {
    public:
        ...
        GENERIC set_progress(const I32& in_progress, const mbase::Json& in_token, const mbase::string& in_message = mbase::string());
        ...
    };

Which implies a usage such as:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResponseResource content_file_uri(mbase::McpServerClient* in_client_instance, const mbase::Json& in_progress_token)
    {
        mbase::McpNotificationLogMessage logMsg;
        logMsg.mLogger = "resource logger";
        logMsg.mError = "Reading content.txt ...";
        in_client_instance->send_log(logMsg);
        mbase::sleep(500);
        in_client_instance->set_progress(25, in_progress_token, "25%");
        mbase::sleep(500);
        in_client_instance->set_progress(50, in_progress_token, "50%");
        mbase::sleep(500);
        in_client_instance->set_progress(75, in_progress_token, "75%");
        mbase::sleep(500);
        in_client_instance->set_progress(100, in_progress_token, "100%");
        mbase::McpResponseTextResource textResourceResponse;
        textResourceResponse.mMimeType = "text/plain";
        textResourceResponse.mText = "This is the content inside the content.txt";

        return textResourceResponse;
    }

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
About List Changed Notifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The list changed notification will be sent automatically when you register or unregister a feature while the MCP server is running.

----------
MCP Client
----------

^^^^^^^^^^^^^^^^^^^^^^
Notification Callbacks
^^^^^^^^^^^^^^^^^^^^^^

Following notification callbacks are defined under the :code:`McpClientBase` class:

.. code-block:: cpp
    :caption: mcp_client_base.h

    class MBASE_API McpClientBase {
    public:
        ...
        // Fundamentals
        virtual GENERIC on_server_notification_t(McpServerStateBase* in_server, const mbase::string& in_method, const mbase::Json& in_params)
        {
            /*
                Invoked when a notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages
            */
        }

        // Notifications
        virtual GENERIC on_log_message_t(McpServerStateBase* in_server, const mbase::McpNotificationLogMessage& in_log_message)
        {
            /*
                Invoked when a "notifications/message" notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2025-03-26/server/utilities/logging
            */
        }
        virtual GENERIC on_cancellation_t(McpServerStateBase* in_server, const mbase::McpNotificationCancellationIdStr& in_cancellation)
        {
            /*
                Invoked when a "notifications/cancelled" notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2025-03-26/basic/utilities/cancellation
            */
        }
        virtual GENERIC on_prompt_list_changed_t(McpServerStateBase* in_server)
        {
            /*
                Invoked when a "notifications/prompts/list_changed" notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2025-03-26/server/prompts#list-changed-notification
            */
        }
        virtual GENERIC on_resource_list_changed_t(McpServerStateBase* in_server)
        {
            /*
                Invoked when a "notifications/resources/list_changed" notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2025-03-26/server/resources#list-changed-notification
            */
        }
        virtual GENERIC on_tool_list_changed_t(McpServerStateBase* in_server)
        {
            /*
                Invoked when a "notifications/tools/list_changed" notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2025-03-26/server/tools#list-changed-notification
            */
        }
        virtual GENERIC on_resource_updated_t(McpServerStateBase* in_server, const mbase::string& in_uri)
        {
            /*
                Invoked when a "notifications/resources/updated" notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2025-03-26/server/resources#subscriptions
            */
        }
        virtual GENERIC on_progress_notification_t(McpServerStateBase* in_server, const mbase::McpNotificationProgress& in_progress_notif)
        {
            /*
                Invoked when a "notifications/progress" notification is sent from the server.

                See: https://modelcontextprotocol.io/specification/2025-03-26/basic/utilities/progress
            */
        }
    };

^^^^^^^^^^^^^^^^^^^^
Setting-up Log Level
^^^^^^^^^^^^^^^^^^^^

The :code:`set_log` method is defined under the :code:`McpServerStateBase` class:

.. code-block:: cpp
    :caption: mcp_client_server_state.h

    class MBASE_API McpServerStateBase : public mbase::logical_processor {
    public:
        ...
        bool set_log(mcp_log_levels in_level, mcp_empty_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
        ...
    };

Where the :code:`mcp_log_levels` enum declared as:

.. code-block:: cpp
    :caption: mcp_common.h

    enum class mcp_log_levels {
        L_DEBUG,
        L_INFO,
        L_NOTICE,
        L_WARNING,
        L_ERROR,
        L_CRITICAL,
        L_ALERT,
        L_EMERGENCY
    };

Which implies a usage such as:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.set_log(mbase::mcp_log_levels::L_DEBUG, [](const int& errCode, mbase::McpClientBase* self_client_instance){
        if(errCode == MBASE_MCP_SUCCESS)
        {
            std::cout << "Logging level set!" << std::endl;
        }
    });

