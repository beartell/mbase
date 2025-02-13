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
* Benchmark application for measuring the impact of LLM inference on your application.
* Plus anything `llama.cpp <llama.cpp_>`_ supports.

---------------------
Implementation Matrix
---------------------

+------------+--------------------------------------------------+--------------------------------------------------+---------------------------+
| Type       | SDK Support                                      | Openai API Support                               | Engine                    |
+============+==================================================+==================================================+===========================+
| TextToText | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | `llama.cpp <llama.cpp_>`_ |
+------------+--------------------------------------------------+--------------------------------------------------+---------------------------+
| Embedder   | :octicon:`check-circle-fill;1em;sd-text-success` | :octicon:`check-circle-fill;1em;sd-text-success` | `llama.cpp <llama.cpp_>`_ |
+------------+--------------------------------------------------+--------------------------------------------------+---------------------------+

-------------------
Supported Platforms
-------------------

* Mac OS
* Linux
* Windows

-----------------------
Download and Setting up
-----------------------

Download page: :doc:`quickstart/setup/download`

SDK setup and compiling from source: :doc:`quickstart/setup/setting_up`

---------------
Useful Programs
---------------

^^^^^^^^^^^^^
Openai Server
^^^^^^^^^^^^^

Detailed documentation: :doc:`programs/openai-server/about`

An Openai API compatible HTTP/HTTPS server for serving LLMs.
This program provides chat completion API for TextToText models and embeddings API For embedder models.

**Usage:**

.. code-block:: bash

    mbase_openai_server *[option [value]]
    mbase_openai_server --hostname "127.0.0.1" -jsdesc description.json
    mbase_openai_server --hostname "127.0.0.1" --port 8080 -jsdesc description.json
    mbase_openai_server --hostname "127.0.0.1" --port 8080 -jsdesc description.json
    mbase_openai_server --hostname "127.0.0.1" --port 8080 --ssl-pub public_key_file --ssl-key private_key_file -jsdesc description.json

^^^^^^^^^^^^^
Benchmark T2T
^^^^^^^^^^^^^

Detailed documentation: :doc:`programs/benchmark-t2t/about`

It is a program written to measure the performance
of the given T2T LLM and its impact on your main application logic.

**Usage:**

.. code-block:: bash

    mbase_benchmark_t2t model_path *[option [value]]
    mbase_benchmark_t2t model.gguf -uc 1 -fps 500 -jout .
    mbase_benchmark_t2t model.gguf -uc 1 -fps 500 -jout . -mdout .

^^^^^^^^^
Embedding
^^^^^^^^^

Detailed documentation: :doc:`programs/embedding/about`

An example program for generating the embeddings of the given prompt or prompts.

**Usage:**

.. code-block:: bash

    mbase_embedding_simple model_path *[option [value]]
    mbase_embedding_simple model.gguf -gl 80 -p 'What is life?'
    mbase_embedding_simple model.gguf -gl 80 -pf prompt1.txt -pf prompt2.txt

^^^^^^^^^
Retrieval
^^^^^^^^^

Detailed documentation: :doc:`programs/retrieval/about`

An example for calculating the distance between the
given query and multiple text files/documents and applying a retrieval operation.

**Usage:**

.. code-block:: bash

    mbase_retrieval model_path *[option [value]]
    mbase_retrieval model.gguf -q 'What is MBASE' -pf file1.txt -pf file2.txt -gl 80


^^^^^^^^^^^^^^^^^^^
Simple Conversation
^^^^^^^^^^^^^^^^^^^

Detailed documentation: :doc:`programs/simple-conversation/about`

It is a simple executable program where you are having
a dialogue with the LLM you provide. It is useful for examining the answer of the LLM since the system prompt 
and sampler values can be altered.

**Usage:**

.. code-block:: bash

    mbase_simple_conversation model_path *[option [value]]
    mbase_simple_conversation model.gguf
    mbase_simple_conversation model.gguf -gl 80
    mbase_simple_conversation model.gguf -gl 80 -sys 'You are a helpful assistant.'

^^^^^^^^^^
Typo Fixer
^^^^^^^^^^

Detailed documentation: :doc:`programs/typo-fixer/about`

This is an applied example use case of the MBASE library.
The program is reading a user-supplied text file and fixing the typos.

**Usage:**

.. code-block:: bash

    mbase_typo_fixer model_path *[option [value]]
    mbase_typo_fixer model.gguf -gl 80 -s typo.txt -o fixed.txt

------------------
SDK Usage Examples
------------------

* :doc:`Single-Prompt <quickstart/single_prompt_ex/about>`: Simple prompt and answer example. At the end of the example, the user will supply a prompt in the terminal and LLM will give the response.
* :doc:`Dialogue-Example <quickstart/dialogue_ex/about>`: More complex dialogue based prompt and answer example. At the end of the example, the user will be able to have a dialogue with LLM using terminal.
* :doc:`Embedding-Example <quickstart/embeddings_ex/about>` :Vector embedding generator which is generally used by RAG programs and more. At the end of the example, the user will supply an input and vector embeddings will be generated by using embedder LLM model.

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