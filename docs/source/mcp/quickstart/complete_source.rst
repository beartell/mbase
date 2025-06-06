===============
Complete Source
===============

--------------
CMakeLists.txt
--------------

.. code-block:: cmake
    :caption: CMakeLists.txt

    cmake_minimum_required(VERSION 3.15...3.31)
    project("mcp_simple_project" LANGUAGES CXX)

    add_executable(mcp_client_sample client.cpp)
    add_executable(mcp_server_sample server.cpp)

    find_package(mbase.libs REQUIRED COMPONENTS mcp)

    target_compile_features(mcp_client_sample PUBLIC cxx_std_17)
    target_link_libraries(mcp_client_sample PRIVATE mbase-mcp)
    target_include_directories(mcp_client_sample PUBLIC mbase-mcp)

    target_compile_features(mcp_server_sample PUBLIC cxx_std_17)
    target_link_libraries(mcp_server_sample PRIVATE mbase-mcp)
    target_include_directories(mcp_server_sample PUBLIC mbase-mcp)

----------
client.cpp
----------

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
        myMcpClient.register_mcp_server(&mcpServerState);
        mcpServerState.start_processor();

        mcpServerState.initialize(&myMcpClient, [&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::McpServerStateBase* server_instance){
            if(errCode == MBASE_MCP_SUCCESS)
            {
                std::cout << "Connection successful" << std::endl;
            }
            else
            {
                std::cout << "Connection failed" << std::endl;
            }

            mcpServerState.list_tools([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpToolDescription>&& tools_list, const mbase::string& pagination_token){
                // do stuff
            });

            mcpServerState.list_prompts([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpPromptDescription>&& prompts_list, const mbase::string& pagination_token){
                // do stuff
            });

            mcpServerState.list_resources([&](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResourceDescription>&& resources_list, const mbase::string& pagination_token){
                // do stuff
            });

            // arbitrary numbers
            mbase::McpToolMessageArgument intNum1 = 10;
            mbase::McpToolMessageArgument intNum2 = 20;
            mbase::McpToolMessageArgument floatNum1 = 10.5f;
            mbase::McpToolMessageArgument floatNum2 = 20.5f;
            mbase::McpToolMessageArgument echoMessage = "Hello world!";

            mbase::McpToolMessageMap argMap;
            argMap["num1"] = intNum1;
            argMap["num2"] = intNum2;

            mcpServerState.tool_call("add_int64", [](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){
                mbase::McpResponseTextTool textResponse = std::get<mbase::McpResponseTextTool>(toolResponse[0]);
                std::cout << textResponse.mText << std::endl;
            }, MBASE_MCP_TIMEOUT_DEFAULT, argMap);

            argMap.clear();
            argMap["num1"] = floatNum1;
            argMap["num2"] = floatNum2;

            mcpServerState.tool_call("add_float64", [](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){
                mbase::McpResponseTextTool textResponse = std::get<mbase::McpResponseTextTool>(toolResponse[0]);
                std::cout << textResponse.mText << std::endl;
            }, MBASE_MCP_TIMEOUT_DEFAULT, argMap);

            argMap.clear();
            argMap["user_message"] = echoMessage;

            mcpServerState.tool_call("echo", [](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseTool>&& toolResponse, bool is_error){
                mbase::McpResponseTextTool textResponse = std::get<mbase::McpResponseTextTool>(toolResponse[0]);
                std::cout << textResponse.mText << std::endl;            
            }, MBASE_MCP_TIMEOUT_DEFAULT, argMap);

            mcpServerState.read_resource("file:///content.txt", [](const int& errCode, mbase::McpClientBase* self_client_instance, mbase::vector<mbase::McpResponseResource>&& resourceResponse){
                mbase::McpResponseTextResource textResponse = std::get<mbase::McpResponseTextResource>(resourceResponse[0]);
                std::cout << "Content: " << textResponse.mText << std::endl;
            });

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
        });
        

        while(1)
        {
            myMcpClient.update();
            mbase::sleep(5);
        }

        return 0;
    }

----------
server.cpp
----------

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_stdio.h>
    #include <mbase/mcp/mcp_server_http_streamable.h>
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
        toolResponse.mText = "Numbers must be 64 bit floats";
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