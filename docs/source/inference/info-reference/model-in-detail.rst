======================
Model Object in Detail
======================

Model object is an abstract object declared as :code:`InfModelBase` in header file :code:`mbase/inference/inf_model.h`.
It's interface is designed to provide expensive operations such as model initialization and destruction,
in a non-blocking manner.

Since it also inherits from a :code:`logical_processor` object, it is assumed to be a signal driven parallel state machine.

.. note::
    
    See :doc:`parallel-state-machine`.

Model object is an object which is responsible for 

* Providing an abstraction over low-level inference engine, which is llama.cpp in our case.
* Providing an information about model.
* Initialization and destruction of the model.
* Initialization and destruction of adapters.
* Processor registration and management.

------
Naming
------

It's derivatives are expected to derive this object and name themselves like:

* InfModelTextToText
* InfModelSpeechToText (not implemented)
* InfModelImageTextToText (not implemented)

Currently, the :code:`InfModelTextToText` has been implemented and :code:`InfModelTextToText` is the main subject of this page.

------------------------------------
Identifying the Expensive Operations
------------------------------------

The model operations such as initialization and destruction are
expensive operations which blocks the main application thread for a long period of time.

The reason for that is the size of the model files "gguf" and loading the tensors that are defined in that file
into the program memory. When you couple this with the fact that if you attempt to run multiple LLMs and load/unload many of them
in a continuous manner, it would be highly unacceptable.

For that reason, :code:`InfModelTextToText` provides a non-blocking model initialization and destruction through its corresponding
methods:

* :code:`initialize_model`: Signals the model initialization.
* :code:`initialize_model_ex`: Signals the model initialize with extra parameters.
* :code:`initialize_model_sync`: Synchronized initialize.
* :code:`initialize_model_ex_sync`: Synchronized initialize ex.
* :code:`destroy`: Signals the model destruction and unregister all processors.
* :code:`destroy_sync`: Synchronized destroy.

The methods can be seen in the :code:`InfModelTextToText` header synopsis:

.. code-block:: cpp
    :caption: mbase/inference/inf_t2t_model.h

    class MBASE_API InfModelTextToText : public InfModelBase{
        public:
            ...
            flags initialize_model_ex(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices = mbase::vector<InfDeviceDescription>());
            flags initialize_model(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1);
            flags initialize_model_ex_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices = mbase::vector<InfDeviceDescription>());
            flags initialize_model_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1);
            flags destroy();
            flags destroy_sync();
            ...
        private:
            ...
    }

Now, we will talk about how the model initialization and destruction work non-blockingly and the workflow.

-------------------
Essential Callbacks
-------------------

.. important::

    LoRA related callbacks are not mentioned here.

There are some essential callbacks that derived classes must implement to inherit from
in order to catch the events related to model object.

Those callbacks are as follows:

* :code:`on_initialize_fail(init_fail_code out_fail_code)`: This is invoked if the model initialization is failed in the parallel thread and the fail reason is stored in the out argument :code:`out_fail_code`.
* :code:`on_initialize`: This is invoked if the model initialization is successful.
* :code:`on_destroy`: This is invoked if the model is being destroyed. It is not invoked on initialization etc. and it is not directly related to model class object destruction. It is only related to destruction of the already initialized model.

Here is how they look like in the model base class:

.. code-block:: cpp
    :caption: mbase/inference/inf_model.h 

    class MBASE_API InfModelBase : public mbase::logical_processor{
        public:
            ...
            virtual GENERIC on_initialize_fail(init_fail_code out_fail_code) = 0;
            virtual GENERIC on_initialize() = 0;
            virtual GENERIC on_destroy() = 0;
            ...
        private:
            ...
    }

-----------------
Essential Signals
-----------------

.. important::

    LoRA related signals are not mentioned here.

User can observe signals on the model object to see if the model initialization/destruction is still operating in parallel.
Here are the essential signals that can be observed in the program loop:

* :code:`signal_state_initializing()`: If this is true, it indicates that the model is initialized and the model object should be updated by calling :code:`update()`.
* :code:`signal_state_destroying()`: If this is true, it means that the model is destroyed and the model object should be updated by calling :code:`update()`.
* :code:`signal_initializing()`: It is true if the model is actively being initialized in parallel.
* :code:`signal_destroying()`: It is true if the model is actively being destroyed in parallel.

.. code-block:: cpp
    :caption: mbase/inference/inf_model.h 

    class MBASE_API InfModelBase : public mbase::logical_processor{
        public:
            ...
            MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_initializing() const;
            MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_destroying() const;
            MBASE_ND(MBASE_OBS_IGNORE) bool signal_initializing() const;
            MBASE_ND(MBASE_OBS_IGNORE) bool signal_destroying() const;
            ...
        private:
            ...
    }

--------------------------
General Operation Workflow
--------------------------

The calling convention of the inference SDK is pretty simple where most methods return an enum named :code:`flags` which
gives information about how the operation went. For example, when you attempt to call the :code:`initialize_model` method without
giving a valid path, the method will return a :code:`INF_MODEL_ERR_MISSING_MODEL` flag indicating that the model is not found
and the initialization operation is not started or, you attempt to destroy a model that is not being initialized by calling
the :code:`destroy` method, it will return :code:`INF_MODEL_SUCCESS` indicates that the destruction operation is already done because
model is not initialized in the first place.

A call to those methods will not block on success. When you validly call the :code:`initialize_model` method it will return 
a flag :code:`INF_MODEL_INFO_INITIALIZING_MODEL` indicating that the initialization operation is started in another thread.
By knowing this fact, you will constantly update your model object's state by calling the :code:`update` method so that when the
model initialization is finished, the respective callback :code:`on_initialize` will be called accordingly or :code:`on_initialize_fail`
if the initialization is failed.

So the flow of operations can be listed like this in order:

1. Derive from the corresponding parent class and implement the essential callbacks.
2. Call the non-blocking method and observe the returned flag.
3. Observe the signals if necessary.
4. Update the model state every frame by calling the :code:`update` method of the model.
5. Good to go!

.. _model-init-example-info:

^^^^^^^^^^^^^^^^^^^^^^
Initialization Example
^^^^^^^^^^^^^^^^^^^^^^

Now, we will write a program which will initialize and then destroy the model.

Here is a hello world code:

.. code-block:: cpp
   :caption: main.cpp

    #include <iostream>

    int main()
    {
        std::cout << "Hello world!!" << std::endl;
        return 0;
    }

1. Lets derive our class from :code:`InfModelTextToText`:

.. code-block:: cpp
    :caption: main.cpp

    #include <iostream>
    #include <mbase/inference/inf_t2t_model.h> // Where InfModelTextToText reside

    class SampleModel : public mbase::InfModelTextToText {
        public:
            void on_initialize_fail(init_fail_code out_fail_code) override
            {
                std::cout << "Model is not initialized ;(" << std::endl;
                exit(1);
            }

            void on_initialize() override
            {
                std::cout << "Model is initialized!" << std::endl;
                this->destroy();
            }

            void on_destroy() override
            {
                std::cout << "Model is destroyed!" << std::endl;
                exit(0);
            }
        private:
    }; 

    int main()
    {
        std::cout << "Hello world!!" << std::endl;
        return 0;
    }

2- Call the non-blocking method and observe the returned flag:

.. code-block:: cpp
   :caption: main.cpp
   
   int main()
   {
       SampleModel modelObject;
       SampleModel::flags resultFlag = modelObject.initialize_model(
           L"<path_to_your_model>",
           4096 // Context size,
       );

       if(resultFlag != SampleModel::flags::INF_MODEL_INFO_INITIALIZING_MODEL)
       {
           std::cout << "Unable to start model initialization." << std::endl;
           return 1;
       }


       return 0;
   }

3. Update the model state every frame by calling the :code:`update` method of the model.

.. code-block:: cpp
   :caption: main.cpp

   while(1)
   {
       modelObject.update();
       mbase::sleep(5);
   }

.. _model-processor-registration:

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Processor Registration Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Now, we will write a program which will register a processor and then exit the program. We will use the code we implemented in :ref:`model-init-example-info` as a base implementation. 

The processor registration workflow is as follows:

**TODO: Reference the context processor documentation**

1. Derive from the corresponding parent context processor and implement the essential callbacks.  
2. Register the processor using :code:`register_context_process` and observe the returned flag.

First, lets derive our class from :code:`InfProcessorTextToText` and implement the essential callbacks:

.. code-block:: cpp
    :caption: main.cpp 
    
    #include <mbase/inference/inf_t2t_processor.h> // Where InfProcessorTextToText reside
    class SampleProcessor : public mbase::InfProcessorTextToText {
        public:
            void on_initialize_fail(last_fail_code out_code) override
            {
                std::cout << "Processor registration failed ;(" << std::endl;
            }

            void on_initialize() override
            {
                std::cout << "Processor is initialized!" << std::endl;
            }

            void on_destroy() override
            {
                std::cout << "Processor is destroyed!" << std::endl;
            }         
        private:
    };

    class SampleModel : public mbase::InfModelTextToText {
        public:
            ...
        private:
            SampleProcessor mProcessor;
    }; 
    
Then, let's register our processor after the model is being initialized.

.. code-block:: cpp
    :caption: main.cpp
    
    class SampleModel : public mbase::InfModelTextToText {
        public:
            ...
            void on_initialize() override
            {
                std::cout << "Model is initialized!" << std::endl;
                if(this->register_context_process(
                    &mProcessor,
                    2048, // context length
                    512, // batch size
                    8, // generation thread count
                    16, // batch processing thread count
                    true, // is flash attention enabled
                    {} // set of samplers
                ) == SampleModel::flags::INF_MODEL_INFO_REGISTERING_PROCESSOR)
                {
                    std::cout << "Processor registration started!" << std::endl;
                }
            }
            ... 
        private:
            SampleProcessor mProcessor;
    }; 

--------
Synopsis
--------

^^^^^^^^^^^^^^^^^
Base Model Object
^^^^^^^^^^^^^^^^^

.. code-block:: cpp
   :caption: mbase/inference/inf_model.h

    #include <mbase/common.h>
    #include <mbase/string.h>
    #include <mbase/list.h>
    #include <mbase/vector.h>
    #include <mbase/unordered_map.h>
    #include <mbase/behaviors.h>
    #include <mbase/thread.h>
    #include <mbase/framework/logical_processing.h>
    #include <mbase/framework/thread_pool.h>
    #include <mbase/framework/timer_loop.h>
    #include <mbase/inference/inf_common.h>
    #include <llama.h>

    MBASE_BEGIN

    class InfModelBase;
    class InfProcessorBase;

    class MBASE_API InfModelBase : public mbase::logical_processor {
    public:
        using size_type = SIZE_T;
        using watcher_type = mbase::inf_processor_watcher<InfProcessorBase>;
        using context_processor_list = mbase::list<watcher_type>;
        using iterator = typename context_processor_list::iterator;
        using const_iterator = typename context_processor_list::const_iterator;
        using reverse_iterator = typename context_processor_list::reverse_iterator;
        using const_reverse_iterator = typename context_processor_list::const_reverse_iterator;

        enum class init_fail_code : U8 {
            NOT_ENOUGH_MEMORY,
            MBASE_PARAMS_DONT_MATCH,
            PATH_NOT_FOUND,
            LLAMA_SYSTEM_ERROR,
            UNDEFINED
        };

        /* ===== BUILDER METHODS BEGIN ===== */
        InfModelBase() noexcept;
        virtual ~InfModelBase() noexcept;
        /* ===== BUILDER METHODS END ===== */

        /* ===== ITERATOR METHODS BEGIN ===== */
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) iterator begin() noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) iterator end() noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_iterator begin() const noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_iterator end() const noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_iterator cbegin() const noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_iterator cend() const noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) reverse_iterator rbegin() noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) reverse_iterator rend() noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_reverse_iterator crbegin() const noexcept;
        MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_reverse_iterator crend() const noexcept;
        /* ===== ITERATOR METHODS END ===== */

        /* ===== OBSERVATION METHODS BEGIN ===== */
        MBASE_ND(MBASE_OBS_IGNORE) const context_processor_list& get_registered_processors() const;
        MBASE_ND(MBASE_OBS_IGNORE) inf_model_category get_model_category() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool is_initialize_failed() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool is_initialized() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_initializing() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_destroying() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool signal_initializing() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool signal_destroying() const;
        GENERIC reset_base_signals();
        /* ===== OBSERVATION METHODS END ===== */

        /* ===== INTERFACE METHODS BEGIN =====*/
        virtual GENERIC on_initialize_fail(init_fail_code out_fail_code) = 0;
        virtual GENERIC on_initialize() = 0;
        virtual GENERIC on_destroy() = 0;
        /* ===== INTERFACE METHODS END =====*/

    protected:
        ...
    };

    // class MBASE_API InfModelImageToText : public InfModelBase{ // possibly using llava
    //	...
    // }

    // class MBASE_API InfModelSpeechToText : public InfModelBase{ // possibly using whisper.cpp
    //	...
    // }

    // class MBASE_API InfModelTextToImage : public InfModelBase{ // possibly using stable_diffusion.cpp
    //	...
    // }

    // class MBASE_API InfModelTextToSpeech : public InfModelBase{ // possibly using bark.cpp
    //	...
    // }

    // class MBASE_API InfModelEmbedding : public InfModelBase{ // Implement soon
    //	...
    // }

    MBASE_END

.. cpp:function:: iterator InfModelBase::begin()

   Registered processor iterator begin.

.. cpp:function:: InfModelBase::iterator begin() noexcept

    Context processor begin iterator.
.. cpp:function:: InfModelBase::iterator end() noexcept

    Context processor end iterator.
.. cpp:function:: InfModelBase::const_iterator begin() const noexcept

    Context processor const begin iterator.
.. cpp:function:: InfModelBase::const_iterator end() const noexcept
 
    Context processor end iterator.

.. cpp:function:: InfModelBase::const_iterator cbegin() const noexcept

    Context processor const begin iterator.
.. cpp:function:: InfModelBase::const_iterator cend() const noexcept

    Context processor const end iterator.
.. cpp:function:: InfModelBase::reverse_iterator rbegin() noexcept

    Context processor reverse begin iterator.
.. cpp:function:: InfModelBase::reverse_iterator rend() noexcept

    Context processor reverse end iterator.
.. cpp:function:: InfModelBase::const_reverse_iterator crbegin() const noexcept

    Context processor const reverse begin iterator.
.. cpp:function:: InfModelBase::const_reverse_iterator crend() const noexcept

    Context processor const reverse end iterator.
.. cpp:function:: const InfModelBase::context_processor_list& get_registered_processors() const

    Context processor list reference. Model object stores context processors internally as a list which
    can be observed by calling this method.

.. cpp:function:: inf_model_category InfModelBase::get_model_category() const
    
    Returns the category of the model. The return enum is defined in :code:`mbase/inference/inf_common.h` as:
    
    .. code-block:: cpp
        
        enum class inf_model_category {
            TEXT_TO_TEXT,
            EMBEDDING,
            UNDEFINED
        };

    
.. cpp:function:: bool InfModelBase::is_initialize_failed() const
    
    Returns true if the model initialization is failed.    

.. cpp:function:: bool InfModelBase::is_initialized() const

    Returns true if the model is initialized.

.. cpp:function:: bool InfModelBase::signal_state_initializing() const
    
    Returns true if the model initialization is finished and model object awaits for frame update.

.. cpp:function:: bool InfModelBase::signal_state_destroying() const
    
    Returns true if the model destruction is finished and model object awaits for frame update.

.. cpp:function:: bool InfModelBase::signal_initializing() const
    
    Returns true if the model initialization process is active in parallel.

.. cpp:function:: bool InfModelBase::signal_destroying() const
    
    Returns true if the model destruction process is active in parallel.

.. cpp:function:: GENERIC InfModelBase::reset_base_signals()
    
    Resets the initialize and destroy signals. This method shouldn't be called.

^^^^^^^^^^^^^^^^^^^^^^^
TextToText Model Object
^^^^^^^^^^^^^^^^^^^^^^^

.. important::

    User can be confident that for every method that has a return type of :code:`flags`, can return one of the following flags on fail:
    
    * :code:`INF_MODEL_INFO_DESTROYING_MODEL`: Means the model destruction process is going on in parallel.
    * :code:`INF_MODEL_INFO_INITIALIZING_MODEL`: Means the model initialization process going on in another thread.
    * :code:`INF_MODEL_ERR_NOT_INITIALIZED`: Means the model is not initialized.

.. code-block:: cpp
   :caption: mbase/inference/inf_t2t_model.h

    #include <mbase/inference/inf_model.h>
    #include <mbase/inference/inf_sampling_set.h>
    #include <mbase/inference/inf_device_desc.h>

    MBASE_BEGIN

    class InfProcessorTextToText;
    class InfEmbedderProcessor;

    class MBASE_API InfModelTextToText : public InfModelBase {
    public:
        enum class flags : U8 {
            INF_MODEL_SUCCESS,
            INF_MODEL_ERR_CANT_LOAD_MODEL,
            INF_MODEL_ERR_MISSING_MODEL,
            INF_MODEL_ERR_NO_SENTENCE,
            INF_MODEL_ERR_UPDATE_LOOP_OCCUPIED,
            INF_MODEL_INFO_REGISTERING_PROCESSOR,
            INF_MODEL_INFO_INITIALIZING_MODEL,
            INF_MODEL_INFO_DESTROYING_MODEL,
            INF_MODEL_INFO_PROCESSOR_IS_BEING_DESTROYED,
            INF_MODEL_INFO_UPDATE_REQUIRED,
            INF_MODEL_ERR_PROC_UNMATCH, // Called if the registered processor match with the model
            INF_MODEL_ERR_PROCESSOR_ALREADY_REGISTERED,
            INF_MODEL_ERR_INVALID_INPUT,
            INF_MODEL_ERR_MODEL_CONTEXT_FULL,
            INF_MODEL_ERR_INVALID_CONTEXT_LENGTH,
            INF_MODEL_ERR_PROCESSOR_NOT_FOUND,
            INF_MODEL_ERR_PROCESSOR_BELONGS_TO_ANOTHER_MODEL,
            INF_MODEL_ERR_UNABLE_REGISTER_PROCESSOR,
            INF_MODEL_ERR_NOT_INITIALIZED,
            INF_MODEL_ERR_TOKENIZATION_FAILED,
            INF_MODEL_ERR_LORA_MISSING,
            INF_MODEL_ERR_LORA_NAME_MISSING,
            INF_MODEL_ERR_LORA_EXISTS,
            INF_MODEL_ERR_LORA_FILE_INVALID,
            INF_MODEL_ERR_LORA_OPERATION_ACTIVE,
            INF_MODEL_ERR_LORA_NOTHING_TO_OPERATE,
            INF_MODEL_ERR_GENERIC
        };

        /* ===== BUILDER METHODS BEGIN ===== */
        InfModelTextToText();
        ~InfModelTextToText();
        /* ===== BUILDER METHODS END ===== */

        /* ===== OBSERVATION METHODS BEGIN ===== */
        MBASE_ND(MBASE_OBS_IGNORE) bool signal_lora_operation() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_lora_operation() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool is_available(const U32& in_context_size) const;
        MBASE_ND(MBASE_OBS_IGNORE) bool is_embedding_model() const;
        MBASE_ND(MBASE_OBS_IGNORE) bool has_lora_adapter(const mbase::string& in_name, inf_lora_adapter& out_adapter);
        llama_model* get_raw_model();
        mbase::vector<inf_text_token> get_special_tokens() const;
        mbase::vector<mbase::string> get_special_tokens_string() const;
        const mbase::string& get_model_name() const;
        const mbase::string& get_architecture() const;
        const mbase::string& get_sys_start() const;
        const mbase::string& get_assistant_start() const;
        const mbase::string& get_usr_start() const;
        const mbase::string& get_sys_end() const;
        const mbase::string& get_assistant_end() const;
        const mbase::string& get_usr_end() const;
        inf_text_token get_eot_token() const;
        inf_text_token get_lf_token() const;
        I32 get_vocab_count() const;
        size_type get_size() const;
        U32 get_embedding_length() const;
        U32 get_head_count() const;
        U32 get_layer_count() const;
        U32 get_max_embedding_context() const;
        bool is_token_eof_generation(inf_text_token in_token) const;
        flags is_token_special(const mbase::string& in_string) const;
        flags is_token_control(inf_text_token in_token) const;
        const mbase::string& get_quantization_string() const;
        const U32& get_total_context_size() const;
        const U32& get_occupied_context_size() const;
        /* ===== OBSERVATION METHODS END ===== */

        /* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
        static bool get_mbase_chat_template_id(const mbase::string& in_architecture, mbase::string& out_id);
        /* ===== NON-MEMBER FUNCTIONS END ===== */

        /* ===== STATE-MODIFIER METHODS BEGIN ===== */
        flags initialize_model_ex(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices = mbase::vector<InfDeviceDescription>());
        flags initialize_model(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1);
        flags initialize_model_ex_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices = mbase::vector<InfDeviceDescription>());
        flags initialize_model_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1);
        flags destroy();
        flags destroy_sync();
        flags register_context_process(
            InfProcessorTextToText* in_processor, 
            const U32& in_context_length,
            U32 in_batch_size,
            U32 in_thread_count,
            U32 in_batch_thread_count,
            const bool& in_flash_attention,
            const inf_sampling_set& in_sampler_set
        );
        flags register_context_process(
            InfEmbedderProcessor* in_processor,
            const U32& in_context_length,
            U32 in_thread_count
        );
        flags declare_lora_remove(const inf_lora_adapter& in_adapter);
        flags declare_lora_adapter(const inf_lora_adapter& in_adapter);
        flags start_lora_operation();

        /* ===== STATE-MODIFIER METHODS END ===== */

        /* ===== NON-MODIFIER METHODS BEGIN ===== */
        flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens);
        /* ===== NON-MODIFIER METHODS END ===== */

        /* ===== INTERFACE METHODS BEGIN ===== */
        virtual GENERIC on_lora_operate(const mbase::vector<inf_lora_adapter>& out_active_loras);
        /* ===== INTERFACE METHODS END ===== */

        /* ===== STATE-MODIFIER METHODS BEGIN ===== */
        GENERIC update() override;
        GENERIC update_t() override;
        /* ===== STATE-MODIFIER METHODS END ===== */

    private:
       ... 
    };

    MBASE_END

.. cpp:function:: bool signal_lora_operation() const
    
    Returns true if the lora operation is active in parallel.

.. cpp:function:: bool signal_state_lora_operation() const
    
    Returns true if the lora operation is finished and the model object awaits frame update.
    
.. cpp:function:: bool is_available(const U32& in_context_size) const

    Returns true if there is enough context to be occupied in size given by the param :code:`in_context_size`. 

.. cpp:function:: bool is_embedding_model() const

    Returns true if the model object is an embedding model.

.. cpp:function:: bool has_lora_adapter(const mbase::string& in_name, inf_lora_adapter& out_adapter)
    
    Returns true if there is either a declared or initialized lora adapter in the model with name given by the param :code:`in_name`. It also passes the reference of that adapter object as an output argument to the user so that the user may observe the adapter.
    
.. cpp:function:: llama_model* get_raw_model()
    
    Returns the raw llama.cpp C SDK model pointer. It can be used by advanced users who want to leverage the capabilites of the low-level llama C SDK.

.. cpp:function:: mbase::vector<inf_text_token> get_special_tokens() const
    
    Returns the special tokens of the model's vocabulary.

.. cpp:function:: mbase::vector<mbase::string> get_special_tokens_string() const
    
    Returns the stringified special tokens of the model's vocabulary.

.. cpp:function:: const mbase::string& get_model_name() const
   
    Returns the model name which is extracted from the GGUF File.

.. cpp:function:: const mbase::string& get_architecture() const
    
    Returns the model architecture.

.. cpp:function:: const mbase::string& get_sys_start() const
    
    It returns the beginning of the system part of the template if the model chat template is defined in MBASE and there is a system prompt part in the model's chat template.

    Empty string if there is no system start template.

.. cpp:function:: const mbase::string& get_assistant_start() const
    
    It returns the beginning of the assistant part of the template if the model chat template is defined in MBASE and there is a assistant prompt part in the model's chat template.

    Empty string if there is no assistant start template.
    
.. cpp:function:: const mbase::string& get_usr_start() const
    
    It returns the beginning of the user part of the template if the model chat template is defined in MBASE and there is a user prompt part in the model's chat template.

    Empty string if there is no user start template.
   
.. cpp:function:: const mbase::string& get_sys_end() const
    
    It returns the ending of the system part of the template if the model chat template is defined in MBASE and there is a system prompt part in the model's chat template.
    
    Empty string if there is no system end template. 

.. cpp:function:: const mbase::string& get_assistant_end() const
       
    It returns the ending of the assistant part of the template if the model chat template is defined in MBASE and there is a assistant prompt part in the model's chat template.

    Empty string if there is no assistant end template.

.. cpp:function:: const mbase::string& get_usr_end() const

    It returns the ending of the user part of the template if the model chat template is defined in MBASE and there is a user prompt part in the model's chat template.

    Empty string if there is no user end template.

.. cpp:function:: inf_text_token get_eot_token() const

    Returns the end of token token.
    
.. cpp:function:: inf_text_token get_lf_token() const

    Returns the linefeed token.

.. cpp:function:: I32 get_vocab_count() const

    Returns the number of tokens in the model vocabulary.

.. cpp:function:: size_type get_size() const
    
    Returns the size of the model in bytes.

.. cpp:function:: U32 get_embedding_length() const

    Returns the embedding length. It is also called hidden layer in some contexts.

.. cpp:function:: U32 get_head_count() const
    
    Returns the kv head count. 

.. cpp:function:: U32 get_layer_count() const
    
    Number of layers in the model.

.. cpp:function:: U32 get_max_embedding_context() const

    Maximum context length of the embedder model.

.. cpp:function:: bool is_token_eof_generation(inf_text_token in_token) const
    
    Whether the given token is eof.

.. cpp:function:: flags is_token_special(const mbase::string& in_string) const
    
    Is the given string a special token.

.. cpp:function:: flags is_token_control(inf_text_token in_token) const
    
    Same as :code:`is_token_special`.

.. cpp:function:: const mbase::string& get_quantization_string() const
    
    Returns the quantization type of the model in string. For example, "Q4_0", "BF16", "Q8_0" etc.

.. cpp:function:: const U32& get_total_context_size() const

    Returns model context size.

.. cpp:function:: const U32& get_occupied_context_size() const

    Returns the total amount of context occupied by multiple context processors.

.. cpp:function:: flags initialize_model_ex(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices = mbase::vector<InfDeviceDescription>())
    
    Model initialization method with extra arguments. On success, it starts the model initialization in parallel and returns the :code:`INF_MODEL_INFO_INITIALIZING_MODEL`.
   
    The behavior of this method differs if the initialization signal already sent by the previous call of this method. To put it simple, if the user attempts to call this method multiple times in a row or in a message loop etc. one of the following behaviors may occur: 
    
    - If the model initialization is finished but the model frame is not updated, this method will return the flag :code:`INF_MODEL_INFO_UPDATE_REQUIRED`. In this case, model object's :code:`update` method should be called which will invoke the :code:`on_initialize` callback if the initialization is finished or :code:`on_initialize_fail` callback if the initialization is failed. 
   
    - If the model initialization is ongoing, the method will return the flag :code:`INF_MODEL_INFO_INITIALIZING_MODEL`. 
    
    - If the model initialization is completed and the model frame is updated, this method will return the flag :code:`INF_MODEL_SUCCESS`. For this reason, in order to initialize some other model using the same model object, you need to call the :code:`destroy` method of the object.
   
    Here is a brief description for each input parameter:
    
    :code:`in_path`: Path to the GGUF file of the model. It must be a valid file path and it must be readable by the process that uses the MBASE SDK. If not, the method will return :code:`INF_MODEL_ERR_MISSING_MODEL`.

    :code:`in_total_context_size`: Total amount of context that can be occupied by the context processors. This shouldn't be confused with the context size of the model.

    :code:`in_gpu_layers`: Total amount of model layers to be offloaded to GPU. You can specify full offload by giving exceedingly large number like 999. If there are no GPUs on the system, it is ignored.

    :code:`in_use_mmap`: Memory mapping the model file if it is true.

    :code:`in_use_mlock`: Whether to enable/disable memory locking.

    :code:`in_devices`: Vector of :code:`InfDeviceDescription` objects. If it is not supplied, all devices in the system will be used for inference. See :doc:`obtaining-hardware-info`.

.. cpp:function:: flags initialize_model(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1)
    
    It is the same method as :code:`initialize_model_ex` but with less parameters. Default values of the missing parameters are as follows:
    
    * :code:`in_use_mmap`: false
    * :code:`in_use_mlock`: true
    * :code:`in_devices`: All devices 

.. cpp:function:: flags initialize_model_ex_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers, bool in_use_mmap, bool in_use_mlock, mbase::vector<InfDeviceDescription> in_devices = mbase::vector<InfDeviceDescription>())

    Synchronized version of the :code:`initialize_model_ex` method. This method blocks the calling thread until the model initialization is finishes.

    What it basically does is that it busy waits on the initialize signal of the model object and by the time initialize signal is unset, it will return :code:`INF_MODEL_INFO_UPDATE_REQUIRED`. 
    
    Here is the source code of the method:

    .. code-block:: cpp

        initialize_model_ex(in_path, in_total_context_size, in_gpu_layers, in_use_mmap, in_use_mlock, in_devices);

        while(signal_initializing())
        {
            mbase::sleep(2);
        }

        if(!is_initialized())
        {
            return flags::INF_MODEL_ERR_CANT_LOAD_MODEL;
        }

        return flags::INF_MODEL_INFO_UPDATE_REQUIRED;

.. cpp:function:: flags initialize_model_sync(const mbase::wstring& in_path, const U32& in_total_context_size, const I32& in_gpu_layers = -1)
    
    Synchronized version of the :code:`initialize_model` method.
    
.. cpp:function:: flags destroy()
    
    This is the model destruction method. On success, it starts the model destruction process in parallel and returns the :code:`INF_MODEL_INFO_DESTROYING_MODEL`. This method doesn't destroy the class object but instead it just destroys the internally managed model. 
    
    The behavior of this method differs if the destruction signal already sent by the previous calls to this method. To put it simple, if the user attempts to call this method multiple times in a row or in a message loop etc. one of the following behaviors may occur:

    - If the model destruction process is finished but the model frame is not updated, this method will return the flag :code:`INF_MODEL_INFO_UPDATE_REQUIRED`. In this case, model object's :code:`update` method should be called which will invoke the :code:`on_destroy` callback.
    
    - If the model destruction is ongoing, the method will return the flag :code:`INF_MODEL_INFO_DESTROYING_MODEL`.

    - If the model destruction is completed and the model frame is updated, this method will return the flag :code:`INF_MODEL_SUCCESS`.

    - If the model object is not even initialized in the first and the user attempts to destroy it, the method will return the flag :code:`INF_MODEL_SUCCESS`.

.. cpp:function:: flags destroy_sync()
    
    Synchronized version of the :code:`destroy` method. It basically busy waits on the return value of the :code:`signal_state_destroying`. 

    If the method returns :code:`INF_MODEL_INFO_UPDATE_REQUIRED`, you should call the :code:`update` method of the model object.   

.. cpp:function:: flags register_context_process( InfProcessorTextToText* in_processor, const U32& in_context_length, U32 in_batch_size, U32 in_thread_count, U32 in_batch_thread_count, const bool& in_flash_attention, const inf_sampling_set& in_sampler_set)
    
    This is the context processor registration method. If the processor which is specified by the input parameter :code:`in_processor` is initialized beforehand, it only registers it into the model object's processor list. If not initialized beforehand, it internally calls the :code:`initialize` method of the :code:`InfProcessorTextToText`. 

    If the processor is not initialized beforehand and the :code:`register_context_process` has been called, it will return the :code:`INF_MODEL_INFO_REGISTERING_PROCESSOR`. Here is a list of flags that the :code:`register_context_process` may return:

    - :code:`INF_MODEL_ERR_PROC_UNMATCH`: Attempting to register an incompatible context processor. For example, attempting to register a :code:`InfProcessorTextToText`object into an embedding model object or vice-versa etc.
    
    - :code:`INF_MODEL_ERR_INVALID_INPUT`: If the :code:`in_processor` param is null or if the :code:`in_context_length` is zero.

    - :code:`INF_MODEL_ERR_PROCESSOR_ALREADY_REGISTERED`: Processor is already registered into some model object.
        
    - :code:`INF_MODEL_ERR_INVALID_CONTEXT_LENGTH`: If the :code:`in_context_length` number is less than minimum token count the model object expects which is 32.

    - :code:`INF_MODEL_INFO_REGISTERING_PROCESSOR`: Processor registeration is ongoing.
    
    - :code:`INF_MODEL_INFO_PROCESSOR_IS_BEING_DESTROYED`: Processor destruction is ongoing.

    - :code:`INF_MODEL_ERR_MODEL_CONTEXT_FULL`: Not enough context remaining in the model object.

    Here is a brief description for each input parameter:
        
    :code:`in_processor`: A processor object to be registered.

    :code:`in_context_length`: Amount of context to be occupied in the model object.

    :code:`in_batch_size`: LLM inference engines process the input in batches. This number is the size of the batch. If this number exceeds the :code:`in_context_length`, it will we bee it will be equalized with the context length internally.  

    :code:`in_thread_count`: Number of threads to be used during the token generation process.

    :code:`in_batch_thread_count`: Number of thread to be used during the batch processing.
    
    :code:`in_flash_attention`: Whether the flash attention is enabled. It is suggested to keep flash attention enabled since it most-likely to increase the performance and no impact on the quality of output of the model.
    
    :code:`in_sampler_set`: A set of samplers to be used when predicting the next token. See :doc:`on-sampling`. 

.. cpp:function:: flags register_context_process( InfEmbedderProcessor* in_processor, const U32& in_context_length, U32 in_thread_count)
    
    Same behavior as :code:`register_context_process` but its for embedder processor.

.. cpp:function:: flags declare_lora_remove(const inf_lora_adapter& in_adapter)
    
    Declaring a lora adapter to be removed. Remove declared adapters will be removed in parallel when the user calls :code:`start_lora_operation`.  
    
    Declaration method will return :code:`INF_MODEL_SUCCESS` on successful declaration. Otherwise, one of the following flags may return:
    
    - :code:`INF_MODEL_ERR_LORA_MISSING`: Attempting to remove non-existing lora.
    
    - :code:`INF_MODEL_ERR_LORA_OPERATION_ACTIVE`: Lora operation is started and is ongoing.   
   
    - :code:`INF_MODEL_ERR_LORA_NAME_MISSING`: Adapter name is missing.

    - :code:`INF_MODEL_INFO_UPDATE_REQUIRED`: Lora operation started and finished and the model frame should be updated. 

.. cpp:function:: flags declare_lora_adapter(const inf_lora_adapter& in_adapter)
   
    Declaring a lora adapter to be added to the model object. Declared adapter will be added to the model in parallel when the user calls :code:`start_lora_operation`. 

    Declaration method will return :code:`INF_MODEL_SUCCESS` on successful declaration. Otherwise, one of the following flags may return:

    - :code:`INF_MODEL_ERR_LORA_EXISTS`: Attempting to add an already existing lora adapter.

    - :code:`INF_MODEL_ERR_LORA_OPERATION_ACTIVE`: Lora operation is started and is ongoing.
    
    - :code:`INF_MODEL_ERR_LORA_NAME_MISSING`: Adapter name is missing.

    - :code:`INF_MODEL_INFO_UPDATE_REQUIRED`: Lora operation started and finished and the model frame should be updated. 

.. cpp:function:: flags start_lora_operation()
    
    This method starts all declared lora operations in parallel. 
    
    Loading/Unloading a LoRA adapter can be considered as an expensive operation in our case. For that reason, it is non-blocking and its happenning in parallel. 
    
    The workflow of lora adapter loading/unloading/applying/deapplying is as follows:

    - Declaring the LoRA operation with methods such as :code:`declare_lora_*`. Multiple operations can be declared.
    
    - Starting the LoRA operation by calling the method :code:`start_lora_operation`. 
    
    This method will return :code:`INF_MODEL_SUCCESS` flag. Otherwise, the following flag will return:

    - :code:`INF_MODEL_ERR_LORA_NOTHING_TO_OPERATE`: Attempting to start a lora operation when there are no lora operation to be applied.

.. cpp:function:: flags tokenize_input(CBYTEBUFFER in_data, size_type in_size, inf_text_token_vector& out_tokens)

    Model's input tokenizer. It converts the given input into a token vector that can be executed by the inference engine.    

    This shouldn't be used in general but instead the processor's tokenizer methods should be used.

    On success, the method will populate the :code:`out_tokens` argument and return the flag :code:`INF_MODEL_SUCCESS`. Otherwise, one of the following flags may return:

    - :code:`INF_MODEL_ERR_INVALID_INPUT`: If the :code:`in_data` is null or :code:`in_size` is 0.
    - :code:`INF_MODEL_ERR_TOKENIZATION_FAILED`: Tokenizer failed to tokenize the input. This can happen if the given input contains a string that is outside the vocabulary of the model.

.. cpp:function:: virtual GENERIC on_lora_operate(const mbase::vector<inf_lora_adapter>& out_active_loras)

    A callback that is called when the lora operation is finished and the model frame is updated at the right time. 

    It has an output argument of :code:`out_active_loras` which shows the lora adapters that are active in the model object.    

.. cpp:function:: GENERIC update() override
    
    Model frame update method. Based off-of the state of the model object, a call to this function may invoke some callbacks. 

    Whenever the documentation tells user to update the model frame, it means that the user should call this method.

.. cpp:function:: GENERIC update_t() override
    
    Model object's parallel state loop. Whenever an operation is signaled such as model initialization/destruction/lora_operation, it is handled in this method.
