==================
Initializing Model
==================

.. note::
    To have an advanced understanding about the Model object, refer to :doc:`../../info-reference/model-in-detail`.

In this section, we will query the devices and initialize our model accordingly.
We will use all available devices in our system to see the maximum potential of the engine.

----------------
Querying Devices
----------------

In order to query the devices in our system, we will include :code:`mbase/inference/inf_device_desc.h`
and query the devices in our system using the function :code:`inf_query_devices` as follows:

.. code-block:: cpp

    #include <mbase/inference/inf_device_desc.h>
    #include <mbase/vector.h>
    #include <iostream>

    int main()
    {
        mbase::vector<mbase::InfDeviceDescription> deviceDesc = mbase::inf_query_devices();

        for(mbase::vector<mbase::InfDeviceDescription>::iterator It = deviceDesc.begin(); It != deviceDesc.end(); It++)
        {
            std::cout << It->get_device_description() << std::endl;
        }

        return 0;
    }

By calling :code:`inf_query_devices`, we are querying all devices. Use :code:`inf_query_gpu_devices` for gpu only, and
:code:`inf_query_cpu_devices` for cpu only.

------------------
About Model Object
------------------

Now, we will initialize our model.

There are three fundamental base objects you should derive your objects from in general.
Those are the model, processor, and the client objects.

In this part, we are concerned with the model class which is InfModelTextToText in our case.

There are three expensive operations on the model object. Those operations are:

* Loading model, through methods :code:`initialize_model_ex` or :code:`initialize_model`
* Unloading model, through method :code:`destroy`
* Loading/Unloading lora adapters, through methods :code:`declare_lora_adapter` and :code:`start_lora_operation`.

Depending on the size of the model, each operation almost takes at least a second which is unacceptable for
high-intensity and performant applications, and creating a context for processing is not much less than this. 
For that reason, neither of those methods will block your main application thread when they are invoked.

When these methods are called, the method first validate the arguments that the user supplies.
If the arguments valid and the operation can be done, the method returns a flag notifying the user that the
operation (assume initialization) is started in another thread.

During this time, while the model is loading in another thread
the user will call the :code:`update` method of the model object as frequent as his/her application loop runs.
When the model object's parallel thread signals the main thread that the operation is complete,
The :code:`on_initialize` method will be called by the time model is updated through the call :code:`update`.

.. code-block:: cpp
    :caption: pseudo while loop

    ModelObject modelObject
    while(1)
    {
        // application logic
        ...
        modelObject.update(); // Doesn't block unless the expensive operation finishes.
        ... 
        // application logic
    }

This behavior guarantees that while the model is being loaded, the main thread won't be blocked
and the :code:`on_initialize`` and many other callbacks are executed synchronously. 

--------------------------
Model Initialize in Action
--------------------------

InfModelTextToText is an interface object where you must define its virtual methods to work.
Here are the methods you must define:

- :code:`on_initialize_fail`
- :code:`on_initialize`
- :code:`on_destroy`

Now, in our main.cpp (the one we created on Creating an empty project chapter) we will include 
the class header ``mbase/inference/inf_t2t_model.h`` and derive our class:

.. code-block:: cpp
    :caption: main.cpp
    
    #include <mbase/inference/inf_device_desc.h>
    #include <mbase/inference/inf_t2t_model.h>
    #include <iostream>
    #include <mbase/vector.h>

    class ModelObject;

    class ModelObject : public mbase::InfModelTextToText {
    public:
        void on_initialize_fail(init_fail_code out_fail_code) override{}
        void on_initialize() override{}
        void on_destroy() override{}
    };

    int main()
    {
        mbase::vector<mbase::InfDeviceDescription> deviceDesc = mbase::inf_query_devices();
        for(mbase::vector<mbase::InfDeviceDescription>::iterator It = deviceDesc.begin(); It != deviceDesc.end(); It++)
        {
            std::cout << It->get_device_description() << std::endl;
        }

        ModelObject modelObject;
    }

Here is a brief on when those methods are called:

* :code:`on_initialize_fail`: If the model initialization is started in another thread but failed for any reason.
* :code:`on_initialize`: If the model initialization is finished successfully.
* :code:`on_destroy`: If the model is successfully destroyed.

In order to initialize the model object, we will call the :code:`initialize_model_ex` method with params as follows:

.. code-block:: cpp
    :caption: main.cpp

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

The first argument is the file location of our model and the last is the devices that our model will run on,
here is a brief on arguments that may require further explanation:

* :code:`totalContextLength`: Total amount of context that can be allocated from multiple processors.
* :code:`gpuLayersToUse`: Amount of layers to offload to in GPU. If no GPU is present, this is ignored.
* :code:`isMmap`: Whether to use memory mapping on model file.
* :code:`isMLock`: Prevents swapping, may increase performance.
* :code:`deviceDesc`: List of devices to use for inference.

---------------------------------
Implementing the Application Loop
---------------------------------

The :code:`initialize_model_ex` call will not block the thread it's been called unless the object goes out-of-scope before finishing the initialization operation.
If you want the initialize operation to be blocking, call :code:`initialize_model_ex_sync` method.

Since the :code:`initialize_model_ex` call is non-blocking, we will implement an infinite loop and update the :code:`ModelObject` state every frame.
Unless you update the frames of the model object, the virtual methods we wrote won't work.

Let's create a global boolean named :code:`gIsRunning` and set to true and implement our infinite loop:

.. code-block:: cpp
    :caption: main.cpp

    bool gIsRunning = true;

And

.. code-block:: cpp
    :caption: main.cpp

    while(gIsRunning)
    {
        mbase::sleep(2);
        modelObject.update();
    }

We are applying 2ms sleep to prevent CPU overuse.

Right now, if the model is being successfully loaded, it will invoke the:

* :code:`on_initialize` if the model is being properly set and loaded.
* :code:`on_initialize_fail` if the model can't be initialized due to some error which can be observed by :code:`init_fail_code out_fail_code`.

We will print "Model is initialized" on :code:`on_initialize` method and
"Model initialization failed." on :code:`on_initialize_fail` method and exit the program.

.. code-block:: cpp
    :caption: main.cpp

    void on_initialize_fail(init_fail_code out_fail_code) override
    {
        std::cout << "Model initialization failed." << std::endl;
        gIsRunning = false;
    }

    void on_initialize() override
    {
        std::cout << "Model is initialized." << std::endl;
    }

Here is the total main.cpp file:

.. code-block:: cpp
    :caption: main.cpp

    #include <mbase/inference/inf_device_desc.h>
    #include <mbase/inference/inf_t2t_model.h>
    #include <iostream>
    #include <mbase/vector.h>

    class ModelObject;

    bool gIsRunning = true;

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

Now that we have have initialized our model, we can initialize and register the processor in the next section.
