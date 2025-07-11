==============
MCP SDK Design
==============

---------------
Finding the SDK
---------------

If you have installed the MBASE SDK, you can find the library
using CMake :code:`find_package` function with components specification.

In order to find the library using cmake, write the following:

.. code-block:: cmake
    
    find_package(mbase.libs REQUIRED COMPONENTS mcp)

Specify the include path and link the libraries:

.. code-block:: cmake

    target_include_directories(<your_target> PUBLIC mbase-mcp)
    target_link_libraries(<your_target> PRIVATE mbase-mcp)

--------------
File Hierarchy
--------------

All header files are prefixed with :code:`mbase/mcp` in the MCP SDK. Below are the MCP SDK headers:

.. note::

    All headers include :code:`mcp_common.h` by default.

+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| File                                | Remarks                                                                                                                                                                                                                                                                       | Used By       | Includes                                                               |
+=====================================+===============================================================================================================================================================================================================================================================================+===============+========================================================================+
| mcp_client_arguments.h              | Contains the STDIO and HTTP init object which are used to initialize the respective clients.It also contains the feature argument typedefs for calling features with arguments.                                                                                               | Client        |                                                                        |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_client_base.h                   | Contains the fundamental :code:`McpClientBase` class. Client can extend the MCP client functionality by inheriting from this class. See: :doc:`mcp_client_detail`.                                                                                                            | Client        | :code:`mcp_notifications.h`, :code:`mcp_server_to_client_requests.h`   |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_client_callbacks.h              | Client feature callback signatures are defined in this file.                                                                                                                                                                                                                  | Client        | :code:`mcp_server_responses.h`, :code:`mcp_server_descriptions.h`      |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_client_server_http.h            | Contains the :code:`McpClientServerHttp` class which is used to establish HTTP connection with the MCP server.                                                                                                                                                                | Client        | :code:`mcp_client_server_state.h`                                      |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_client_server_stdio.h           | Contains the :code:`McpClientServerStdio` class which is used to create a MCP server subprocess and communicate it through STDIO.                                                                                                                                             | Client        | :code:`mcp_client_server_state.h`                                      |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_common.h                        | Contains error code macros, includes frequently used data structures such as unordered_map, vector etc.                                                                                                                                                                       | Client/Server |                                                                        |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_notifications.h                 | Defines notification objects                                                                                                                                                                                                                                                  | Client/Server |                                                                        |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_packet_parsing.h                | MCP parsing procedures. They can be used to build and parse an MCP packet.                                                                                                                                                                                                    | Client/Server |                                                                        |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_arguments.h              | Defines :code:`McpPromptArgument`, :code:`McpToolArgument` objects which are used to describe a feature containing arguments.                                                                                                                                                 | Server        |                                                                        |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_base.h                   | Contains the fundamental :code:`McpServerBase` class. Use shouldn't extend directly from the base but should instead either from :code:`McpServerStdio`, :code:`McpServerHttpStreamableStateful` or :code:`McpServerHttpStreamableStateless`. See: :doc:`mcp_server_detail`   | Server        | :code:`mcp_server_features.h`, :code:`mcp_server_client_state.h`       |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_client_http_streamable.h | Defines the :code:`McpServerClientHttpStreamable` which is created for each HTTP client connection to the server. The creation and management of this class is being handled by either the :code:`McpServerHttpStreamableStateful` or :code:`McpServerHttpStreamableStateles` | Server        | :code:`mcp_server_client_state.h`                                      |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_client_state.h           | Defines the :code:`McpServerClient` object which holds the state information about the current client connection. Since, this is managed by the server, user don't have anything to do with this class.                                                                       | Server        | :code:`mcp_notifications.h`, :code:`mcp_server_to_client_requests.h`   |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_client_stdio.h           | Defines the :code:`McpServerStdioClient` object which extends the :code:`McpServerClient` objects. This is used to manage the STDIO transport layer with the client. User don't have anything to do with this class.                                                          | Server        | :code:`mcp_server_client_state.h`                                      |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_descriptions.h           | Contains the description objects such as :code:`McpResourceDescription`, :code:`McpPromptDescription` and :code:`McpToolDescription` which are used the describe and register a feature on the server.                                                                        | Client/Server | :code:`mcp_server_arguments.h`                                         |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_features.h               | Contains the feature objects and feature callback signatures.                                                                                                                                                                                                                 | Server        | :code:`mcp_server_descriptions.h`, :code:`mcp_server_responses.h`      |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_http_streamable.h        | Contains both the :code:`McpServerHttpStreamableStateful` and :code:`McpServerHttpStreamableStateless` objects which are used to initialize an MCP HTTP server.                                                                                                               | Server        | :code:`mcp_server_base.h`, :code:`mcp_server_client_http_streamable.h` |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_responses.h              | Defines the feature response objects and expose them as variant object  such as :code:`McpResponseResource`, :code:`McpResponsePrompt` and :code:`McpResponseTool`. This header is used to send and read responses.                                                           | Client/Server |                                                                        |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_stdio.h                  | Contains the :code:`McpServerStdio` object which is used to initialize an MCP STDIO server.                                                                                                                                                                                   | Server        | :code:`mcp_server_base.h`, :code:`mcp_server_client_stdio.h`           |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+
| mcp_server_to_client_requests.h     | Defines callbacks signatures and request objects for server to client roots/sampling requests. The objects defined under this file is used by both client and the server.                                                                                                     | Client/Server |                                                                        |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+---------------+------------------------------------------------------------------------+


----------------
Object Hierarchy
----------------

.. image:: mcp_object_diagram.png
  :width: 1920
  :alt: Alternative text

-----------
Error Codes
-----------

Given error codes are declared under the :code:`mcp_common.h` file:

+------------------------------+--------+
| Macro                        | Code   |
+==============================+========+
| MBASE_MCP_SUCCESS            | 0      |
+------------------------------+--------+
| MBASE_MCP_PARSE_ERROR        | -32700 |
+------------------------------+--------+
| MBASE_MCP_INVALID_REQUEST    | -32600 |
+------------------------------+--------+
| MBASE_MCP_METHOD_NOT_FOUND   | -32601 |
+------------------------------+--------+
| MBASE_MCP_INVALID_PARAMS     | -32602 |
+------------------------------+--------+
| MBASE_MCP_INTERNAL_ERROR     | -32603 |
+------------------------------+--------+
| MBASE_MCP_TIMEOUT            | -32001 |
+------------------------------+--------+
| MBASE_MCP_RESOURCE_NOT_FOUND | -32002 |
+------------------------------+--------+

------
Macros
------

Given macros are declared under the :code:`mcp_common.h` file:

+-------------------------------+-------+----------------------------------------------------------------------------+
| Macro                         | Value | Description                                                                |
+===============================+=======+============================================================================+
| MBASE_MCP_TIMEOUT_DEFAULT     | 10    | Default request timeout in seconds. Applies to both client and server.     |
+-------------------------------+-------+----------------------------------------------------------------------------+
| MBASE_MCP_STDIO_BUFFER_LENGTH | 65536 | Default STDIO read/write buffer length. Applies to both client and server. |
+-------------------------------+-------+----------------------------------------------------------------------------+