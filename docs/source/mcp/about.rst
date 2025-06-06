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

MBASE MCP Library is an MCP Client/Server SDK implementation in C++ with near full-featured implementation with the exception of resource templates and SSE.

A full MCP documentation can be found in their official documentation website: https://modelcontextprotocol.io/introduction

-----------------
Transport Methods 
-----------------

+-----------------------------+--------------------------------------------------+------+
| Method                      | Support                                          | Docs |
+=============================+==================================================+======+
| stdio                       | :octicon:`check-circle-fill;1em;sd-text-success` | TBD  |
+-----------------------------+--------------------------------------------------+------+
| Streamable HTTP Stateless   | :octicon:`check-circle-fill;1em;sd-text-success` | TBD  |
+-----------------------------+--------------------------------------------------+------+
| Streamable HTTP Stateful    | :octicon:`check-circle-fill;1em;sd-text-success` | TBD  |
+-----------------------------+--------------------------------------------------+------+
| SSE                         | :octicon:`x-circle-fill;1em;sd-text-danger`      | TBD  |
+-----------------------------+--------------------------------------------------+------+

----------------------
Feature Support Matrix 
----------------------

+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Feature                      | Client                                           | Server                                          | Docs|
+==============================+==================================================+=================================================+=====+
| Prompts                      | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| asd |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Resources                    | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Tools                        | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Prompt Compilation           | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Logging                      | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Pagination                   | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Ping                         | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Progress                     | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Cancellation                 | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Roots                        | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Sampling                     | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success`| TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+
| Resource Templates           | :octicon:`x-circle-fill;1em;sd-text-danger`      | :octicon:`x-circle-fill;1em;sd-text-danger`     | TBD |
+------------------------------+--------------------------------------------------+-------------------------------------------------+-----+

-------------------
Additional Features
-------------------

* HTTP API key protection support 
* HTTPS support through OpenSSL
* JSON RPC 2.0 batch processing
* Custom MCP request/notification/response implementation support
* Monitoring standard MCP callbacks (requests/notifications/responses)
* Registering/Unregistering features dynamically while the client/server is running