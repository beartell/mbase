======================
Complete Project Setup
======================

From now on, we will use the **CMakeLists.txt**, **client.cpp** and **server.cpp** files as a base
for almost every demonstration example we build together.

The examples are:

* :doc:`MCP STDIO/HTTP Server Usage<mcp_server>`: MCP STDIO/HTTP server implementation example. Codes under this section will be written under the :code:`server.cpp`.

* :doc:`MCP STDIO/HTTP Client Usage<mcp_client>`: MCP STDIO/HTTP client implementation example. Codes under this section will be written under the :code:`client.cpp`.

--------------------
Total CMakeLists.txt
--------------------

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

----------------
Total client.cpp
----------------

.. code-block:: cpp
    :caption: client.cpp

    #include <mbase/mcp/mcp_client_base.h>
    #include <iostream>

    int main()
    {
        mbase::McpClientBase myMcpClient(
            "MCP Sample Client",
            "1.0.0"
        );
        std::cout << myMcpClient.get_client_name() << " " << myMcpClient.get_client_version() << std::endl;
        return 0;
    }

----------------
Total server.cpp
----------------

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_base.h>
    #include <mbase/mcp/mcp_server_stdio.h>
    #include <iostream>

    int main()
    {
        mbase::McpServerStdio stdioServer(
            "MCP Sample Server",
            "1.0.0"
        );
        std::cout << stdioServer.get_server_name() << " " << stdioServer.get_server_version() << std::endl;
        return 0;
    }