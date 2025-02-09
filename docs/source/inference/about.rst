.. include:: links.rst

=================
Inference Library
=================

Welcome to the MBASE inference library documentation!

-----
About
-----

MBASE inference library is a high-level non-blocking LLM inference library written on top
of the `llama.cpp <llama.cpp_>`_ library to provide the necessary tools and APIs to allow developers
to integrate popular LLMs into their application with minimal performance loss and development time.

The inference SDK will allow developers the utilize the LLMs and create their own solutions,
tooling mechanisms etc.

When the phrase "local LLM inference" is thrown around,
it usually means that hosting an Openai API compatible HTTP server and using the completions API locally.
The MBASE inference library is expected to change this notion by providing you the LLM inference capability 
through its low-level objects and procedures so that 
you can integrate and embedd LLM into your high-performance application such as games, server applications and many more.

Also you still have the option of hosting Openai server using the :doc:`mbase_openai_server <programs/openai-server/about>` program or you can
code a similar one yourself!

There also is a benchmarking tool developed for testing the performance of the LLM but more specifically,
the impact of the inference operation on your main program loop. For further details refer to :doc:`benchmark <programs/benchmark-t2t/about>` documentation.

--------
Features
--------

* Non-blocking TextToText LLM inference SDK.
* Non-blocking Embedder model inference SDK.
* GGUF file meta-data manipulation SDK.
* Openai server program supporting both TextToText and Embedder endpoints with system prompt caching support which implies significant performance boost.
* Hosting multiple models in a single Openai server program.
* Using `llama.cpp <llama.cpp_>`_ as an inference backend so that models that are supported by the `llama.cpp <llama.cpp_>`_ library are supported by default.
* Benchmark application for measuring the impact of LLM inference on your applications.
* Plus anything `llama.cpp <llama.cpp_>`_ supports.

-------------------
Supported Platforms
-------------------

* Mac OS
* Linux
* Windows

-------------------
Document Navigation
-------------------

The document is structured into four chapters where each chapter has a distinct purpose.
Those chapters are as follows:

* :doc:`Quickstart <quick_start>`: This chapter starts by explaining how to setup your environment, and goes with system requirements identification, library installations and simple project examples to show how to use the inference SDK.

* :doc:`Programs <programs/index>`: This chapter explains the programs that are developed using the inference library. The programs in that chapter are complete executable programs.

* :doc:`Information Reference <info-reference/index>`: This chapter is for explaining the concepts, inference and SDK in detail. It is your goto if you struggle to understand some parts of the code, or struggling to understand the SDK usage, or examples in quickstart chapter. 

.. toctree::
    :hidden:

    Quickstart <quick_start>
    Programs <programs/index>
    Information Reference <info-reference/index>