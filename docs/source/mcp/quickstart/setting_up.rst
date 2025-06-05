=========================
Creating an Empty Project
=========================

-----
About
-----

In this page, you will create an empty CMake project and link the MCP library into your project.

----------------
Setting up CMake 
----------------

In your working directory, create a directory named 'mcp_simple_project' and go into that directory:

.. code-block:: bash

    mkdir mcp_simple_project
    cd mcp_simple_project

In that directory, open your favorite IDE or using terminal, create three files given as:

* CMakeLists.txt
* client.cpp
* server.cpp

.. code-block:: bash
    
    touch CMakeLists.txt
    touch client.cpp
    touch server.cpp

Now, properly configure your CMakeLists.txt by setting up a project and minimum version etc.

.. code-block:: cmake
    :caption: CMakeLists.txt

    cmake_minimum_required(VERSION 3.15...3.31)
    project("mcp_simple_project" LANGUAGES CXX)

Then create two executables named as **mcp_client** and **mcp_server** with targets given as :code:`client.cpp` and :code:`server.cpp`.

.. code-block:: cmake
    :caption: CMakeLists.txt

    add_executable(mcp_client_sample client.cpp)
    add_executable(mcp_server_sample server.cpp)

Then, we will set the C++ version as 17 (as long as the minimum is 17, higher versions are fine) and then,
we will find the MBASE libraries and link the MCP library and specify the include path. Here is how you do it:

.. code-block:: cmake
    :caption: CMakeLists.txt

    find_package(mbase.libs REQUIRED COMPONENTS mcp)

    target_compile_features(mcp_client_sample PUBLIC cxx_std_17)
    target_link_libraries(mcp_client_sample PRIVATE mbase-mcp)
    target_include_directories(mcp_client_sample PUBLIC mbase-mcp)

    target_compile_features(mcp_server_sample PUBLIC cxx_std_17)
    target_link_libraries(mcp_server_sample PRIVATE mbase-mcp)
    target_include_directories(mcp_server_sample PUBLIC mbase-mcp)

After all those operations, your CMakeLists.txt should look like this:

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

-------------------
Setting up the code
-------------------

After we set the CMake configuration, we will print the self name and version of both MCP client and server.

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

.. code-block:: cpp
    :caption: server.cpp

    #include <mbase/mcp/mcp_server_base.h>
    #include <mbase/mcp/mcp_server_stdio.h>
    #include <iostream>

    int main()
    {
        mbase::McpServerStdio mcpServer(
            "MCP Sample Server",
            "1.0.0"
        );
        std::cout << mcpServer.get_server_name() << " " << mcpServer.get_server_version() << std::endl;
        return 0;
    }

After you run those programs and see the name and version, we are good to go!