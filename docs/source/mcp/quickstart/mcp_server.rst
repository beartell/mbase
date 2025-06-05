==========
MCP Server
==========

In this chapter, we will:

1. Initialize a STDIO server (if STDIO transport is preferred)
2. Initialize a HTTP server (if HTTP transport is preferred)
3. Implement tools such as:
    - **add_int64**: adds two 64 bit integers.
    - **add_float64**: adds two 64 bit floats.
    - **echo**: will echo back the received string argument to the client.
4. Expose resources such as:
    - **file:///content.txt**: Contains random text.
5. Server prompts such as:
    - **greeting_prompt**: Will return a greetings prompt template with the argument substituted inside.
    - **mbase_sdk_inform** == Will return a prompt in the format: "Tell me about MBASE {argument}" in which the argument is provided by the client. We will implement a prompt compilation with this one.
6. Register those features
7. Implement the program loop.

---------------------------
STDIO Server Initialization
---------------------------

In order to initialize the STDIO server, include the following header:

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_stdio.h>

Then, you will instantiate the :code:`McpServerStdio` class as follows:

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_stdio.h>

    int main()
    {
        mbase::McpServerStdio mcpServer(
            "MCP Sample Server",
            "1.0.0"
        );
    }

After the instantiation is complete, we will start the stdio transport thread as follows:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.start_processor(); // doesn't block the main thread

Which will make the server read stdin in parallel thread waiting for parent process to send messages.

Since it runs on parallel, we will need to synchronize with the transport thread at some point in the program 
which will be mentioned in the :ref:`mcp-server-implement-program-loop`.

--------------------------
HTTP Server Initialization
--------------------------

In order to initialize the stateless HTTP server, include the following header:

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_http_streamable.h>

Then, you will instantiate the :code:`mbase::McpServerHttpStreamableStateless` class as follows:

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_http_streamable.h>

    int main()
    {
        mbase::McpServerHttpStreamableStateless mcpServer(
            "MCP Sample Server",
            "1.0.0",
            "127.0.0.1", // hostname
            8080 // port
        );
    }

After the instantiation is complete, we will start the HTTP transport thread as follows:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.start_processor(); // doesn't block the main thread

Which will make the server accept HTTP requests in parallel thread.

Since it runs on parallel, we will need to synchronize with the transport thread at some point in the program 
which will be mentioned in the :ref:`mcp-server-implement-program-loop`.

User can also disable the HTTP server by calling:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.stop_processor();

---------------------
Implementing Features
---------------------

Feature registration workflow is as follows:

1. Include the :code:`mcp_server_features.h` file.
2. Implement the feature callback and respond with a valid response object defined under the :code:`mcp_server_responses.h`.
3. Initialize argument description objects if the features requires arguments.
4. Initialize the feature description object.
5. Register the feature description object to the server.

Start by including the :code:`mcp_server_features.h` and :code:`mcp_server_responses.h` file.

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_stdio.h>
    #include <mbase/mcp/mcp_server_features.h> // hi!
    #include <mbase/mcp/mcp_server_responses.h> // hi!

    int main()
    {
        mbase::McpServerStdio mcpServer(
            "MCP Sample Server",
            "1.0.0"
        );
        mcpServer.start_processor(); // doesn't block the main thread
    }

^^^^
Tool
^^^^

Implement the feature callbacks and respond with a valid response object:

.. code-block:: cpp
    :caption: server.cpp

    // inputs are normally validated by the library but
    // in the JSON model, a "number" may be both 64-bit integer or float 
    // that is why we must do double check like this
    // this double check is not necessary if the arguments are strings as you may have seen in the "echo" tool

    mbase::McpResponseTool add_int64(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextTool toolResponse;
        if(std::holds_alternative<int64_t>(in_msg_map.at("num1")) && std::holds_alternative<int64_t>(in_msg_map.at("num2")))
        {
            int64_t number1 = std::get<int64_t>(in_msg_map.at("num1"));
            int64_t number2 = std::get<int64_t>(in_msg_map.at("num2"));
            toolResponse.mText = mbase::string::from_format("%llu + %llu = %llu", number1, number2, number1 + number2);
            return toolResponse;
        }
        toolResponse.mText = "Numbers must be 64 bit integers";
        return toolResponse;
    }

    mbase::McpResponseTool add_float64(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextTool toolResponse;
        if(std::holds_alternative<double>(in_msg_map.at("num1")) && std::holds_alternative<double>(in_msg_map.at("num2")))
        {
            double number1 = std::get<double>(in_msg_map.at("num1"));
            double number2 = std::get<double>(in_msg_map.at("num2"));
            toolResponse.mText = mbase::string::from_format("%f + %f = %f", number1, number2, number1 + number2);
            return toolResponse;
        }
        toolResponse.mText = "Numbers must be 64 floats";
        return toolResponse;
    }

    mbase::McpResponseTool echo(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextTool toolResponse;
        toolResponse.mText = std::get<mbase::string>(in_msg_map.at("user_message"));
        return toolResponse;
    }

Initialize the argument description objects if the tool requires arguments:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpToolArgument toolArg1;
    toolArg1.mArgType = mbase::McpValueType::NUMBER;
    toolArg1.mArgumentName = "num1";
    toolArg1.mDescription = "First number of the add_int64/float64 tool";
    toolArg1.mIsRequired = true;

    mbase::McpToolArgument toolArg2;
    toolArg2.mArgType = mbase::McpValueType::NUMBER;
    toolArg2.mArgumentName = "num2";
    toolArg2.mDescription = "Second number of the add_int64/float64 tool";
    toolArg2.mIsRequired = true;

    mbase::McpToolArgument echoToolArg;
    echoToolArg.mArgType = mbase::McpValueType::STRING;
    echoToolArg.mArgumentName = "user_message";
    echoToolArg.mDescription = "Message to echo";
    echoToolArg.mIsRequired = true;

Initialize the feature description objects:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpToolDescription addToolint64Description;
    addToolint64Description.mDescription = "This tool adds two 64-bit integers and return the result";
    addToolint64Description.mName = "add_int64";
    addToolint64Description.mArguments = {toolArg1, toolArg2};

    mbase::McpToolDescription addToolfloat64Description;
    addToolfloat64Description.mDescription = "This tool adds two 64-bit floating point numbers and return the result";
    addToolfloat64Description.mName = "add_float64";
    addToolfloat64Description.mArguments = {toolArg1, toolArg2};

    mbase::McpToolDescription echoToolDescription;
    echoToolDescription.mDescription = "This tool echoes the 'message' argument back to the user";
    echoToolDescription.mName = "echo";
    echoToolDescription.mArguments = {echoToolArg};

Register the feature descriptions to the server:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.register_tool(addToolint64Description, add_int64);
    mcpServer.register_tool(addToolfloat64Description, add_float64);
    mcpServer.register_tool(echoToolDescription, echo);

Next, we will implement a single resource feature with logging and progress tracking.

^^^^^^^^
Resource
^^^^^^^^

Implement the feature callback and respond with a valid response object:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResponseResource content_file_uri(mbase::McpServerClient* in_client_instance, const mbase::Json& in_progress_token)
    {
        mbase::McpNotificationLogMessage logMsg;
        logMsg.mLogger = "resource logger";
        logMsg.mError = "Reading content.txt ...";
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

Initialize the resource description object:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpResourceDescription contentResourceDescription;
    contentResourceDescription.mName = "content.txt";
    contentResourceDescription.mUri = "file:///content.txt";

Register the feature description to the server:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.register_resource(contentResourceDescription, content_file_uri);

Next, we will implement a prompt with compilation support.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Prompt with Compilation Support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Implement the feature callbacks and respond with a valid response object:

.. code-block:: cpp
    :caption: server.cpp

    mbase::vector<mbase::McpResponsePrompt> greeting_prompt_cb(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextPrompt textPromptResp;
        textPromptResp.mRole = "assistant";
        textPromptResp.mText = "You must greet the user with the following message: " + std::get<mbase::string>(in_msg_map.at("greet_text"));
        return {textPromptResp};
    }

    mbase::vector<mbase::McpResponsePrompt> mbase_sdk_inform_cb(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextPrompt textPromptResp;
        textPromptResp.mRole = "user";
        textPromptResp.mText = "Tell me about MBASE " + std::get<mbase::string>(in_msg_map.at("mbase_arg"));
        return {textPromptResp};
    }

Initialize the argument description objects if the prompt requires arguments: 

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpPromptArgument greetArgument;
    greetArgument.mArgumentName = "greet_text";
    greetArgument.mDescription = "A greetings text";
    greetArgument.mIsRequired = true;

    mbase::McpPromptArgument mbaseArgument;
    mbaseArgument.mArgumentName = "mbase_arg";
    mbaseArgument.mCompletionStrings = {"mcp-sdk", "mcp-sdk-examples", "mcp-sdk-usage", "mcp-server-about", "mcp-server-usage"};
    mbaseArgument.mDescription = "What to describe in MBASE MCP SDK";
    mbaseArgument.mIsRequired = true;

Initialize the prompt description objects:

.. code-block:: cpp
    :caption: server.cpp

    mbase::McpPromptDescription greetPromptDescription;
    greetPromptDescription.mName = "greeting_prompt";
    greetPromptDescription.mDescription = "Will return a greetings prompt template with the argument substituted inside.";
    greetPromptDescription.mArguments = {greetArgument};

    mbase::McpPromptDescription mbasePromptDescription;
    mbasePromptDescription.mName = "mbase_sdk_inform";
    mbasePromptDescription.mDescription = "Will return a prompt in the format: 'Tell me about MBASE {argument}'";
    mbasePromptDescription.mArguments = {mbaseArgument};

Register the feature descriptions to the server:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.register_prompt(greetPromptDescription, greeting_prompt_cb);
    mcpServer.register_prompt(mbasePromptDescription, mbase_sdk_inform_cb);

.. _mcp-server-implement-program-loop:
-----------------------------
Implementing the Program Loop
-----------------------------

The server will listen for messages from the client in a parallel transport thread after the call:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.start_processor(); // doesn't block the main thread

The server will queue the valid MCP messages in its state or discard the received message if the message
is invalid.

Queued messages will be dispatched and all the corresponding callbacks and :code:`on_*` events 
will be called by the time the :code:`update` method of the server is called:

.. code-block:: cpp
    :caption: server.cpp

    mcpServer.update();

The frequency of this call will determine the dispatch frequency.

For that reason, we need to write a program loop and continuosly call the update method of the server:

.. code-block:: cpp
    :caption: server.cpp

    while(mcpServer.is_processor_running())
    {
        mcpServer.update();
        mbase::sleep(5); // in order to prevent resource exhaustion
    }

The method :code:`is_processor_running()` will return true if the server is listening for messages in the transport thread.

-----
Done!
-----

Congratulations! You have implemented your first MCP server in C++ with all fundamental features. 
In order to work with the advanced features of the MCP SDK, refer to the information reference section.

We will now call the features we wrote in this chapter from the :doc:`MCP C++ client <mcp_client>` we write in the next chapter.

--------------------
Complete Source Code
--------------------

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_stdio.h>
    #include <mbase/mcp/mcp_server_features.h>
    #include <mbase/mcp/mcp_server_responses.h>

    // inputs are normally validated by the library but
    // the JSON model dictates a "number" which may be both 64-bit integer or float
    // that is why we must do double check like this
    // this double check is not necessary if the arguments are strings as you may have seen in the "echo" tool

    mbase::McpResponseTool add_int64(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextTool toolResponse;
        if(std::holds_alternative<int64_t>(in_msg_map.at("num1")) && std::holds_alternative<int64_t>(in_msg_map.at("num2")))
        {
            int64_t number1 = std::get<int64_t>(in_msg_map.at("num1"));
            int64_t number2 = std::get<int64_t>(in_msg_map.at("num2"));
            toolResponse.mText = mbase::string::from_format("%llu + %llu = %llu", number1, number2, number1 + number2);
            return toolResponse;
        }
        toolResponse.mText = "Numbers must be 64 bit integers";
        return toolResponse;
    }

    mbase::McpResponseTool add_float64(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextTool toolResponse;
        if(std::holds_alternative<double>(in_msg_map.at("num1")) && std::holds_alternative<double>(in_msg_map.at("num2")))
        {
            double number1 = std::get<double>(in_msg_map.at("num1"));
            double number2 = std::get<double>(in_msg_map.at("num2"));
            toolResponse.mText = mbase::string::from_format("%f + %f = %f", number1, number2, number1 + number2);
            return toolResponse;
        }
        toolResponse.mText = "Numbers must be 64 floats";
        return toolResponse;
    }

    mbase::McpResponseTool echo(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextTool toolResponse;
        toolResponse.mText = std::get<mbase::string>(in_msg_map.at("user_message"));
        return toolResponse;
    }

    mbase::McpResponseResource content_file_uri(mbase::McpServerClient* in_client_instance, const mbase::Json& in_progress_token)
    {
        mbase::McpNotificationLogMessage logMsg;
        logMsg.mLogger = "resource logger";
        logMsg.mError = "Reading content.txt ...";
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

    mbase::vector<mbase::McpResponsePrompt> greeting_prompt_cb(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextPrompt textPromptResp;
        textPromptResp.mRole = "assistant";
        textPromptResp.mText = "You must greet the user with the following message: " + std::get<mbase::string>(in_msg_map.at("greet_text"));
        return {textPromptResp};
    }

    mbase::vector<mbase::McpResponsePrompt> mbase_sdk_inform_cb(mbase::McpServerClient* in_client_instance, const mbase::McpMessageMap& in_msg_map, const mbase::Json& in_progress_token)
    {
        mbase::McpResponseTextPrompt textPromptResp;
        textPromptResp.mRole = "user";
        textPromptResp.mText = "Tell me about MBASE " + std::get<mbase::string>(in_msg_map.at("mbase_arg"));
        return {textPromptResp};
    }

    int main()
    {
        mbase::McpServerStdio mcpServer(
            "MCP Sample Server",
            "1.0.0"
        );

        mcpServer.start_processor();

        mbase::McpToolArgument toolArg1;
        toolArg1.mArgType = mbase::McpValueType::NUMBER;
        toolArg1.mArgumentName = "num1";
        toolArg1.mDescription = "First number of the add_int64/float64 tool";
        toolArg1.mIsRequired = true;

        mbase::McpToolArgument toolArg2;
        toolArg2.mArgType = mbase::McpValueType::NUMBER;
        toolArg2.mArgumentName = "num2";
        toolArg2.mDescription = "Second number of the add_int64/float64 tool";
        toolArg2.mIsRequired = true;

        mbase::McpToolArgument echoToolArg;
        echoToolArg.mArgType = mbase::McpValueType::STRING;
        echoToolArg.mArgumentName = "user_message";
        echoToolArg.mDescription = "Message to echo";
        echoToolArg.mIsRequired = true;

        mbase::McpPromptArgument greetArgument;
        greetArgument.mArgumentName = "greet_text";
        greetArgument.mDescription = "A greetings text";
        greetArgument.mIsRequired = true;

        mbase::McpPromptArgument mbaseArgument;
        mbaseArgument.mArgumentName = "mbase_arg";
        mbaseArgument.mCompletionStrings = {"mcp-sdk", "mcp-sdk-examples", "mcp-sdk-usage", "mcp-server-about", "mcp-server-usage"};
        mbaseArgument.mDescription = "What to describe in MBASE MCP SDK";
        mbaseArgument.mIsRequired = true;

        mbase::McpToolDescription addToolint64Description;
        addToolint64Description.mDescription = "This tool adds two 64-bit integers and return the result";
        addToolint64Description.mName = "add_int64";
        addToolint64Description.mArguments = {toolArg1, toolArg2};

        mbase::McpToolDescription addToolfloat64Description;
        addToolfloat64Description.mDescription = "This tool adds two 64-bit floating point numbers and return the result";
        addToolfloat64Description.mName = "add_float64";
        addToolfloat64Description.mArguments = {toolArg1, toolArg2};

        mbase::McpToolDescription echoToolDescription;
        echoToolDescription.mDescription = "This tool echoes the 'message' argument back to the user";
        echoToolDescription.mName = "echo";
        echoToolDescription.mArguments = {echoToolArg};

        mbase::McpResourceDescription contentResourceDescription;
        contentResourceDescription.mName = "content_file_uri";
        contentResourceDescription.mUri = "file:///content.txt";

        mbase::McpPromptDescription greetPromptDescription;
        greetPromptDescription.mName = "greeting_prompt";
        greetPromptDescription.mDescription = "Will return a greetings prompt template with the argument substituted inside.";
        greetPromptDescription.mArguments = {greetArgument};

        mbase::McpPromptDescription mbasePromptDescription;
        mbasePromptDescription.mName = "mbase_sdk_inform";
        mbasePromptDescription.mDescription = "Will return a prompt in the format: 'Tell me about MBASE {argument}'";
        mbasePromptDescription.mArguments = {mbaseArgument};

        mcpServer.register_tool(addToolint64Description, add_int64);
        mcpServer.register_tool(addToolfloat64Description, add_float64);
        mcpServer.register_tool(echoToolDescription, echo);
        mcpServer.register_resource(contentResourceDescription, content_file_uri);
        mcpServer.register_prompt(greetPromptDescription, greeting_prompt_cb);
        mcpServer.register_prompt(mbasePromptDescription, mbase_sdk_inform_cb);

        while(mcpServer.is_processor_running())
        {
            mcpServer.update();
            mbase::sleep(5);
        }

        return 0;
    }