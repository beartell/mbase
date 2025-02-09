.. include:: ../links.rst

=============
SDK Structure
=============

MBASE Inference SDK consists of three parts given as; inference part, gguf configuration part and common part.
Even though the common part is interconnected with the inference part, they are individual on their own.

In this document, we will explain what those parts are, what are their usage and how to use them.

-----------------------
File Naming and Purpose
-----------------------

Inference SDK headers reside under the :code:`mbase/inference` directory where each header file prefixed by the word :code:`inf_`.
For example, the header file contains the necessary objects and procedures for device
query mechanism is under the file named as :code:`mbase/inference/inf_device_desc.h`.

Here are the list of header files under the :code:`mbase/inference` directory grouped into their respective parts
in the SDK.

**Inference Part Files**

- :code:`inf_client.h`: Contains the client base class for client implementation.
- :code:`inf_t2t_client.h`: Contains Text-To-Text client implementation. 
- :code:`inf_embedder_client.h`: Contains the embedder client implementation.
- :code:`inf_model.h`: Contains the model base class for model implementation. 
- :code:`inf_t2t_model.h`: Contains the Text-To-Text model implementation.
- :code:`inf_processor.h`: Contains the processor base class for processor implementation.
- :code:`inf_t2t_processor.h`: Contains the Text-To-Text processor implementation.
- :code: `inf_embedder.h`: Contains the embedder processor implementation.
- :code:`inf_t2t_proc_diagnostics.h`: A diagnostics objects that is being used by the processors object.

**GGUF Part Files**

- :code:`inf_gguf_metadata_configurator.h`: Contains the gguf metadata configurator object for gguf manipulation. See :doc:`gguf-files`.

**Common Part Files**

- :code:`inf_chat_templates.h`: List of hard-coded chat templates and a procedure for formatting 
conversation.

- :code:`inf_common.h`: Useful aliases and procedures for common use.
- :code:`inf_context_line.h`: Contains the :code:`context_line` structure for conversation forming. See :doc:`message-preparation`.
- :code:`inf_device_desc.h`: Contains the necessary objects and procedures for device query mechanism. See :doc:`obtaining-hardware-info`.
- :code:`inf_sampling_set.h`: Contains the sampler description struct which is used for sampling.

------------
Dependencies
------------

The inference SDK internally resolves the dependencies when you find the library using cmake.
However, it is worth mentioning the dependencies of the inference SDK.

The inference SDK depends on the following libraries:

- libc: C Standard library.
- libc++: C++ Standard library at least c++17.
- `llama.cpp <llama.cpp_>`_: It is the main inference engine used by the SDK.
- MBASE STD: MBASE standard library which is not documented yet.
- MBASE PC: MBASE Program core library which is not documented yet.

---------------
Finding the SDK
---------------

If you have installed the MBASE SDK, you can find the library using CMake :code:`find_package` function
with components specification.

In order to find the library using cmake, write the following:

.. code-block:: cmake

    find_package(mbase.libs REQUIRED COMPONENTS inference)

This will find the inference SDK. In order to link both include directories and link libraries,
write the following:

.. code-block:: cmake

    target_compile_features(<your_target> PUBLIC cxx_std_17)
    target_link_libraries(<your_target> PRIVATE mbase-inference)
    target_include_directories(<your_target> PUBLIC mbase-inference)

.. note::
    
    The library dependencies are internally linked into the mbase-inference so, you don't need to worry about that.

--------------
Inference Part
--------------

The inference part of the SDK is the most important where you will spend almost all of your time using this
part of the SDK.

It implements objects such as model, processor and client objects which are essential for LLM inference.

The model object and its procedures are for essential operations such as loading/unloading, LoRA adapters registering processors
etc. The processor object and its procedures are for applying encoding-decoding opeations or doing inference in genral for given LLM.
The client object is for observing and catching the result of the inference operation that is produced by the processor object.

They are explained in detail in their respective chapters.

See :doc:`model-in-detail`

See :doc:`processor-in-detail`

See :doc:`client-in-detail`

^^^^^^
Naming
^^^^^^

In the LLM world, there are tags assigned to LLMs such as Text-To-Text, Image-To-Text, Speech-To-Text etc.
which indicates the purpose and direction of the LLM that is being processed. The model, processor and client objects
are named based off-of the direction of the LLM object. For example, the model object for Text-To-Text inference is named
as :code:`InfModelTextToText` or the processor object in this regard named as :code:`InfProcessorTextToText`.

-----------------------
GGUF Configuration Part
-----------------------

This part contains a single file provides necessary functionality for manipulating or displaying gguf files.
For more detailed information, refer to :doc:`gguf-files`.

-----------
Common Part
-----------

This part contains some common utility structs or type aliases to be leveraged by the
inference SDK.
