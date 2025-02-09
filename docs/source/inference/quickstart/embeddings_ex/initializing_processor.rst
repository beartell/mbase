======================
Initializing Processor
======================

After you have initialized the model, we will need to initialize our processor object and register into the model.
Processor is an essential concept in MBASE Inference and it is the object where most of the computation is happening.

.. note::
    For detailed information about the processor, refer to :doc:`../../info-reference/processor-in-detail`

----------------------
About Processor Object
----------------------

Processor object is an object who is responsible for doing the inference in general.
Since a processor will process the model, a model object must first be initialized.
Then, processor can either be registered into the model object or can process it without registering itself.

Before we proceed, lets identify the expensive operations of the processor object.
Those operations are:

* Creating context, through methods :code:`register_context_process` or :code:`initialize`
* Destroying context, through method :code:`destroy`
* Tokenization through the method :code:`tokenize_input`
* Batch procesing or prompt processing, through method :code:`execute_input`
* Token generation, through method :code:`next`

Depending on the size of the model, size of the context, and input, each operation takes considerable amount of time.
Fortunately, all of those methods are non-blocking.

Just like we have said before in model initialization chapter, when those methods are called,
the method first validate the arguments, and if the arguments are valid and the operation can start,
it returns a flag notifying the user that the operation (assume creating context) is started in another thread.

During this time, while the processor is doing expensive operations in another thread,
the user will call the :code:`update` method of the model or processor object as frequent as the loop runs.

If the processor is being registered into the model, processor's :code:`update` method will be called when 
model's :code:`update` method is called.

You can register as many processors as you want as long as your system is great enough to do a lot of parallel processing.

------------------------------
Processor Initialize in Action
------------------------------

We have a base InfProcessor class where InfEmbedderProcessor derives from.
In the near future, all inference types such as S2T, T2I will be implemented through that.

In our main.cpp, we will include the class header :code:`#include <mbase/inference/inf_embedder.h>` and derive our class named as 
:code:`ProcessorObject`:

.. code-block:: cpp
    :caption: main.cpp

    #include <mbase/inference/inf_device_desc.h>
    #include <mbase/inference/inf_t2t_model.h>
    #include <mbase/inference/inf_embedder.h>
    #include <iostream>
    #include <mbase/vector.h>

    bool gIsRunning = true;

    class ProcessorObject;
    class ModelObject;

    class ProcessorObject : public mbase::InfEmbedderProcessor {
    public:
        void on_initialize_fail(last_fail_code out_code) override
        {
            std::cout << "Processor initialization failed." << std::endl;
            gIsRunning = false;
        }

        void on_initialize() override
        {
            std::cout << "Processor is initialized." << std::endl;
        }

        void on_destroy() override
        {

        }
    private:
    };

Then we will register our processor into the model when the model is initialized by the program.

First, we will set :code:`ProcessorObject` as a member of the :code:`ModelObject` class as follows:

.. code-block:: cpp
    :caption: main.cpp

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
        }
        void on_destroy() override{}
    private:
        ProcessorObject processorObject; // Hi!!
    };

.. tip::
    Even if it is not necessary for :code:`processorObject` to be a member of :code:`ModelObject` to be registered, 
    we are implementing this way for easier management.

Now, we will call the register_context_process method of the :code:`ModelObject` 
in :code:`on_initialize` method to register our processor as follows:

.. code-block:: cpp
    :caption: main.cpp

    class ModelObject : public mbase::InfModelTextToText {
        ...
        void on_initialize() override
        {
            std::cout << "Model is initialized." << std::endl;

            uint32_t contextSize = 0;
            uint32_t procThreadCount = 16;
            this->get_max_embedding_context(contextSize);
            ModelObject::flags registerationStatus = this->register_context_process(
                &processorObject,
                contextSize,
                procThreadCount
            );

            if(registerationStatus != ModelObject::flags::INF_MODEL_INFO_REGISTERING_PROCESSOR)
            {
                std::cout << "Registration unable to proceed." << std::endl;
                gIsRunning = false;
            }
        }
        ...
    };

We should register the context with the max embedding context length, otherwise the input that is greater than
the context train size, will make the inference engine crash.

After the registeration is successful, "Processor is initialized." will be displayed
on the terminal.

Here is the total main.cpp file:

.. code-block:: cpp
    :caption: main.cpp

    #include <mbase/inference/inf_device_desc.h>
    #include <mbase/inference/inf_t2t_model.h>
    #include <mbase/inference/inf_embedder.h>
    #include <iostream>
    #include <mbase/vector.h>

    bool gIsRunning = true;

    class ModelObject;
    class ProcessorObject;

    class ProcessorObject : public mbase::InfEmbedderProcessor {
    public:
        void on_initialize_fail(last_fail_code out_code) override
        {
            std::cout << "Processor initialization failed." << std::endl;
            gIsRunning = false;
        }

        void on_initialize() override
        {
            std::cout << "Processor is initialized." << std::endl;
        }

        void on_destroy() override
        {

        }
    private:
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

            uint32_t contextSize = 0;
            uint32_t procThreadCount = 16;
            this->get_max_embedding_context(contextSize);
            ModelObject::flags registerationStatus = this->register_context_process(
                &processorObject,
                contextSize,
                procThreadCount
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