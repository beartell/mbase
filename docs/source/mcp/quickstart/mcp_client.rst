==========
MCP Client
==========

In this chapter, we will:

1. Initialize the STDIO client (if STDIO transport is preferred)
2. Initialize the HTTP client (if HTTP transport is preferred)
3. Register the MCP server and initiate the connection.
4. List the tools/resources/prompts of the :doc:`mcp_server` we wrote in the previous chapter.
5. Call the tools/resources/prompts of the :doc:`mcp_server`.
6. Implement the program loop.
7. Compile and run the program.

------------
STDIO Client
------------

In order to initialize the STDIO client connection, include the following header:

.. code-block:: cpp
    :caption: client.cpp

    #include <mbase/mcp/mcp_client_server_stdio.h>    

Then, you will create the :code:`mbase::McpServerStdioInit` object as follows:

.. code-block:: cpp
    :caption: client.cpp

    #include <mbase/mcp/mcp_client_base.h>
    #include <mbase/mcp/mcp_client_server_stdio.h>
    #include <iostream>

    int main()
    {
        mbase::McpClientBase myMcpClient(
            "MCP Sample Client",
            "1.0.0"
        );
        mbase::McpServerStdioInit initDesc;
        initDesc.mServerName = "MCP Sample Server";
        initDesc.mCommand = "./mcp_server_sample"; // path to the executable
        // initDesc.mArguments = {arg1, arg2 ...} /* if you want to start the subprocess with arguments */
        // initDesc.mEnvironmentVariables = mbase::unordered_map<mbase::string, mbase::string>  /* if you want to pass environment variables */

        return 0;
    }

Since our :doc:`server.cpp <mcp_server>` doesn't require arguments or environment variables, 
we are not passing them to program.

Now, we will initialize the :code:`mbase::McpClientServerStdio` object and give
our :code:`mbase::McpServerStdioInit` to its constructor:

.. code-block:: cpp
    :caption: client.cpp

    #include <mbase/mcp/mcp_client_base.h>
    #include <mbase/mcp/mcp_client_server_stdio.h>
    #include <iostream>

    int main()
    {
        mbase::McpClientBase myMcpClient(
            "MCP Sample Client",
            "1.0.0"
        );
        mbase::McpServerStdioInit initDesc;
        initDesc.mServerName = "MCP Sample Server";
        initDesc.mCommand = "./mcp_server_sample";

        mbase::McpClientServerStdio mcpServerState(initDesc);

        return 0;
    }

-----------
HTTP Client
-----------

--------------------------
Registering the MCP Server
--------------------------

To register the MCP server, we need to call the :code:`register_mcp_server` method of the client
and start the server's transport thread:

.. code-block:: cpp
    :caption: client.cpp

    myMcpClient.register_mcp_server(&mcpServerState);
    mcpServerState.start_processor();

After the registration, we need to initiate an MCP connection with the server:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.initialize(&myMcpClient, [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::McpServerStateBase* server_instance){
        if(errCode == MBASE_MCP_SUCCESS)
        {
            std::cout << "Connection successful" << std::endl;
        }
        else
        {
            std::cout << "Connection failed" << std::endl;
        }
    });

.. important::

    The term "Connection successful" may mean two different things depending on your transport method:

    STDIO: If the transport method is STDIO, it means that the subprocess is successfully created and the MCP capability negotiation
    between the subprocess and the parent process (MCP Client) in our case is established successfully.

    HTTP: If the transport method is HTTP, it means that the remote connection with the MCP server is established and the MCP capability negotiation
    between the client and server is established successfully.

    See: `MCP Transport <https://modelcontextprotocol.io/specification/2025-03-26/basic/transports>`_

-------------------------------
Listing Tools/Resources/Prompts
-------------------------------

After the initialization is complete, we can call the methods of the MCP server inside the initialize callback

In order to list the features of an MCP server, we need make the following call:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.list_tools([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpToolDescription>&& tools_list, const mbase::string& pagination_token){
        // do stuff
    });

    mcpServerState.list_prompts([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpPromptDescription>&& prompts_list, const mbase::string& pagination_token){
        // do stuff
    });

    mcpServerState.list_resources([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResourceDescription>&& resources_list, const mbase::string& pagination_token){
        // do stuff
    });

The description vector will contain all the information about each feature the server provides. If you have noticed
that it is the same object we used to register features in the :doc:`mcp_server` chapter.

.. important::

    Pagination is also supported for list operations.

    See: TODO, put pagination reference link

-------------
Calling Tools
-------------

Now, we will call the :code:`add_int64`, :code:`add_float64` and :code:`echo` tools in which we have written
in the previous :doc:`mcp_server` chapter.

For the sake of clarity, the call will look like this in its most simple form:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.tool_call("add_int64", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){

    });

    mcpServerState.tool_call("add_float64", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){

    });

    mcpServerState.tool_call("echo", [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){

    });

Now, we will pass the arguments and display the call result:

.. code-block:: cpp
    :caption: client.cpp

    // arbitrary numbers
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

-----------------
Reading Resources
-----------------

We will read the :code:`file:///content.txt` uri as follows:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.read_resource("file:///content.txt", [](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseResource>&& resourceResponse){
        mbase::McpResponseTextResource textResponse = std::get<mbase::McpResponseTextResource>(resourceResponse[0]);
        std::cout << "Content: " << textResponse.mText << std::endl;
    });

---------------
Getting Prompts
---------------

We will get the :code:`greeting_prompt` and :code:`mbase_sdk_inform` prompts. 
For the sake of clarity, the call will look like this in its simplest form:

.. code-block:: cpp
    :caption: client.cpp

    mcpServerState.get_prompt("greeting_prompt", [](const int& errCode, mbase::McpClientBase* self_client_instance, const mbase::string& prompt_description, mbase::vector<mbase::McpResponsePrompt>&& promptResponse) {

    });

    mcpServerState.get_prompt("mbase_sdk_inform", [](const int& errCode, mbase::McpClientBase* self_client_instance, const mbase::string& prompt_description, mbase::vector<mbase::McpResponsePrompt>&& promptResponse) {

    });

Now, we will pass the arguments and display the call result:

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

-----------------------------
Implementing the Program Loop
-----------------------------

-----
Done!
-----