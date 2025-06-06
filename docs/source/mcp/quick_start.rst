==========
Quickstart
==========

.. tip::
    It is highly recommended that you read the sections in order, as they are not arranged randomly in the document.

In this chapter, we will cover everything to setup, run and develop using MBASE MCP library.
Even if the setting up section covers everything to setup the MCP library,
the concepts that are implemented in the examples section are just fundamental MCP features which
are mostly sufficient for every-day usage of MCP.

In order to use and understand the MCP library in detail, make sure you refer to the :doc:`information_reference`.

---------
Structure
---------

This chapter is composed of six sections which cover topics as follows:

* :doc:`Setting-up<quickstart/setting_up>`: Creating a CMake project and linking the MCP library.

* :doc:`Complete Project Setup<quickstart/project_setup>`: Properly configured :code:`CMakeLists.txt`, :code:`client.cpp` and :code:`server.cpp` for reference.

* :doc:`MCP STDIO/HTTP Server Usage<quickstart/mcp_server>`: MCP STDIO/HTTP server implementation example. Codes under this section will be written under the :code:`server.cpp`.

* :doc:`MCP STDIO/HTTP Client Usage<quickstart/mcp_client>`: MCP STDIO/HTTP client implementation example. Codes under this section will be written under the :code:`client.cpp`.

* :doc:`Complete Source Code<quickstart/complete_source>`: A Complete source code of both MCP client and server examples.

* :doc:`What Now?<quickstart/what_now>`: Small-talk about MCP, where to find resources and advanced MCP SDK Usage.

.. toctree::
    :hidden:

    Setting-up <quickstart/setting_up>
    Complete Project Setup <quickstart/project_setup>
    MCP STDIO/HTTP Server Usage <quickstart/mcp_server>
    MCP STDIO/HTTP Client Usage <quickstart/mcp_client>
    Complete Source Code <quickstart/complete_source>
    What Now? <quickstart/what_now>