=============================
MCP Client Features in Detail
=============================

In order to extend the functionality of the MCP client, you should extend
the :code:`McpClientBase` which is defined under the :code:`mcp_client_base.h`.

Here is what the declaration looks like:

.. code-block:: cpp
    :caption: mcp_client_base.h

    #include <mbase/mcp/mcp_common.h>
    #include <mbase/mcp/mcp_notifications.h> // notification objects
    #include <mbase/mcp/mcp_server_to_client_requests.h> // McpSamplingRequestObject, McpSamplingResult

    class MBASE_API McpClientBase {
        ...
    };

-------------------
Transport Callbacks
-------------------

:code:`McpClientBase` has overridable transport callbacks that are invoked on the transport thread.
These callbacks are called before the MCP message is dispatched to the application thread.
You can override the following callbacks:

.. code-block:: cpp
    :caption: mcp_client_base.h

    class MBASE_API McpClientBase {
    public:
        ...
        // Fundamentals
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
        virtual GENERIC on_server_response_t(McpServerStateBase* in_server, const mbase::string& in_msgid, const mbase::Json& in_params)
        {
            /*
                Invoked when a response is sent from the server.

                Unless you have implemented a custom request/response handling routine, 
                server will send a response to the following client to server requests:

                - InitializeRequest = initialize
                - PingRequest = ping
                - ListResourcesRequest = resources/list
                - ListPromptsRequest = prompts/list
                - ListToolsRequest = tools/list
                - SubscribeRequest = resources/subscribe
                - UnsubscribeRequest = resources/unsubscribe
                - CompletionRequest = completion/complete
                - SetLevelRequest = logging/setLevel
                - ReadResourceRequest = resources/read
                - GetPromptRequest = prompts/get
                - CallToolRequest = tools/call

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

        // Sampling request callbacks
        GENERIC on_sampling_request_t(McpServerStateBase* in_server, const mbase::McpSamplingRequestObject& in_sampling_request)
        {
            /*
                Invoked when a sampling request is received from the server.

                See: https://modelcontextprotocol.io/specification/2025-03-26/client/sampling
            */ 
        }

    };

------------------
Working with Tools
------------------

Following are the tool related methods that are defined under the :code:`McpServerStateBase` class:

.. code-block:: cpp
    :caption: mcp_client_server_state.h 

    class MBASE_API McpServerStateBase : public mbase::logical_processor {
    public:
        ...
        // Returns true if the tooling is supported
        bool is_tooling_supported() const noexcept;

        // Returns true if the tool call initiated
        bool tool_call(const mbase::string& in_tool_name, mcp_tool_call_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT, const McpToolMessageMap& in_arguments = McpToolMessageMap());
        bool tool_call(const mbase::string& in_tool_name, mcp_tool_call_cb in_cb, const I64& in_timeout, const McpToolMessageMap& in_arguments, mbase::string& out_progress_token);
        ...
    };

Where the :code:`mcp_tool_call_cb` callback defined as:

.. code-block:: cpp
    :caption: mcp_client_callbacks.h

    // err code, self client instance, tool response
    using mcp_tool_call_cb = std::function<GENERIC(const I32&, McpClientBase*, mbase::vector<McpResponseTool>&&, bool)>; 

And :code:`McpToolMessageMap` as: 

.. code-block:: cpp
    :caption: mcp_client_arguments.h

    using McpToolMessageArgument = std::variant<I64, F64, bool, mbase::string, mbase::vector<mbase::Json>, std::map<mbase::string, mbase::Json>>;
    using McpToolMessageMap = mbase::unordered_map<mbase::string, McpToolMessageArgument>;

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Listing Tools with/without Pagination
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here is an example below where we list the tools when the client recevies a tool lists changed notification with pagination:

.. code-block:: cpp
    :caption: client.cpp

    mbase::McpServerStateBase* gConnectedServer = nullptr;

    void toolListCb(const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpToolDescription>&& tools_list, const mbase::string& pagination_token)
    {
        for(const mbase::McpToolDescription& tmpDesc : tools_list)
        {
            std::cout << "Tool name: " << tmpDesc.mName << std::endl;
            std::cout << "Description: " << tmpDesc.mDescription << std::endl;
            std::cout << "Arguments: " << std::endl;
            for(const mbase::McpToolArgument& tmpArg : tmpDesc.mArguments)
            {
                std::cout << "\tName: " << tmpArg.mArgumentName << std::endl;
                std::cout << "\tDescription: " << tmpArg.mDescription << std::endl;
                std::cout << "\tRequired: " << tmpArg.mIsRequired << std::endl;
                std::cout << "\tType: ";
                switch (tmpArg.mArgType)
                {
                case mbase::McpValueType::ARRAY:
                    std::cout << "Array" << std::endl;
                    break;
                case mbase::McpValueType::BOOL:
                    std::cout << "Boolean" << std::endl;
                    break;
                case mbase::McpValueType::JSON:
                    std::cout << "Object" << std::endl;
                    break;
                case mbase::McpValueType::NUMBER:
                    std::cout << "Number" << std::endl;
                    break;
                case mbase::McpValueType::STRING:
                    std::cout << "String" << std::endl;
                    break;
                default:
                    break;
                }
                std::cout << "==" << std::endl;
            }
        }
        std::cout << std::endl;
        if(pagination_token.size())
        {
            gConnectedServer->list_tools(toolListCb, MBASE_MCP_TIMEOUT_DEFAULT, pagination_token);
        }
    }

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ...
        void on_tool_list_changed_t(mbase::McpServerStateBase* in_server) override
        {
            gConnectedServer = in_server;
            in_server->list_tools(toolListCb);
        }
        ...
    };

Without pagination:

.. code-block:: cpp
    :caption: client.cpp

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ...
        void on_tool_list_changed_t(mbase::McpServerStateBase* in_server) override
        {
            in_server->list_tools([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpToolDescription>&& tools_list, const mbase::string& pagination_token){
                for(const mbase::McpToolDescription& tmpDesc : tools_list)
                {
                    std::cout << "Tool name: " << tmpDesc.mName << std::endl;
                    std::cout << "Description: " << tmpDesc.mDescription << std::endl;
                    std::cout << "Arguments: " << std::endl;
                    for(const mbase::McpToolArgument& tmpArg : tmpDesc.mArguments)
                    {
                        std::cout << "\tName: " << tmpArg.mArgumentName << std::endl;
                        std::cout << "\tDescription: " << tmpArg.mDescription << std::endl;
                        std::cout << "\tRequired: " << tmpArg.mIsRequired << std::endl;
                        std::cout << "\tType: ";
                        switch (tmpArg.mArgType)
                        {
                        case mbase::McpValueType::ARRAY:
                            std::cout << "Array" << std::endl;
                            break;
                        case mbase::McpValueType::BOOL:
                            std::cout << "Boolean" << std::endl;
                            break;
                        case mbase::McpValueType::JSON:
                            std::cout << "Object" << std::endl;
                            break;
                        case mbase::McpValueType::NUMBER:
                            std::cout << "Number" << std::endl;
                            break;
                        case mbase::McpValueType::STRING:
                            std::cout << "String" << std::endl;
                            break;
                        default:
                            break;
                        }
                        std::cout << "==" << std::endl;
                    }
                }
                std::cout << std::endl;
            });
        }
        ...
    };

^^^^^^^^^^^^^
Calling Tools
^^^^^^^^^^^^^

.. important:: 
    Example is from :ref:`mcp-client-quickstart-calling-tools`.

Calling tools with arguments:

.. code-block:: cpp
    :caption: client.cpp

    mbase::McpToolMessageArgument intNum1 = 10;
    mbase::McpToolMessageArgument intNum2 = 20;
    mbase::McpToolMessageArgument floatNum1 = 10.5f;
    mbase::McpToolMessageArgument floatNum2 = 20.5f;
    mbase::McpToolMessageArgument echoMessage = "Hello world!";

    mbase::McpToolMessageMap argMap;
    argMap["num1"] = intNum1;
    argMap["num2"] = intNum2;

    mcpServerState.tool_call("add_int64", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){
        mbase::McpResponseTextTool textResponse = std::get<mbase::McpResponseTextTool>(toolResponse[0]);
        std::cout << textResponse.mText << std::endl;
    }, MBASE_MCP_TIMEOUT_DEFAULT, argMap);

    argMap.clear();
    argMap["num1"] = floatNum1;
    argMap["num2"] = floatNum2;

    mcpServerState.tool_call("add_float64", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){
        mbase::McpResponseTextTool textResponse = std::get<mbase::McpResponseTextTool>(toolResponse[0]);
        std::cout << textResponse.mText << std::endl;
    }, MBASE_MCP_TIMEOUT_DEFAULT, argMap);

    argMap.clear();
    argMap["user_message"] = echoMessage;

    mcpServerState.tool_call("echo", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){
        mbase::McpResponseTextTool textResponse = std::get<mbase::McpResponseTextTool>(toolResponse[0]);
        std::cout << textResponse.mText << std::endl;            
    }, MBASE_MCP_TIMEOUT_DEFAULT, argMap);

Without arguments:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.tool_call("example_tool_1", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){

    });

    mcpServerState.tool_call("example_tool_2", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){

    });

    mcpServerState.tool_call("example_tool_3", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){

    });

Where the :code:`McpResponseTool` variant defined as:

.. code-block:: cpp
    :caption: mcp_server_responses.h

    struct McpResponseTextTool {
        mbase::string mType = "text";
        mbase::string mText;
    };

    struct McpResponseImageTool {
        mbase::string mType = "image";
        mbase::string mMimeType; // "image/png" ...
        mbase::string mData; // base64-encoded-image-data
    };

    struct McpResponseAudioTool {
        mbase::string mType = "audio";
        mbase::string mMimeType; // "audio/wav" ...
        mbase::string mData; // base64-encoded-audio-data
    };

    using McpResponseTool = std::variant<McpResponseTextTool, McpResponseImageTool, McpResponseAudioTool>;

----------------------
Working with Resources
----------------------

Following are the resource related methods that are defined under the :code:`McpServerStateBase` class:

.. code-block:: cpp
    :caption: mcp_client_server_state.h

    class MBASE_API McpServerStateBase : public mbase::logical_processor {
    public:
        ...
        // Returns true if the feature is supported
        bool is_resource_supported() const noexcept;
        bool is_resource_subscription_supported() const noexcept;

        // Returns true if the operation is initiated
        bool read_resource(const mbase::string& in_uri, mcp_read_resource_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
        bool read_resource(const mbase::string& in_uri, mcp_read_resource_cb in_cb, mbase::string& out_progress_token, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
        bool subscribe(const mbase::string& in_uri, mcp_empty_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
        bool unsubscribe(const mbase::string& in_uri, mcp_empty_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
        ...
    };

Where the :code:`mcp_read_resource_cb` defined as: 

.. code-block:: cpp
    :caption: mcp_client_callbacks.h

    using mcp_read_resource_cb = std::function<GENERIC(const I32&, McpClientBase*, mbase::vector<McpResponseResource>&&)>;

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Listing Resources with/without Pagination
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here is an example below where we list the resources when the client recevies a resource lists changed notification with pagination:

.. code-block:: cpp
    :caption: client.cpp

    mbase::McpServerStateBase* gConnectedServer = nullptr;

    void resourceListCb(const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResourceDescription>&& resources_list, const mbase::string& pagination_token)
    {
        for(const mbase::McpResourceDescription& resourceDesc : resources_list)
        {
            std::cout << "Name: " << resourceDesc.mName << std::endl;
            std::cout << "Uri: " << resourceDesc.mUri << std::endl;
            std::cout << "Description: " << resourceDesc.mDescription << std::endl;
            std::cout << "Mime type: " << resourceDesc.mMimeType << std::endl;
            std::cout << "Size: " << resourceDesc.mSize << std::endl;
            std::cout << std::endl;    
        }

        if(pagination_token.size())
        {
            gConnectedServer->list_resources(resourceListCb, MBASE_MCP_TIMEOUT_DEFAULT, pagination_token);
        }
    }

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ...
        void on_resource_list_changed_t(mbase::McpServerStateBase* in_server) override
        {
            gConnectedServer = in_server;
            in_server->list_resources(resourceListCb);
        }
        ...
    };

Without pagination:

.. code-block:: cpp
    :caption: client.cpp

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ...
        void on_resource_list_changed_t(mbase::McpServerStateBase* in_server) override
        {
            in_server->list_resources([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResourceDescription>&& resources_list, const mbase::string& pagination_token){
                for(const mbase::McpResourceDescription& resourceDesc : resources_list)
                {
                    std::cout << "Name: " << resourceDesc.mName << std::endl;
                    std::cout << "Uri: " << resourceDesc.mUri << std::endl;
                    std::cout << "Description: " << resourceDesc.mDescription << std::endl;
                    std::cout << "Mime type: " << resourceDesc.mMimeType << std::endl;
                    std::cout << "Size: " << resourceDesc.mSize << std::endl;
                    std::cout << std::endl;    
                }
            });
        }
        ...
    };

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Subscribing/Unsubscribing Resources
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Subscribing:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.subscribe("file:///resource.txt", [&](const int& errCode, mbase::McpClientBase* self_client_instance){
        if(errCode == MBASE_MCP_SUCCESS)
        {
            std::cout << "Subscribed to a resource!" << std::endl;
        }
    });

Unsubscribing:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.unsubscribe("file:///resource.txt", [&](const int& errCode, mbase::McpClientBase* self_client_instance){
        if(errCode == MBASE_MCP_SUCCESS)
        {
            std::cout << "Unsubscribed to a resource!" << std::endl;
        }
    });

^^^^^^^^^^^^^^^^^
Reading Resources
^^^^^^^^^^^^^^^^^

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.read_resource("file:///resource.txt", [](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseResource>&& resourceResponse){

    });

Where the :code:`McpResponseResource` variant is defined as:

.. code-block:: cpp
    :caption: mcp_server_responses.h

    struct McpResponseTextResource {
        mbase::string mUri;
        mbase::string mMimeType;
        mbase::string mText;
    };

    struct McpResponseBinaryResource {
        mbase::string mUri;
        mbase::string mMimeType;
        mbase::string mBlob;
    };

    using McpResponseResource = std::variant<McpResponseTextResource, McpResponseBinaryResource>;

--------------------
Working with Prompts
--------------------

Following are the prompt related methods that are defined under the :code:`McpServerStateBase` class:

.. code-block:: cpp
    :caption: mcp_client_server_state.h

    class MBASE_API McpServerStateBase : public mbase::logical_processor {
    public:
        ...
        // Returns true if the feature is supported
        bool is_prompt_supported() const noexcept;

        // Returns true if the operation is initiated
        bool get_prompt(const mbase::string& in_prompt_name, mcp_get_prompt_cb in_cb, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT, const McpPromptMessageMap& in_arguments = McpPromptMessageMap());
        bool get_prompt(const mbase::string& in_prompt_name, mcp_get_prompt_cb in_cb, const I64& in_timeout, const McpPromptMessageMap& in_arguments, mbase::string& out_progress_token);
        bool prompt_compilation(mcp_prompt_compilation_cb in_cb, const mbase::string& in_prompt_name, const mbase::string& in_argument_name, const mbase::string& in_argument_value, const I64& in_timeout = MBASE_MCP_TIMEOUT_DEFAULT);
        ...
    };

Where the :code:`mcp_get_prompt_cb` and :code:`mcp_prompt_compilation_cb` callbacks are defined as:

.. code-block:: cpp
    :caption: mcp_client_callbacks.h

    using mcp_get_prompt_cb = std::function<GENERIC(const I32&, McpClientBase*, const mbase::string&, mbase::vector<McpResponsePrompt>&&)>;
    using mcp_prompt_compilation_cb = std::function<GENERIC(const I32&, McpClientBase*, const McpPromptCompilationResult&)>;

Where the :code:`McpPromptCompilationResult` and :code:`McpResponsePrompt` objects are defined as:

.. code-block:: cpp
    :caption: mcp_server_responses.h

    struct McpResponseTextPrompt {
        mbase::string mRole = "user";
        mbase::string mType = "text";
        mbase::string mText;
    };

    struct McpResponseImagePrompt {
        mbase::string mRole = "user";
        mbase::string mType = "image";
        mbase::string mMimeType; // "image/png" ...
        mbase::string mData; // base64-encoded-image-data
    };

    struct McpResponseAudioPrompt {
        mbase::string mRole = "user";
        mbase::string mType = "audio";
        mbase::string mMimeType; // audio/wav etc.
        mbase::string mData; // base-64-encoded-audio-data
    };

    struct McpPromptCompilationResult {
        mbase::vector<mbase::string> mValues;
        I32 mTotal = 0;
        bool mHasMore = false;
    };

    using McpResponsePrompt = std::variant<McpResponseTextPrompt, McpResponseImagePrompt, McpResponseAudioPrompt>;

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Listing Prompts with/without Pagination
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here is an example below where we list the prompts when the client receives a prompt lists changed notification with pagination:

.. code-block:: cpp
    :caption: client.cpp

    mbase::McpServerStateBase* gConnectedServer = nullptr;

    void promptListCb(const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpPromptDescription>&& prompts_list, const mbase::string& pagination_token)
    {
        for(const mbase::McpPromptDescription& tmpDescription : prompts_list)
        {
            std::cout << "Name: " << tmpDescription.mName << std::endl;
            std::cout << "Description" << tmpDescription.mDescription << std::endl;
            std::cout << "Arguments: " << std::endl;
            for(const mbase::McpPromptArgument& tmpArgument : tmpDescription.mArguments)
            {
                std::cout << "\tName: " << tmpArgument.mArgumentName << std::endl;
                std::cout << "\tDescription: " << tmpArgument.mDescription << std::endl;
                std::cout << "\tIs Required: " << tmpArgument.mIsRequired << std::endl;
                std::cout << "======" << std::endl;
            }

            std::cout << std::endl;
        }

        if(pagination_token.size())
        {
            gConnectedServer->list_prompts(promptListCb, MBASE_MCP_TIMEOUT_DEFAULT, pagination_token);
        }
    }

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ...
        void on_prompt_list_changed_t(mbase::McpServerStateBase* in_server) override
        {
            gConnectedServer = in_server;
            in_server->list_prompts(promptListCb);
        }
        ...
    };

Without pagination:

.. code-block:: cpp
    :caption: client.cpp

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ...
        void on_prompt_list_changed_t(mbase::McpServerStateBase* in_server) override
        {
            in_server->list_prompts([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpPromptDescription>&& prompts_list, const mbase::string& pagination_token){
                for(const mbase::McpPromptDescription& tmpDescription : prompts_list)
                {
                    std::cout << "Name: " << tmpDescription.mName << std::endl;
                    std::cout << "Description" << tmpDescription.mDescription << std::endl;
                    std::cout << "Arguments: " << std::endl;
                    for(const mbase::McpPromptArgument& tmpArgument : tmpDescription.mArguments)
                    {
                        std::cout << "\tName: " << tmpArgument.mArgumentName << std::endl;
                        std::cout << "\tDescription: " << tmpArgument.mDescription << std::endl;
                        std::cout << "\tIs Required: " << tmpArgument.mIsRequired << std::endl;
                        std::cout << "======" << std::endl;
                    }

                    std::cout << std::endl;
                }
            });
        }
        ...
    };

^^^^^^^^^^^^^^^
Getting Prompts
^^^^^^^^^^^^^^^

.. important:: 
    Example is from :ref:`mcp-client-quickstart-getting-prompts`.

Getting a prompt with arguments:

.. code-block:: cpp
    :caption: client.cpp

    mbase::McpPromptMessageMap promptArgMap;
    promptArgMap["greet_text"] = "Hello developer!";
    promptArgMap["mbase_arg"] = "mcp-sdk";

    mcpServerState.get_prompt("greeting_prompt", [](const int& errCode, mbase::McpClientBase* self_client_instance, const mbase::string& prompt_description, mbase::vector<mbase::McpResponsePrompt>&& promptResponse) {
        mbase::McpResponseTextPrompt textPromptRes = std::get<mbase::McpResponseTextPrompt>(promptResponse[0]);
        std::cout << "Role: " << textPromptRes.mRole << std::endl;
        std::cout << "Prompt: " << textPromptRes.mText << std::endl;
    }, MBASE_MCP_TIMEOUT_DEFAULT, promptArgMap);

    mcpServerState.get_prompt("mbase_sdk_inform", [](const int& errCode, mbase::McpClientBase* self_client_instance, const mbase::string& prompt_description, mbase::vector<mbase::McpResponsePrompt>&& promptResponse) {
        mbase::McpResponseTextPrompt textPromptRes = std::get<mbase::McpResponseTextPrompt>(promptResponse[0]);
        std::cout << "Role: " << textPromptRes.mRole << std::endl;
        std::cout << "Prompt: " << textPromptRes.mText << std::endl;
    }, MBASE_MCP_TIMEOUT_DEFAULT, promptArgMap);

Without arguments:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.get_prompt("example_prompt_get_1", [](const int& errCode, mbase::McpClientBase* self_client_instance, const mbase::string& prompt_description, mbase::vector<mbase::McpResponsePrompt>&& promptResponse) {
            
    });

    mcpServerState.get_prompt("example_prompt_get_2", [](const int& errCode, mbase::McpClientBase* self_client_instance, const mbase::string& prompt_description, mbase::vector<mbase::McpResponsePrompt>&& promptResponse) {
        
    });

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Sending Prompt Compilation Requests
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.prompt_compilation([](const int& errCode, mbase::McpClientBase* self_client_instance, const mbase::McpPromptCompilationResult& compilationResult){
        for(const mbase::string& completedString : compilationResult.mValues)
        {
            std::cout << completedString << std::endl;
        }
        if(compilationResult.mHasMore)
        {
            // There are more strings to be completed   
        }
    }, "example_prompt_get_1", "prompt_arg_1", "random_value");

---------------------
Adding/Removing Roots
---------------------

Adding and removing roots will send a roots list changed notification to the server.

Here is how you add/remove roots:

.. code-block:: cpp
    :caption: client.cpp

    myMcpClient.add_root("file:///example1.txt", "example_file_root1");
    myMcpClient.add_root("file:///example2.txt", "example_file_root2");
    myMcpClient.add_root("http://www.example.com", "example_web_root");
    myMcpClient.add_root("git://example.git", "example_git_root");

    myMcpClient.remove_root("file:///example1.txt");
    myMcpClient.remove_root("file:///example2.txt");
    myMcpClient.remove_root("http://www.example.com");
    myMcpClient.remove_root("git://example.git");

-------------------------
Sampling Request Handling
-------------------------

Below are the methods that are related to the sampling request handling:

.. code-block:: cpp
    :caption: mcp_client_base.h

    class MBASE_API McpClientBase {
    public:
        McpClientBase(
            const mbase::string& in_client_name,
            const mbase::string& in_client_version,
            const bool& in_sampling_supported = false // set this as true
        );

        // Returns true if the sampling is supported
        const bool& is_sampling_supported() const noexcept;
        ...

        virtual bool on_sampling_request(McpServerStateBase* in_server, mbase::McpSamplingRequestObject&& in_sampling_request);
        GENERIC send_sampling_result(const mbase::McpSamplingRequestObject& in_sampling_request, const mbase::McpSamplingResult& in_result);
        ...
    };

Where the :code:`McpSamplingRequestObject` and :code:`McpSamplingResult` are defined as: 

.. code-block:: cpp
    :caption: mcp_server_to_client_requests.h

    struct McpSamplingRequestObject {
        mbase::Json mRequestId;
        McpServerStateBase* requestOwner = nullptr;
        McpSamplingRequest samplingRequest;
    };

    struct McpSamplingResult {
        mbase::string mRole;
        mbase::string mModel;
        mbase::string mStopReason;
        mcp_sampling_content_type mContentType = mcp_sampling_content_type::TEXT;
        mbase::string mTextContent;
        mbase::string mBase64Content;
        mbase::string mMimeType;
    };

The sampling request handling workflow is as follows:

1. Initialize the :code:`McpClientBase` with sampling supported params as true.
2. Override the :code:`on_sampling_request` method of the :code:`McpClientBase`.
3. Return false on :code:`on_sampling_request` method if you reject the sampling request.
4. Return true if you accept the sampling request and own the :code:`McpSamplingRequestObject`.
5. Call the :code:`send_sampling_result` method of the :code:`McpClientBase` with the :code:`McpSamplingRequestObject` you owned before.

^^^^^^^^^
In Action
^^^^^^^^^

Let's derive from the :code:`McpClientBase` and declare the class as follows:

.. code-block:: cpp
    :caption: client.cpp

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ExampleClientDerivation() : mbase::McpClientBase("MCP Example Client", "1.0.0", true){}
    };

Override the :code:`on_sampling_request`:

.. code-block:: cpp
    :caption: client.cpp

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ExampleClientDerivation() : mbase::McpClientBase("MCP Example Client", "1.0.0"){}
        bool on_sampling_request(mbase::McpServerStateBase* in_server, mbase::McpSamplingRequestObject&& in_sampling_request) override
        {
            /*
                Return false if you want to reject the sampling request
            */
        }
    };

Call the :code:`send_sampling_result`:

.. code-block:: cpp
    :caption: client.cpp

    class ExampleClientDerivation : public mbase::McpClientBase {
    public:
        ExampleClientDerivation() : mbase::McpClientBase("MCP Example Client", "1.0.0"){}
        bool on_sampling_request(mbase::McpServerStateBase* in_server, mbase::McpSamplingRequestObject&& in_sampling_request) override
        {
            mbase::McpSamplingResult samplingResult;
            samplingResult.mTextContent = "This is a sampling response from LLM";
            samplingResult.mContentType = mbase::mcp_sampling_content_type::TEXT;
            samplingResult.mMimeType = "text/plain";
            samplingResult.mRole = "assistant";
            samplingResult.mStopReason = "EOT";
            samplingResult.mModel = "<some_llm_model>";

            this->send_sampling_result(in_sampling_request, samplingResult);

            return true;
        }
    };

^^^^^^^^^^^^^^^^^
Important Remarks
^^^^^^^^^^^^^^^^^

* The :code:`on_sampling_request` method is called on the application thread so that you don't need to worry about thread synchronization.

* While your client application is running, you may send the sampling response at any time, provided you still own the :code:`McpSamplingRequestObject`.

* If you return true from the :code:`on_sampling_request` but don't own the :code:`McpSamplingRequestObject`, the request will stale on the server side which will inevitably result in request timeout.