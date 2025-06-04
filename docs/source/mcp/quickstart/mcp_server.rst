==========
MCP Server
==========

In this example, we will:

1. Initialize a STDIO server (if STDIO transport is preferred)
2. Initialize a HTTP server (if HTTP transport is preferred)
3. Implement tools such as:
    - **add_int64**: adds two 64 bit integers.
    - **add_float64**: adds two 64 bit floats.
    - **subtract_int64**: subtracts two 64 bit integers.
    - **subtract_float64**: subtracts two 64 bit floats.
    - **echo**: will echo back the received string argument to the client.
4. Expose resources such as:
    - **file://content.txt**: Contains random text.
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
        mbase::McpServerStdio stdioServer(
            "MCP Sample Server",
            "1.0.0"
        );
    }



--------------------------
HTTP Server Initialization
--------------------------

---------------------
Implementing Features
---------------------

^^^^
Tool
^^^^

^^^^^^^^
Resource
^^^^^^^^

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Prompt with Compilation Support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

--------------------
Registering Features
--------------------

-----------------------------
Implementing the Program Loop
-----------------------------

-----
Done!
-----