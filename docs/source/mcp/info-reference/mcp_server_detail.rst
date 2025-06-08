=============================
MCP Server Features in Detail
=============================

There are three essential header files you should consider when working with 
the MCP server you implement.

Those headers are:

* :code:`mcp_server_base.h`: Contains the fundamental :code:`McpServerBase` class. User shouldn't extend directly from the base but should instead either from :code:`McpServerStdio`, :code:`McpServerHttpStreamableStateful` or :code:`McpServerHttpStreamableStateless`.
* :code:`mcp_server_stdio.h`: Contains the :code:`McpServerStdio` object which is used to initialize an MCP STDIO server. You must inherit from :code:`McpServerStdio` if you want to extend your MCP STDIO server functionality.
* :code:`mcp_server_http_streamable.h`: Contains both the :code:`McpServerHttpStreamableStateful` and :code:`McpServerHttpStreamableStateless` objects which are used to initialize an MCP HTTP server. You must inherit from one of those if you want to extend your MCP HTTP server functionality.

Here is what the declarations look like:

.. code-block:: cpp
    :caption: mcp_server_base.h

    #include <mbase/mcp/mcp_common.h>
    #include <mbase/framework/logical_processing.h> // Async IO
    #include <mbase/mcp/mcp_server_features.h> // Feature objects, Feature request object
    #include <mbase/mcp/mcp_server_client_state.h> // McpServerClient

    class MBASE_API McpServerBase : public mbase::logical_processor {
        ...
    };

.. code-block:: cpp
    :caption: mcp_server_stdio.h

    #include <mbase/mcp/mcp_server_base.h> // McpServerBase
    #include <mbase/mcp/mcp_server_client_stdio.h> // McpServerStdioClient

    class MBASE_API McpServerStdio : public mbase::McpServerBase {
        ...
    };

.. code-block:: cpp
    :caption: mcp_server_http_streamable.h

    #include <mbase/mcp/mcp_common.h>
    #include <mbase/mcp/mcp_server_base.h> // McpServerBase
    #include <mbase/mcp/mcp_server_client_http_streamable.h> // McpServerHttpBase
    #include <memory>

    class MBASE_API McpServerHttpBase : public mbase::McpServerBase {
        ...
    };

    class MBASE_API McpServerHttpStreamableStateful : public mbase::McpServerHttpBase {
        ...
    };

    class MBASE_API McpServerHttpStreamableStateless : public mbase::McpServerHttpBase {
        ...
    };

-------------------
Transport Callbacks
-------------------

:code:`McpServerBase` has overridable transport callbacks that are invoked on the transport thread. 
These callbacks are called before the MCP message is dispatched to the application thread. 
You can override the following callbacks:

.. code-block:: cpp
    :caption: mcp_server_base.h

    class MBASE_API McpServerBase : public mbase::logical_processor {
    public:
        ...

    // Fundamental
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
    virtual GENERIC on_client_response_t(mbase::McpServerClient* in_client, const mbase::string& in_msgid, const mbase::Json& in_params)
    {
        /*
            Invoked when a response is sent from the client.

            Unless you have implemented a custom request/response handling routine, 
            client will send a response to the following server to client requests:

            "roots/list"
            "sampling/createMessage"

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

    // Utility callbacks
    virtual GENERIC on_cancellation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_reason = mbase::string())
    {
        /*
            Invoked when a client sends a cancellation request to the server.

            The cancellation only works if the cancel requested operation id
            is not already dispatched to the application thread. Otherwise, 
            the cancellation request is ignored.

            To put it simply, if the MCP message is dispatched to the application thread, the cancellation
            request is ignored.
        */
    }
    virtual GENERIC on_ping_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid)
    {
        /*
            Invoked when a client sends a ping request.

            See: https://modelcontextprotocol.io/specification/2025-03-26/basic/utilities/ping
        */
    }
    virtual GENERIC on_logging_set_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, mcp_log_levels in_log_level)
    {
        /*
            Invoked when a client sends a logging set request.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/utilities/logging
        */
    }
    virtual GENERIC on_resource_subscribe(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri)
    {
        /*
            Invoked when a client sends a resource subscription request.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/resources#subscriptions
        */
    }
    virtual GENERIC on_resource_unsubscribe(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_uri)
    {
        /*
            Invoked when a client sends a resource unsubscription request.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/resources#subscriptions
        */
    }
    virtual GENERIC on_prompt_compilation_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_prompt, const mbase::string& in_argument_name, const mbase::string& in_argument_value)
    {
        /*
            Invoked when a client sends a prompt compilation request for an argument in a prompt.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/utilities/completion
        */
    }

    // List callbacks
    virtual GENERIC on_list_tool_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string())
    {
        /*
            Invoked when a client sends a list tools request.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/tools#listing-tools
        */
    }
    virtual GENERIC on_list_prompt_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string())
    {
        /*
            Invoked when a client sends a list prompts request.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/prompts#listing-prompts
        */
    }
    virtual GENERIC on_list_resource_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::string& in_pagination = mbase::string())
    {
        /*
            Invoked when a client sends a list resources request.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/resources#listing-resources
        */
    }

    // Invocation callbacks
    virtual GENERIC on_tool_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpToolFeature* in_tool, McpMessageMap& in_arguments)
    {
        /*
            Invoked when a client sends a tool call request.

            User-supplied tool feature callback will not be called on the transport thread.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/tools
        */
    }
    virtual GENERIC on_prompt_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpPromptFeature* in_prompt, McpMessageMap& in_arguments)
    {
        /*
            Invoked when a client sends a prompt get request.

            User-supplied prompt feature callback will not be called on the transport thread.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/prompts
        */
    }
    virtual GENERIC on_resource_call_t(mbase::McpServerClient* in_client, const mbase::Json& in_msgid, const mbase::Json& in_progress_id, McpResourceFeature* in_resource)
    {
        /*
            Invoked when a client sends a resource read request.

            User-supplied prompt feature callback will not be called on the transport thread.

            See: https://modelcontextprotocol.io/specification/2025-03-26/server/resources
        */
    }

        ...
    private:
        ...
    };

------------------
Working with Tools
------------------

Tool response object variant is defined as follows:

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

Tool feature callback signature is defined as follows:

.. code-block:: cpp
    :caption: mcp_server_features.h

    // Client instance, Message map, progress token
    typedef McpResponseTool(*mcp_server_tool_cb)(McpServerClient*, const McpMessageMap&, const mbase::Json&);

Tool argument is defined as follows:

.. code-block:: cpp
    :caption: mcp_server_arguments.h

    enum class McpValueType {
        NUMBER,
        BOOL,
        STRING,
        ARRAY,
        JSON
    };

    struct McpToolArgument {
        mbase::string mArgumentName;
        mbase::string mDescription;
        McpValueType mArgType;
        bool mIsRequired = false;
    };

Tool description object is defined as follows:

.. code-block:: cpp
    :caption: mcp_server_descriptions.h

    struct McpToolDescription {
        mbase::string mName;
        mbase::string mDescription; // Optional
        mbase::vector<mbase::McpToolArgument> mArguments; // Optional
    };


^^^^^^^^^^^^^
Writing Tools
^^^^^^^^^^^^^

Writing example callbacks:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResponseTool example_text_res(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        // Responding with text

        mbase::McpResponseTextTool textResp;
        textResp.mText = "...";

        return textResp;
    }

    mbase::McpResponseTool example_image_res(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        // Responding with image

        mbase::McpResponseImageTool imageResp;
        imageResp.mData = "base64 encoded data";
        imageResp.mMimeType = "...";

        return imageResp;
    }

    mbase::McpResponseTool example_audio_res(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        // Responding with audio

        mbase::McpResponseAudioTool audioResp;
        audioResp.mData = "base64 encoded data";
        audioResp.mMimeType = "...";

        return audioResp;
    }

Create description objects:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpToolDescription toolDesc1;
    toolDesc1.mName = "example_text_res";
    toolDesc1.mDescription = "An example text response tool";

    mbase::McpToolDescription toolDesc2;
    toolDesc2.mName = "example_image_res";
    toolDesc2.mDescription = "An example image response tool";

    mbase::McpToolDescription toolDesc3;
    toolDesc3.mName = "example_audio_res";
    toolDesc3.mDescription = "An example audio response tool";

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Registering/Unregistering Tools
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Registration:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.register_tool(toolDesc1, example_text_res);
    mcpServer.register_tool(toolDesc2, example_image_res);
    mcpServer.register_tool(toolDesc3, example_audio_res);

Unregistration:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.unregister_tool("example_text_res");
    mcpServer.unregister_tool("example_image_res");
    mcpServer.unregister_tool("example_audio_res");

----------------------
Working with Resources
----------------------

Resource response object variant is defined as follows:

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

Resource feature callback signature is defined as follows:

.. code-block:: cpp
    :caption: mcp_server_features.h

    // Client instance, progress token
    typedef McpResponseResource(*mcp_server_resource_cb)(McpServerClient*, const mbase::Json&);

Resource description object is defined as follows:

.. code-block:: cpp
    :caption: mcp_server_descriptions.h

    struct McpResourceDescription {
        mbase::string mUri;
        mbase::string mName;
        mbase::string mDescription; // Optional
        mbase::string mMimeType; // Optional
        mbase::SIZE_T mSize = 0; // Optional
    };

^^^^^^^^^^^^^^^^^
Writing Resources
^^^^^^^^^^^^^^^^^

Writing example callbacks:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResponseResource example_text_resource(mbase::McpServerClient* in_client_instance, const mbase::Json& in_progress_token)
    {
        // Responding with text

        mbase::McpResponseTextResource textResp;
        textResp.mMimeType = "text/...";
        textResp.mText = "...";
        textResp.mUri = "...";

        return textResp;
    }

    mbase::McpResponseResource example_binary_resource(mbase::McpServerClient* in_client_instance, const mbase::Json& in_progress_token)
    {
        // Responding with binary

        mbase::McpResponseBinaryResource binaryResp;
        binaryResp.mMimeType = ".../...";
        binaryResp.mBlob = "base64 encoded data";
        binaryResp.mUri = "...";

        return binaryResp;
    }

Create description objects:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResourceDescription resourceDesc1;
    resourceDesc1.mName = "example_text_resource";
    resourceDesc1.mDescription = "Example text resource";
    resourceDesc1.mUri = "...";

    mbase::McpResourceDescription resourceDesc2;
    resourceDesc2.mName = "example_binary_resource";
    resourceDesc2.mDescription = "Example binary resource";
    resourceDesc2.mUri = "...";

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Registering/Unregistering Resources
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Registration:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.register_resource(resourceDesc1, example_text_resource);
    mcpServer.register_resource(resourceDesc2, example_binary_resource);

Unregistration:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.unregister_resource("example_text_resource");
    mcpServer.unregister_resource("example_binary_resource");

--------------------
Working with Prompts
--------------------

Prompt response object variant is defined as follows:

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

    using McpResponsePrompt = std::variant<McpResponseTextPrompt, McpResponseImagePrompt, McpResponseAudioPrompt>;

Prompt feature callback signature is defined as follows:

.. code-block:: cpp
    :caption: mcp_server_features.h

    // Client instance, Message map, progress token
    typedef mbase::vector<McpResponsePrompt>(*mcp_server_prompt_cb)(McpServerClient*, const McpMessageMap&, const mbase::Json&);

Prompt argument is defined as follows:

.. code-block:: cpp
    :caption: mcp_server_arguments.h

    struct McpPromptArgument {
        mbase::string mArgumentName;
        mbase::string mDescription;
        mbase::vector<mbase::string> mCompletionStrings;
        bool mIsRequired = false;
    };


Prompt description object is defined as follows:

.. code-block:: cpp
    :caption: mcp_server_descriptions.h

    struct McpPromptDescription {
        mbase::string mName;
        mbase::string mDescription; // Optional
        mbase::vector<mbase::McpPromptArgument> mArguments; // Optional
    };

^^^^^^^^^^^^^^^
Writing Prompts
^^^^^^^^^^^^^^^

Writing example callbacks:

.. code-block:: cpp
    :caption: server.cpp

    mbase::vector<mbase::McpResponsePrompt> example_text_prompt(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        // Responding with text

        mbase::McpResponseTextPrompt textRespSystem;
        textRespSystem.mRole = "system";
        textRespSystem.mText = "Your name is " + std::get<mbase::string>(in_msg_map.at("name_arg"));

        mbase::McpResponseTextPrompt textRespUser;
        textRespUser.mRole = "user";
        textRespUser.mText = "How are you?";

        return {textRespSystem, textRespUser};
    }

    mbase::vector<mbase::McpResponsePrompt> example_image_prompt(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        // Responding with image

        mbase::McpResponseImagePrompt imageResp;
        imageResp.mRole = "user";
        imageResp.mMimeType = "image/...";
        imageResp.mData = "base64 encoded data";

        return {imageResp};
    }

    mbase::vector<mbase::McpResponsePrompt> example_audio_prompt(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        // Responding with audio

        mbase::McpResponseImagePrompt audioResp;
        audioResp.mRole = "user";
        audioResp.mMimeType = "audio/...";
        audioResp.mData = "base64 encoded data";

        return {audioResp};
    }

Create prompt arguments:

.. code-block:: cpp
    :caption: server.cpp

    // What is a completion string? See: https://modelcontextprotocol.io/specification/2025-03-26/server/utilities/completion
    mbase::McpPromptArgument promptArg;
    promptArg.mArgumentName = "name_arg";
    promptArg.mDescription = "A Name argument";
    promptArg.mCompletionStrings = {"comp1", "comp2", "compN"}; // Argument completion strings
    promptArg.mIsRequired = true;

Create description objects:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpPromptDescription promptDesc1;
    promptDesc1.mName = "example_text_prompt";
    promptDesc1.mArguments = {promptArg};
    promptDesc1.mDescription = "Example text prompt";

    mbase::McpPromptDescription promptDesc2;
    promptDesc2.mName = "example_image_prompt";
    promptDesc2.mDescription = "Example image prompt";

    mbase::McpPromptDescription promptDesc3;
    promptDesc3.mName = "example_audio_prompt";
    promptDesc3.mDescription = "Example audio prompt";

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Registering/Unregistering Prompts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Registration:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.register_prompt(promptDesc1, example_text_prompt);
    mcpServer.register_prompt(promptDesc2, example_image_prompt);
    mcpServer.register_prompt(promptDesc3, example_audio_prompt);

Unregistration:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.unregister_prompt("example_text_prompt");
    mcpServer.unregister_prompt("example_image_prompt");
    mcpServer.unregister_prompt("example_audio_prompt");


--------------------
Listing Client Roots
--------------------

Connected client object instance :code:`mbase::McpServerClient` has the following roots related methods:

.. code-block:: cpp
    :caption: mcp_server_client_state.h

    class MBASE_API McpServerClient : public mbase::logical_processor {
    public:
        ...
        // Returns true if the client supports roots
        bool has_roots() const noexcept; 
        ...
        GENERIC list_roots(mcp_st_list_roots_cb in_cb);
        ...

    protected:
        ...
    };

Where the :code:`mcp_st_list_roots_cb` callback signature defined as:

.. code-block:: cpp
    :caption: mcp_server_to_client_requests.h

    struct McpRootsResult {
        mbase::string mUri;
        mbase::string mName;
    };

    // error code, server instance, result object
    using mcp_st_list_roots_cb = std::function<GENERIC(const I32&, McpServerBase*, const mbase::vector<McpRootsResult>&)>;

For example, listing the client roots when the client is initialized:

.. code-block:: cpp

    class ExampleDerivation : public mbase::McpServerStdio {
    public:
        ...
        void on_client_init(mbase::McpServerClient* in_client) override
        {
            if(in_client->has_roots())
            {
                in_client->list_roots([&](const int& errCode, McpServerBase* self_instance, const mbase::vector<mbase::McpRootsResult>& listedRoots){
                    std::cout << "Client provides the following roots:" << std::endl;
                    for(const mbase::McpRootsResult& tmpRoot : listedRoots)
                    {
                        std::cout << "Name: " << tmpRoot.mName << std::endl;
                        std::cout << "Uri: " << tmpRoot.mUri << std::endl;
                    }
                });
            }
        }
        ...
    };

For more information: https://modelcontextprotocol.io/specification/2025-03-26/client/roots

-------------------------
Sending Sampling Requests
-------------------------

Connected client object instance :code:`mbase::McpServerClient` has the following sampling related methods:

.. code-block:: cpp
    :caption: mcp_server_client_state.h

    class MBASE_API McpServerClient : public mbase::logical_processor {
    public:
        ...
        // Returns true if the client supports sampling
        bool has_sampling() const noexcept;
        ...
        GENERIC request_sampling(const McpSamplingRequest& in_params, mcp_st_sampling_cb in_cb);
        ...

    protected:
        ...
    };

Where the :code:`McpSamplingRequest` object and :code:`mcp_st_sampling_cb` callback is defined as:

.. code-block:: cpp
    :caption: mcp_server_to_client_requests.h

    struct McpSamplingModelPreferences {
        mbase::vector<mbase::string> mModelHintsList;
        F64 mIntelligencePriority = 0.8;
        F64 mSpeedPriority = 0.5;
    };

    struct McpSamplingRequest {
        mbase::vector<mbase::McpSamplingMessage> mMessages;
        McpSamplingModelPreferences mModelPreferences;
        mbase::string mSystemPrompt;
        I32 mMaxTokens = 0;
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

    // error code, server instance, result object
    using mcp_st_sampling_cb = std::function<GENERIC(const I32&, McpServerBase*, const McpSamplingResult&)>;

For example, sending a sampling request when the client is initialized:

.. code-block:: cpp
    :caption: server.cpp

    class ExampleDerivation : public mbase::McpServerStdio {
    public:
        ...
        void on_client_init(mbase::McpServerClient* in_client) override
        {
            if(in_client->has_sampling())
            {
                mbase::McpSamplingMessage assistantMessage;
                assistantMessage.mRole = "assistant";
                assistantMessage.mContentType = mbase::mcp_sampling_content_type::TEXT;
                assistantMessage.mTextContent = "How can I help you?";

                mbase::McpSamplingMessage userMessage;
                userMessage.mRole = "user";
                userMessage.mContentType = mbase::mcp_sampling_content_type::TEXT;
                userMessage.mTextContent = "Is Gaben a wonderful person?";

                mbase::McpSamplingModelPreferences modelPreference;
                modelPreference.mIntelligencePriority = 1.0;
                modelPreference.mModelHintsList = {"model1", "model2", "modelN"};
                modelPreference.mSpeedPriority = 1.0;

                mbase::McpSamplingRequest samplingRequestDesc;
                samplingRequestDesc.mMaxTokens = 100;
                samplingRequestDesc.mMessages = {assistantMessage, userMessage};
                samplingRequestDesc.mModelPreferences = modelPreference;
                samplingRequestDesc.mSystemPrompt = "You are a helpful assistant with charming personality.";
                
                in_client->request_sampling(samplingRequestDesc, [&](const int& errCode, mbase::McpServerBase* self_instance, const mbase::McpSamplingResult& samplingResult) {
                    std::cout << "Model: " << samplingResult.mModel << " responded with a message: " << std::endl;
                    std::cout << "Role: " << samplingResult.mRole << std::endl;
                    std::cout << "- " << samplingResult.mTextContent;
                    std::cout << std::endl;
                    std::cout << "Stop reason: " << samplingResult.mStopReason << std::endl;
                });
            }
        }
        ...
    };

For more information: https://modelcontextprotocol.io/specification/2025-03-26/client/sampling