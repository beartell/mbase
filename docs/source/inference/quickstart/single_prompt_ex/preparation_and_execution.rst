===============================
Input Preperation and Execution
===============================

.. note::

    For deeper understanding, refer to :doc:`../../info-reference/message-preparation`


Now that we have registered our client to the processor, it is time to read an input (stdin) from terminal and make the inference engine work.

Here are the steps should be taken to execute our input:

* Read the input from terminal.
* Create context line and specify the role of the input.
* Tokenize the input.
* Execute the input.
* Compute and generate tokens.

-------------
Reading Input
-------------

In :code:`ClientObject`'s :code:`on_register` method, let's read a string from the terminal:

.. code-block:: cpp
    :caption: main.cpp

    void on_register(mbase::InfProcessorBase* out_processor) override 
    {
        std::cout << "Client registered!" << std::endl;
        std::cout << "Enter your prompt: ";
        mbase::string clientInput = mbase::get_line();
    }

This will wait for you to enter your prompt. Now, we will create a context line.

-----------------------
Setting up Context Line
-----------------------

Context line is a helper prompt descriptor for tokenizer to tokenize the given prompt for LLM to understand.

Now, lets create a context line:

.. code-block:: cpp
    :caption: main.cpp

    void on_register(mbase::InfProcessorBase* out_processor) override 
    {
        std::cout << "Client registered!" << std::endl;
        std::cout << "Enter your prompt: " << std::endl;
        mbase::string clientInput = mbase::get_line();
        
        mbase::context_line contextLine;
        contextLine.mMessage = clientInput;
        contextLine.mRole = mbase::context_role::USER;
    }

------------------------
Tokenizing Context Lines
------------------------

Now, we will tokenize our context line and generate a token vector from that.
Only the token vectors can be supplied to the inference engine to work.

Here is how we tokenize and generate the token vector:

.. code-block:: cpp
    :caption: main.cpp

    void on_register(mbase::InfProcessorBase* out_processor) override 
    {
        std::cout << "Client registered!" << std::endl;
        std::cout << "Enter your prompt: " << std::endl;
        mbase::string clientInput = mbase::get_line();
        
        mbase::context_line contextLine;
        contextLine.mMessage = clientInput;
        contextLine.mRole = mbase::context_role::USER;

        mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(out_processor);
        mbase::inf_text_token_vector tokenVector;

        tmpProcessorObject->tokenize_input(&contextLine, 1, tokenVector);
    }

The reason :code:`tokenize_input` takes :code:`context_line` array is that the 
order of context lines supplied to the tokenizer is the order of communication in the context.

----------------
Executing Tokens
----------------

Now that we have generated our tokens, we will execute the token vector using processor's :code:`execute_input` method.

.. code-block:: cpp
    :caption: main.cpp

    void on_register(mbase::InfProcessorBase* out_processor) override 
    {
        std::cout << "Client registered!" << std::endl;
        std::cout << "Enter your prompt: " << std::endl;
        mbase::string clientInput = mbase::get_line();
        
        mbase::context_line contextLine;
        contextLine.mMessage = clientInput;
        contextLine.mRole = mbase::context_role::USER;

        mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(out_processor);
        mbase::inf_text_token_vector tokenVector;

        tmpProcessorObject->tokenize_input(&contextLine, 1, tokenVector);
        tmpProcessorObject->execute_input(tokenVector); // non-blocking call
        std::cout << "Batch processing started!" << std::endl;
    }

The call to :code:`execute_input` will not block your main thread but instead it will send a signal 
to the processor's parallel update loop. This way, the expensive computation will not block your thread.

After the batch processing is finished, :code:`on_batch_processed` method will be called indicating that the processor
is ready to compute the upcoming tokens. In our batch processing method, write the following in order to observe how long it takes
to process the input.

.. code-block:: cpp
    :caption: main.cpp

    void on_batch_processed(mbase::InfProcessorTextToText* out_processor, const uint32_t& out_proc_batch_length, const bool& out_is_kv_locked) override
    {
        std::cout << "Batch processed!\n" << std::endl;
    }

-----------------------
Generating the Response
-----------------------

Now the exciting part, we will start generating our tokens.
In :code:`on_batch_processed` method, we will call the processor's :code:`next` method to compute the next token.

In order to call the next, a decode behavior must be specified which only tells processor 
how many tokens to compute and at what state it should be after generating those tokens.
For detailed information, refer to: :doc:`../../info-reference/processor-in-detail`

.. code-block:: cpp
    :caption: main.cpp

    void on_batch_processed(mbase::InfProcessorTextToText* out_processor, const uint32_t& out_proc_batch_length, const bool& out_is_kv_locked) override
    {
        std::cout << "Batch processed!\n" << std::endl;
        ProcessorObject* tmpProcessorObject = static_cast<ProcessorObject*>(out_processor);

        mbase::decode_behavior_description dbd;
        dbd.mHaltOnWrite = false;
        dbd.mHaltDelay = 2;
        dbd.mTokenAtMost = 1;

        tmpProcessorObject->next(dbd); // non-blocking call
    }

Here is a brief of those properties:

* :code:`mHaltOnWrite`: Processor object is running in its own thread. If this set true, every time a token is computed, it destroys the processor thread. Is this set false, processor thread will keep waiting for the :code:`next` call.

* :code:`mHaltDelay`: If the halt on write is false and since the processor waiting idly, this is the idle delay put on processor's thread (calling :code:`mbase::sleep(mHaltdDelay)`) on processor thread.

* :code:`mTokenAtMost`: How many tokens to be computed on the next call.

When the computation is finished, :code:`on_write` method will be called for every time a number of tokens being computed 
which is specified by the :code:`decode_behavior_description` :code:`mTokenAtMost` which is 1 in our case.

Inside the :code:`on_write` method, we will convert the generated token into token description object and display the result of the LLM. 
We also should check if the computation is finished which can be observed by checking :code:`out_is_finish` (true if finished, false if not).

.. code-block:: cpp
    :caption: main.cpp

    void on_write(mbase::InfProcessorTextToText* out_processor, const mbase::inf_text_token_vector& out_token, bool out_is_finish) override 
    {
        if(!out_is_finish)
        {
            mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(out_processor);
            mbase::inf_token_description tokenDesc;
            tmpProcessorObject->token_to_description(out_token[0], tokenDesc);

            fflush(stdout);
            std::cout << tokenDesc.mTokenString;
        }
    }

The code above will display the first token that is generated by LLM.
In order to compute the next token, we will again call the :code:`next` method with :code:`decode_behavior_description` just like before.
Here is how we do it:

.. code-block:: cpp
    :caption: main.cpp

    void on_write(mbase::InfProcessorTextToText* out_processor, const mbase::inf_text_token_vector& out_token, bool out_is_finish) override 
    {
        if(!out_is_finish)
        {
            mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(out_processor);
            mbase::inf_token_description tokenDesc;
            tmpProcessorObject->token_to_description(out_token[0], tokenDesc);

            fflush(stdout);
            std::cout << tokenDesc.mTokenString;

            mbase::decode_behavior_description dbd;
            dbd.mHaltOnWrite = false;
            dbd.mHaltDelay = 2;
            dbd.mTokenAtMost = 1;
            tmpProcessorObject->next(dbd);
        }
    }

Keep in mind that when you call :code:`next`, the system does not stream the next token from the already existing string
but instead the next token is being computed when the call has been made.

-------
The End
-------

Now that we can see the response of the LLM, we will close the program after we receive the response.

As we have said before, if the :code:`out_is_finish` is true, it indicates that the generated tokens are the last
tokens that are generated. In this case, if the :code:`out_is_finish` is true, inference engine will call the :code:`on_finish` after the :code:`on_write` call.

Based off of the information above, we can set :code:`gIsRunning` to false to finish the execution.

.. code-block:: cpp
    :caption: main.cpp

    void on_finish(mbase::InfProcessorTextToText* out_processor, size_type out_total_token_size, mbase::InfProcessorTextToText::finish_state out_finish_state) override 
    {
        gIsRunning = false;
    }

Now congratulations! You have used the inference engine to get a response from the LLM.

This example will be used as a base for :doc:`dialogue example <../dialogue_ex/about>` in which we will implement
in the next chapter.

.. tip::
    If things seem complicated or some parts of the program or explanations are unclear,
    make sure to refer to :doc:`../../info-reference/index` which you will find detailed information
    for the things that are present here.

Here is the total main.cpp file:

.. code-block:: cpp
    :caption: main.cpp

    #include <mbase/inference/inf_device_desc.h>
    #include <mbase/inference/inf_t2t_model.h>
    #include <mbase/inference/inf_t2t_processor.h>
    #include <mbase/inference/inf_t2t_client.h>
    #include <iostream>
    #include <mbase/vector.h>

    bool gIsRunning = true;

    class ModelObject;
    class ProcessorObject;
    class ClientObject;

    class ClientObject : public mbase::InfClientTextToText {
    public:
        void on_register(mbase::InfProcessorBase* out_processor) override 
        {
            std::cout << "Client registered!" << std::endl;
            std::cout << "Enter your prompt: ";
            mbase::string clientInput = mbase::get_line();
            
            mbase::context_line contextLine;
            contextLine.mMessage = clientInput;
            contextLine.mRole = mbase::context_role::USER;

            mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(out_processor);
            mbase::inf_text_token_vector tokenVector;

            tmpProcessorObject->tokenize_input(&contextLine, 1, tokenVector);
            tmpProcessorObject->execute_input(tokenVector);
        }

        void on_unregister(mbase::InfProcessorBase* out_processor) override {}

        void on_batch_processed(mbase::InfProcessorTextToText* out_processor, const uint32_t& out_proc_batch_length, const bool& out_is_kv_locked) override
        {
            std::cout << "Batch processed!\n" << std::endl;
            mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(out_processor);

            mbase::decode_behavior_description dbd;
            dbd.mHaltOnWrite = false;
            dbd.mHaltDelay = 2;
            dbd.mTokenAtMost = 1;

            tmpProcessorObject->next(dbd); // non-blocking call
        }

        void on_write(mbase::InfProcessorTextToText* out_processor, const mbase::inf_text_token_vector& out_token, bool out_is_finish) override 
        {
            if(!out_is_finish)
            {
                mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(out_processor);
                mbase::inf_token_description tokenDesc;
                tmpProcessorObject->token_to_description(out_token[0], tokenDesc);

                fflush(stdout);
                std::cout << tokenDesc.mTokenString;

                mbase::decode_behavior_description dbd;
                dbd.mHaltOnWrite = false;
                dbd.mHaltDelay = 2;
                dbd.mTokenAtMost = 1;
                tmpProcessorObject->next(dbd);
            }
        }

        void on_finish(mbase::InfProcessorTextToText* out_processor, size_type out_total_token_size, mbase::InfProcessorTextToText::finish_state out_finish_state) override 
        {
            gIsRunning = false;
        }
    };

    class ProcessorObject : public mbase::InfProcessorTextToText {
    public:
        ProcessorObject(){}
        ~ProcessorObject()
        {
            this->release_inference_client_stacked();
        }

        void on_initialize_fail(last_fail_code out_code) override
        {
            std::cout << "Processor initialization failed." << std::endl;
            gIsRunning = false;
        }

        void on_initialize() override
        {
            std::cout << "Processor is initialized." << std::endl;
            this->set_inference_client(&clientObject); // 100% success
        }

        void on_destroy() override{}
    private:
        ClientObject clientObject;
    };

    class ModelObject : public mbase::InfModelTextToText {
    public:
        void on_initialize_fail(init_fail_code out_fail_code) override
        {
            std::cout << "Model initialization failed." << std::endl;
            gIsRunning = false;
        }

        void on_initialize() override
        {
            std::cout << "Model is initialized." << std::endl;

            uint32_t contextSize = 4096;
            uint32_t batchSize = 1024;
            uint32_t procThreadCount = 16;
            uint32_t genThreadCount = 8;
            bool isFlashAttention = true;
            mbase::inf_sampling_set samplingSet; // We are setting greedy sampler by supplying empty sampling set

            ModelObject::flags registerationStatus = this->register_context_process(
                &processorObject,
                contextSize,
                batchSize,
                genThreadCount,
                procThreadCount,
                isFlashAttention,
                samplingSet
            );

            if(registerationStatus != ModelObject::flags::INF_MODEL_INFO_REGISTERING_PROCESSOR)
            {
                std::cout << "Registration unable to proceed." << std::endl;
                gIsRunning = false;
            }
        }
        void on_destroy() override{}
    private:
        ProcessorObject processorObject;
    };

    int main()
    {
        mbase::vector<mbase::InfDeviceDescription> deviceDesc = mbase::inf_query_devices();
        for(mbase::vector<mbase::InfDeviceDescription>::iterator It = deviceDesc.begin(); It != deviceDesc.end(); It++)
        {
            std::cout << It->get_device_description() << std::endl;
        }
        
        ModelObject modelObject;

        uint32_t totalContextLength = 32000;
        int32_t gpuLayersToUse = 80;
        bool isMmap = true;
        bool isMLock = true;

        if (modelObject.initialize_model_ex(
            L"<path_to_your_model>",
            totalContextLength,
            gpuLayersToUse,
            isMmap,
            isMLock,
            deviceDesc
        ) != ModelObject::flags::INF_MODEL_INFO_INITIALIZING_MODEL)
        {
            std::cout << "Unable to start initializing the model." << std::endl;
            return 1;
        }

        while(gIsRunning)
        {
            modelObject.update();
            mbase::sleep(2);
        }

        return 0;
    }