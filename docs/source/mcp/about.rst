.. include:: ../inference/links.rst

==============================
Model Context Protocol Library
==============================

Welcome to the MBASE MCP library documentation!

-------------------
Document Navigation
-------------------

The document is structured into two chapters as follows:

* :doc:`Quickstart <quick_start>`: In this chapter, we start by creating a CMake project and linking the MCP SDK to it. Afterwards, we implement a simple MCP server with tool/resource/prompt features and communicate with that server through the MCP client which is also implemented using the SDK.

* :doc:`Information Reference <information_reference>`: This chapter contains useful reference information for an advanced, full-featured MCP SDK usage.

.. toctree::
    :hidden:

    Quickstart <quick_start>
    Information Reference <information_reference>

-----
About
-----

MBASE MCP Library is an MCP client/server SDK written in C++ with all fundamental features implemented.

A full MCP documentation can be found in their official documentation website: https://modelcontextprotocol.io/introduction

-----------------
Transport Methods 
-----------------

+-----------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------+
| Method                      | Support                                          | Docs                                                                                   |
+=============================+==================================================+========================================================================================+
| stdio                       | :octicon:`check-circle-fill;1em;sd-text-success` | STDIO Init :ref:`Client <mcp-client-stdio-init>`/:ref:`Server <mcp-server-stdio-init>` |
+-----------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------+
| Streamable HTTP Stateless   | :octicon:`check-circle-fill;1em;sd-text-success` | HTTP Init :ref:`Client <mcp-client-http-init>`/:ref:`Server <mcp-server-http-init>`    |
+-----------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------+
| Streamable HTTP Stateful    | :octicon:`gear;1em`                              |                                                                                        |
+-----------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------+
| SSE                         | :octicon:`x-circle-fill;1em;sd-text-danger`      |                                                                                        |
+-----------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------+

----------------------
Feature Support Matrix 
----------------------

+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Feature            | Client                                            | Server                                           | Docs                                                                                                                                    |
+====================+===================================================+==================================================+=========================================================================================================================================+
| Prompts            |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Working with Prompts :ref:`Client <mcp-client-working-with-prompts>`/:ref:`Server <mcp-server-working-with-prompts>`                    |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Resources          |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Working with Resources :ref:`Client <mcp-client-working-with-resources>`/:ref:`Server <mcp-server-working-with-resources>`              |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Tools              |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Working with Tools :ref:`Client <mcp-client-working-with-tools>`/:ref:`Server <mcp-server-working-with-tools>`                          |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Prompt Compilation |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Sending/Providing Compilation :ref:`Client <mcp-client-prompt-compilation-request>`/:ref:`Server <mcp-server-working-with-prompts>`     |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Logging            |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Sending/Reading Logs :ref:`Client <mcp-client-reading-notifications>`/:ref:`Server <mcp-server-sending-log-message>`                    |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Pagination         |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Listing with Pagination :ref:`Client <mcp-client-listing-tools>`                                                                        |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Ping               |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | TBD                                                                                                                                     |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Progress           |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Sending/Reading Progress Notification :ref:`Client <mcp-client-reading-notifications>`/:ref:`Server <mcp-server-progress-notification>` |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Cancellation       |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | TBD                                                                                                                                     |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Roots              |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Adding/Removing/Listing Roots :ref:`Client <mcp-client-add-remove-roots>`/:ref:`Server <mcp-server-listing-roots>`                      |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Sampling           |  :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | Sending/Handling Sampling Requests :ref:`Client <mcp-client-handle-sampling>`/:ref:`Server <mcp-server-sending-sampling-requests>`      |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+
| Resource Templates | :octicon:`x-circle-fill;1em;sd-text-danger`       | :octicon:`x-circle-fill;1em;sd-text-danger`      |                                                                                                                                         |
+--------------------+---------------------------------------------------+--------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------+

-------------------
Additional Features
-------------------

* HTTP API key protection support 
* HTTPS support through OpenSSL
* JSON RPC 2.0 batch processing
* Custom MCP request/notification/response implementation support
* Monitoring standard MCP callbacks (requests/notifications/responses)
* Registering/Unregistering features dynamically while the client/server is running