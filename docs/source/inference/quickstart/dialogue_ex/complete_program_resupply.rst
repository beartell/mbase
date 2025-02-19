================
Complete Program
================

-------------------
CMakeLists.txt File
-------------------

Here is the complete CMakeLists.txt file:

.. code-block:: cmake
    :caption: CMakeLists.txt

    cmake_minimum_required(VERSION 3.15...3.31)
    project("mbase_simple_project" LANGUAGES CXX)

    add_executable(simple_project main.cpp)

    find_package(mbase.libs REQUIRED COMPONENTS std inference)

    target_compile_features(simple_project PUBLIC cxx_std_17)
    target_link_libraries(simple_project PRIVATE mbase-std mbase-inference)
    target_include_directories(simple_project PUBLIC mbase-std mbase-inference)

-------------
main.cpp File
-------------

Here is the complete main.cpp file:

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
