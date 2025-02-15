==========================
Processor Object in Detail
==========================

Processor object is an abstract object declared as :code:`InfProcessorBase` in header file :code:`mbase/inference/inf_processor.h`.

It is responsible for multiple things as listed below:

- Abstraction over llama.cpp C context SDK.
- Creating a context for the inference.
- Provides non-blocking model inference methods for the target model.
- Provides the tokenizer.

When an LLM is loaded into the program memory using the methods of :doc:`model object <model-in-detail>`, the model object waits for processors to be registered. 
When a single or multiple processor objects get registered into the model object, their states are updated everytime when the model object state is updated by 
calling the :code:`update` method of the model object. Thus, the model object will act as a scheduler and synchronization point for multiple processor objects
while those processor objects are doing the requested inference operation in parallel.

The :doc:`model object <model-in-detail>` also handles the case in which if the model is destroyed during the inference process, it will destroy the contexts of the registered
processors, unregister all of them automatically, and will gracefully unload the model.

However, if the processor is not registered into the :doc:`model object<model-in-detail>` but only initialized, the user should manually update the processor state by calling
its :code:`update` method and be careful not to do inference operations if the :doc:`model object<model-in-detail>` is not valid.

Since, it also inherits from a :code:`logical_processor` object, it is assumed to be a signal driven parallel state machine.

.. note::
   
   See :doc:`parallel-state-machine`. 

------
Naming
------

It is named as processor because it literally processes your input and generates the response using the LLM.

Its derivatives are expected to derive this object and name themselves like: 

- InfProcessorTextToText
- InfProcessorEmbedder
- InfProcessorSpeechToText (not implemented)
- InfProcessorImageTextToText (not implemented)

Currently, the :code:`InfProcessorTextToText` and :code:`InfProcessorEmbedder` has been implemented and the :code:`InfProcessorTextToText` is the main subject of this page.

------------------------------------
Identifying the Expensive Operations
------------------------------------

The operations such as, context creation/deletion, input batch processing and output token generation can be considered as an expensive operations which block the main application thread for a long period of time.

Out of all above, the most expensive one is the input batch processing where your input is supplied to inference engine as batches and all matrix calculations are applied during this period. The output token generation speed will be the same regardless of your input. Both is highly affected by the parameter count and quantization type.

The tokenization operation is not an expensive operation so it is handled synchronously.

Here is a list of methods that correspond to expensive operations:

- :code:`initialize`: Create and initialize the context.
- :code:`initialize_sync`: Synchronized initialize.
- :code:`destroy`: Destroy the context.
- :code:`destroy_sync`: Synchronized destroy.
- :code:`declare_lora_assign`: Assigns a lora adapter into the context. 
- :code:`declare_lora_remove`: Remove an assigned lora adapter from the context.
- :code:`execute_input`: Signals the parallel state machine to batch process your input.
- :code:`execute_input_sync`: Synchronized execute.
- :code:`next`: Signals the parallel state machine to compute the next token.
- :code:`next_sync`: Synchronized next.

-------------------
Essential Callbacks
-------------------

.. important::

   LoRA related callbacks are not mentioned here.


There are some essential callbacks that derived classes must implement to inherit from in order to catch the events related to the processor object.

Those callbacks are as follows:

- :code:`on_initialize`: This is called if the context creation is successful.
- :code:`on_initialize_fail(last_fail_code out_code)`: This is called if the context creation is failed in parallel thread and the fail reason is stored in the out argument :code:`out_code`.
- :code:`on_destroy`: This is called if the processor context is destroyed.

-----------------
Essential Signals
-----------------

.. important::

   LoRA related signals are not mentioned here.

User can observe signals on the processor object to see if an operation is still operating in parallel.
Here are the essential signals that can be observed in the program loop:

* :code:`signal_state_initializing()`: If this is true, it indicates that the processor is initialized and the processor object should be updated by calling :code:`update()`.
* :code:`signal_state_destroying()`: If this is true, it means that the processor is destroyed and the processor object should be updated by calling :code:`update()`.
* :code:`signal_initializing()`: It is true if the processor is actively being initialized in parallel.
* :code:`signal_destroying()`: It is true if the processor is actively being destroyed in parallel.

.. code-block:: cpp
   :caption: mbase/inference/inf_processor.h 

   class MBASE_API InfProcessorBase : public mbase::logical_processor {
   public:
      ...
      MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_initializing() const noexcept;
      MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_destroying() const noexcept;
      MBASE_ND(MBASE_OBS_IGNORE) bool signal_initializing() const noexcept;
      MBASE_ND(MBASE_OBS_IGNORE) bool signal_destroying() const noexcept;
      ...
   protected:
      ...
   };

* :code:`signal_state_input_process()`:
* :code:`signal_state_decode_process()`:
* :code:`signal_state_kv_locked_process()`:
* :code:`signal_input_process()`:
* :code:`signal_decode_process()`:

-------------------------
TextToText Execution Flow
-------------------------

1. In order to do an inference operation using the processor, you first need to register the processor into the model object. See :ref:`model-processor-registration`.

2. Then you need to register your TextToText client to the processor.

3. Then you need to tokenize your input using the tokenization methods. See :doc:`message-preparation`.

4. Then you will execute your input.

5. Then you will compute and generate tokens.

.. tip::

   In the documentation, there is a fully-implemented example. See :doc:`Single-Prompt Example <../quickstart/single_prompt_ex/about>`

^^^^^^^^^^^^^^^^^^^^^^^^^^^
Client Registration Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Input Execution and Token Generation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

--------
Advanced
--------

^^^^^^^^^^^^^^^^^^^^^^^^^^^
Decode Behavior Description
^^^^^^^^^^^^^^^^^^^^^^^^^^^

// GENERATION BEHAVIOR MANIP

^^^^^^^^^^^^^^
Manual Caching
^^^^^^^^^^^^^^

// FOR PROMPT CACHING

^^^^^^^^^^^^^^^^
Context Shifting
^^^^^^^^^^^^^^^^

// FOR INFINITE TOKEN GENERATION