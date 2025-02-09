===================
Initializing Client
===================

After we have initialized and registered our processor, it is now time to initialize the client and come one step closer to do an LLM inference.
Processors can do inference on their own but without assigning a client we will 
be unable to examine the result that the processor generates.

Inference client is an interface name :code:`InfClientBase` and there is an object derived from that class
named :code:`InfClientTextToText`. You must inherit from the :code:`InfClientTextToText` and define its virtual methods to work:

* :code:`on_register`: To get notified when the client is registered.
* :code:`on_unregister`: To get notified when the client is unregistered.
* :code:`on_batch_processed`: Invoked when the processor object finished processing the initial prompt.
* :code:`on_write`: Invoked when a new token is generated.
* :code:`on_finish`: Invoked after :code:`on_write` when a new token is generated and if that token is the last token.

In our main.cpp, we will include the client class header :code:`#include <mbase/inference/inf_t2t_client.h>`
and derive our class named as :code:`ClientObject`:

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
        }

        void on_unregister(mbase::InfProcessorBase* out_processor) override {}

        void on_batch_processed(mbase::InfProcessorTextToText* out_processor, const uint32_t& out_proc_batch_length, const bool& out_is_kv_locked) override{}

        void on_write(mbase::InfProcessorTextToText* out_processor, const mbase::inf_text_token_vector& out_token, bool out_is_finish) override {}

        void on_finish(mbase::InfProcessorTextToText* out_processor, size_type out_total_token_size, mbase::InfProcessorTextToText::finish_state out_finish_state) override {}
    };

:code:`ClientObject` registration is much like processor registration but much simpler.
Attempting to register a client to a processor that is not being initialized or registered is a valid operation,
it may make sense for some special scenarios.

Now, we will set :code:`ClientObject` as a member of the :code:`ProcessorObject` object class and register as follows:

.. code-block:: cpp
    :caption: main.cpp

    class ProcessorObject : public mbase::InfProcessorTextToText {
    public:
        void on_initialize_fail(last_fail_code out_code) override
        {
            std::cout << "Processor initialization failed." << std::endl;
            gIsRunning = false;
        }

        void on_initialize() override
        {
            std::cout << "Processor is initialized." << std::endl;
            this->set_inference_client(&clientObject); // registering the client
        }

        void on_destroy() override
        {

        }
    private:
        ClientObject clientObject;
    };

And, we will unregister the client in the destructor:

.. code-block:: cpp
    :caption: main.cpp

    class ProcessorObject : public mbase::InfProcessorTextToText {
    public:
        ProcessorObject(){}
        ~ProcessorObject()
        {
            this->release_inference_client_stacked(); // won't call on_unregister
        }
        ...
    private:
        ClientObject clientObject;
    };

We now have successfully registered our client into the processor.

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
        }

        void on_unregister(mbase::InfProcessorBase* out_processor) override {}

        void on_batch_processed(mbase::InfProcessorTextToText* out_processor, const uint32_t& out_proc_batch_length, const bool& out_is_kv_locked) override{}

        void on_write(mbase::InfProcessorTextToText* out_processor, const mbase::inf_text_token_vector& out_token, bool out_is_finish) override {}

        void on_finish(mbase::InfProcessorTextToText* out_processor, size_type out_total_token_size, mbase::InfProcessorTextToText::finish_state out_finish_state) override {}
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
        void on_destroy() override
        {

        }
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

Now, it is time to read an input and make the inference engine work in the next section.