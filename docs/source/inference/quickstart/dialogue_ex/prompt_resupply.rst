=========================
Prompt Resupplying Method
=========================

In this section, we will do prompt resupplying for context preservation.

Recall the example we implemented in single-prompt example. We will use the CMakeLists and main.cpp as a base for this example.
You can see those files in :doc:`complete program <../single_prompt_ex/complete_program>`.

Now in the :code:`ClientObject`, we will define a :code:`context_line` vector named as 
:code:`chatHistory`:

.. code-block:: cpp
    :caption: main.cpp

    class ClientObject : public mbase::InfClientTextToText {
    public:
        ...
    private:
        mbase::vector<mbase::context_line> chatHistory;
    };

Then, we will write a method named :code:`start_dialogue` and we will implement
the logic of reading stdin, tokenization and execution which normally reside under :code:`on_register`.

.. code-block:: cpp
    :caption: main.cpp
    :force:

    void start_dialogue(mbase::InfProcessorBase* in_processor)
    {
        std::cout << "Enter your prompt: ";
        mbase::string clientInput = mbase::get_line();

        mbase::context_line contextLine;
        contextLine.mMessage = clientInput;
        contextLine.mRole = mbase::context_role::USER;

	chatHistory.push_back(contextLine);

        mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(in_processor);
        mbase::inf_text_token_vector tokenVector;
		
        tmpProcessorObject->tokenize_input(chatHistory.data(), chatHistory.size(), tokenVector);
        tmpProcessorObject->execute_input(tokenVector);
    }

    void on_register(mbase::InfProcessorBase* out_processor) override 
    {
        std::cout << "Client registered!" << std::endl;
        this->start_dialogue(out_processor);
    }

When you execute the input and the batch execution finishes, :code:`on_batch_processed` will be called and you will
call the :code:`next` method to compute the next token just like we did in :doc:`single-prompt <../single_prompt_ex/preparation_and_execution>`
example.

So the :code:`on_batch_processed` will be the same but we will need to store the response of the LLM for the next
prompt supply. To do this, we will declare a string named :code:`llmResponse` and append the response of the LLM into it.

And when the response finishes, we will create a :code:`context_line` with role given as :code:`ASSISTANT` 
and push it into the chat history vector

Lets declare the :code:`llmResponse`:

.. code-block:: cpp
    :caption: main.cpp

    class ClientObject : public mbase::InfClientTextToText {
    public:
        ...
    private:
        mbase::vector<mbase::context_line> chatHistory;
        mbase::string llmResponse;
    };

Now in :code:`on_write` method, append the response of the LLM into our string:

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
            llmResponse += tokenDesc.mTokenString; // added this

            mbase::decode_behavior_description dbd;
            dbd.mHaltOnWrite = false;
            dbd.mHaltDelay = 2;
            dbd.mTokenAtMost = 1;
            tmpProcessorObject->next(dbd);
        }
    }

Now in :code:`on_finish` method, create c :code:`context_line` with :code:`ASSISTANT` role and restart the conversation.

.. code-block:: cpp
    :caption: main.cpp

    void on_finish(mbase::InfProcessorTextToText* out_processor, size_type out_total_token_size, mbase::InfProcessorTextToText::finish_state out_finish_state) override
    {
		std::cout << std::endl;
        mbase::context_line tmpContext;
		tmpContext.mRole = mbase::context_role::ASSISTANT;
		tmpContext.mMessage = llmResponse;

		llmResponse.clear(); // clear the string

		chatHistory.push_back(tmpContext);
		this->start_dialogue(out_processor);
    }

Congratulations, now you can have a dialogue with the LLM using prompt resupplying method.

You may notice that the program never finish in our case so let's implement a mechanism where if the
following conditions occur, stop executing:

* If the input token length exceeds the context length of the processor.
* If the context becomes full during the generation.

--------------------
Input Token Overflow
--------------------

The :code:`execute_input` method returns a flag indicating that the execution started.
If the input token length exceeds the context length, it will return :code:`INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT`.
In that case, we will exit.

.. code-block:: cpp
    :caption: main.cpp

    void start_dialogue(mbase::InfProcessorBase* in_processor)
	{
		std::cout << "Enter your prompt: ";
        mbase::string clientInput = mbase::get_line();

        mbase::context_line contextLine;
        contextLine.mMessage = clientInput;
        contextLine.mRole = mbase::context_role::USER;

		chatHistory.push_back(contextLine);

        mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(in_processor);
        mbase::inf_text_token_vector tokenVector;
		
        tmpProcessorObject->tokenize_input(chatHistory.data(), chatHistory.size(), tokenVector);
        if(tmpProcessorObject->execute_input(tokenVector) == mbase::InfProcessorTextToText::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT)
		{
            std::cout << "Abort: Input token overflow." << std::endl;
			gIsRunning = false;
		}
	}

-------------------------
Generation Token Overflow
-------------------------

By the time :code:`on_finish` is called, there is an argument called :code:`out_finish_state`
which tells us that the generation is finished but in which state.

The states can be:

* :code:`FINISHED`: Finished gracefully.
* :code:`TOKEN_LIMIT_REACHED`: All KV cells are filled.

.. note::
    Even though there are more finish states, they are irrelevant in our case.

We will exit the program if the finish state is :code:`TOKEN_LIMIT_REACHED`:

.. code-block:: cpp
    :caption: main.cpp

    void on_finish(mbase::InfProcessorTextToText* out_processor, size_type out_total_token_size, mbase::InfProcessorTextToText::finish_state out_finish_state) override
    {
		std::cout << std::endl;

		if(out_finish_state == mbase::InfProcessorTextToText::finish_state::TOKEN_LIMIT_REACHED)
		{
			std::cout << "Abort: Generation token overflow." << std::endl;
			gIsRunning = false;
			return;
		}

        mbase::context_line tmpContext;
		tmpContext.mRole = mbase::context_role::ASSISTANT;
		tmpContext.mMessage = llmResponse;

		llmResponse.clear(); // clear the string

		chatHistory.push_back(tmpContext);
		this->start_dialogue(out_processor);
    }

This is all about prompt resupplying. For the manual caching method, go to next section.

Total main.cpp file:

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
        void start_dialogue(mbase::InfProcessorBase* in_processor)
        {
            std::cout << "Enter your prompt: ";
            mbase::string clientInput = mbase::get_line();

            mbase::context_line contextLine;
            contextLine.mMessage = clientInput;
            contextLine.mRole = mbase::context_role::USER;

            chatHistory.push_back(contextLine);

            mbase::InfProcessorTextToText* tmpProcessorObject = static_cast<mbase::InfProcessorTextToText*>(in_processor);
            mbase::inf_text_token_vector tokenVector;
            
            tmpProcessorObject->tokenize_input(chatHistory.data(), chatHistory.size(), tokenVector);
            if(tmpProcessorObject->execute_input(tokenVector) == mbase::InfProcessorTextToText::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT)
            {
                std::cout << "Abort: Input token overflow." << std::endl;
                gIsRunning = false;
            }
        }

        void on_register(mbase::InfProcessorBase* out_processor) override
        {
            std::cout << "Client registered!" << std::endl;
            this->start_dialogue(out_processor);
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
                llmResponse += tokenDesc.mTokenString;

                mbase::decode_behavior_description dbd;
                dbd.mHaltOnWrite = false;
                dbd.mHaltDelay = 2;
                dbd.mTokenAtMost = 1;
                tmpProcessorObject->next(dbd);
            }
        }

        void on_finish(mbase::InfProcessorTextToText* out_processor, size_type out_total_token_size, mbase::InfProcessorTextToText::finish_state out_finish_state) override
        {
            std::cout << std::endl;

            if(out_finish_state == mbase::InfProcessorTextToText::finish_state::TOKEN_LIMIT_REACHED)
            {
                std::cout << "Abort: Generation token overflow." << std::endl;
                gIsRunning = false;
                return;
            }

            mbase::context_line tmpContext;
            tmpContext.mRole = mbase::context_role::ASSISTANT;
            tmpContext.mMessage = llmResponse;

            llmResponse.clear(); // clear the string

            chatHistory.push_back(tmpContext);
            this->start_dialogue(out_processor);
        }
    private:
        mbase::vector<mbase::context_line> chatHistory;
        mbase::string llmResponse;
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
            uint32_t batchSize = 2048;
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
