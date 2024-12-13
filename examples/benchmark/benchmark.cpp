#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_client.h>
#include <mbase/argument_get_value.h>
#include <mbase/filesystem.h>
#include <math.h>
#include <chrono>
#include <signal.h>

#define MBASE_BENCHMARK_VERSION "v1.1.0"

using namespace mbase;

class BenchmarkModel;
class BenchmarkProcessor;
class BenchmarkClient;

struct program_parameters {
    mbase::string mModelFile;
    I32 mThreadCount = 16;
    I32 mBatchThreadCount = 8;
    I32 mContextLength = 768;
    I32 mBatchLength = 512;
    I32 mGpuLayer = 999;
    I32 mPredictCount = 256;
    I32 mUserCount = 1;
};

mbase::vector<InfDeviceDescription> deviceDescription;
program_parameters gSampleParams;
bool gIsProgramRunning = true;
I32 gFinishedProcessors = 0;

GENERIC catching_interrupt_signal(I32 out_sig_id)
{
    printf("Program interrupted\n");
    gIsProgramRunning = false;
}

GENERIC print_usage()
{
    printf("========================================\n");
    printf("#Program name:      mbase-benchmark-t2t\n");
    printf("#Version:           %s\n", MBASE_BENCHMARK_VERSION);
    printf("#Type:              Utility, Example\n");
    printf("#Further docs: \n");
    printf("***** DESCRIPTION *****\n");
    printf("This is a utility program to measure the performance of the given T2T LLM.\n");
    printf("The program will do an inference based-off of the given context size, batch length, and n predict, simultaneusly on multiple users at the same time.\n");
    printf("At the end of the inference, it will display the following metrics:\n");
    printf("- The time it took to initialize the context in milliseconds.\n");
    printf("- Prompt processing tokens per second(pp t/s).\n");
    printf("- Token generation tokens per second(tg t/s).\n");
    printf("### NOTE ###\n");
    printf("If the context kv cache is filled and there still are tokens to predict, they will not be processed, since we are not doing context window shifting.\n");
    printf("========================================\n\n");
    printf("Usage: mbase-benchmark-t2t <model_path> *[<option> [<value>]]\n");
    printf("Options: \n\n");
    printf("-h, --help                        Print usage.\n");
    printf("-v, --version                     Shows program version.\n");
    printf("-t, --thread-count <int>          Amount of threads to use for output processing (default=16).\n");
    printf("-bt, --batch-thread-count <int>   Amount of threads to use for initial batch processing (default=8).\n");
    printf("-c, --context-length <int>        Total context length (default=2048).\n");
    printf("-b, --batch-length <int>          Batch length (default=512).\n");
    printf("-gl, --gpu-layers <int>           GPU layers to offload to (default=999).\n");
    printf("-np, --n-predict <int>            Number of tokens to predict (default=256).\n");
    printf("-uc, --user-count <int>           Number of users that will be processed in parallel. (default=1)\n");
    printf("-jout, --json-output-path <str>   If the json output path is specified, result will be written there in file(mbase_bench.json) (default='').\n\n");
}

class BenchmarkModel : public InfModelTextToText {
public:
    GENERIC on_initialize_fail(init_fail_code out_fail_code) override{}
    GENERIC on_initialize() override{}
    GENERIC on_destroy() override{}
private:
};

class BenchmarkProcessor : public InfProcessorTextToText {
public:
    BenchmarkProcessor() : mNPredict(gSampleParams.mPredictCount) {}
    GENERIC on_initialize() override{}
    GENERIC on_initialize_fail(last_fail_code out_code) override
    {
        fflush(stdout);
        printf("ERR: Context initialization failed.\n");
        printf("ERR: Insufficent memory.\n");
        exit(1);
    }
    GENERIC on_destroy() override{}
    GENERIC decrement_predict() {--mNPredict;}
    I32 get_predict_count(){ return mNPredict; }
private:
    I32 mNPredict = 0;
};

class BenchmarkClient : public InfClientTextToText {
public:
    GENERIC on_register(InfProcessorBase* out_processor) override
    {
    }

    GENERIC on_unregister(InfProcessorBase* out_processor) override
    {
    }
    
    GENERIC on_batch_processed(InfProcessorTextToText* out_processor, const U32& out_proc_batch_length) override
    {
        BenchmarkProcessor* hostProcessor = static_cast<BenchmarkProcessor*>(out_processor);
        mbase::decode_behavior_description dbd;
        dbd.mTokenAtMost = 1;
        dbd.mHaltOnWrite = false;
        hostProcessor->next(dbd);
    }

    GENERIC on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) override
    {
        BenchmarkProcessor* hostProcessor = static_cast<BenchmarkProcessor*>(out_processor);
        if(out_is_finish)
        {
            return;
        }
        else
        {
            if(!hostProcessor->get_predict_count())
            {
                hostProcessor->stop_processor();
                gFinishedProcessors++;
                if(gFinishedProcessors == gSampleParams.mUserCount)
                {
                    gIsProgramRunning = false;
                }
            }
        }

        hostProcessor->decrement_predict();
        mbase::inf_token_description tokenDesc;
        mbase::decode_behavior_description dbd;
        dbd.mTokenAtMost = 1;
        dbd.mHaltOnWrite = false;
        hostProcessor->next(dbd);
    }

    GENERIC on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state) override
    {
        out_processor->release_inference_client_stacked();
        gFinishedProcessors++;
        if(gFinishedProcessors == gSampleParams.mUserCount)
        {
            gIsProgramRunning = false;
        }
    }
private:
};

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("ERR: Model file is not supplied\n");
        print_usage();
        return 1;
    }

    mbase::argument_get<mbase::string>::value(0, argc, argv, gSampleParams.mModelFile);

    if(gSampleParams.mModelFile == "-h" || gSampleParams.mModelFile == "--help")
    {
        print_usage();
        return 1;
    }
    
    if(!mbase::is_file_valid(mbase::from_utf8(gSampleParams.mModelFile)))
    {
        printf("ERR: Can't open model file: %s\n", gSampleParams.mModelFile.c_str());
        return 1;
    }

    for(I32 i = 2; i < argc; i++)
    {
        mbase::string argumentString = argv[i];
        if(argumentString == "--help" || argumentString == "-h")
        {
            print_usage();
            return 1;
        }

        else if(argumentString == "-v" || argumentString == "--version")
        {
            printf("MBASE Benchmark %s\n", MBASE_BENCHMARK_VERSION);
            return 0;
        }

        else if(argumentString == "--thread-count" || argumentString == "-t")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mThreadCount);
        }

        else if(argumentString == "--batch-thread-count" || argumentString == "-bt")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mBatchThreadCount);
        }

        else if(argumentString == "--context-length" || argumentString == "-c")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mContextLength);
        }

        else if(argumentString == "--batch-length" || argumentString == "-b")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mBatchLength);
        }

        else if(argumentString == "--gpu-layers" || argumentString == "-gl")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mGpuLayer);
        }

        else if(argumentString == "-np" || argumentString == "--n-predict")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mPredictCount);
        }

        else if(argumentString == "--user-count" || argumentString == "-uc")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mUserCount);
        }
    }
    
    if(gSampleParams.mThreadCount <= 0)
    {
        printf("ERR: Thread must be at least 1 but given: %d\n", gSampleParams.mThreadCount);
        return 1;   
    }

    if(gSampleParams.mBatchThreadCount <= 0)
    {
        printf("ERR: Batch thread must be at least 1 but given: %d\n", gSampleParams.mBatchThreadCount);
        return 1;
    }

    deviceDescription = inf_query_devices();

    BenchmarkModel benchModel;
    BenchmarkClient benchClient;
    mbase::vector<BenchmarkProcessor*> processorsList; 
    for(I32 i = 0; i < gSampleParams.mUserCount; i++)
    {
        processorsList.push_back(new BenchmarkProcessor); // os will release the resource on program termination anyways.
    }

    benchModel.initialize_model_ex_sync(mbase::from_utf8(gSampleParams.mModelFile), 1200000, gSampleParams.mGpuLayer, true, true, deviceDescription);
    
    if(!benchModel.is_initialized())
    {
        printf("ERR: Unable to initialize model\n");
        return 1;
    }

    I32 i = 0;

    for(BenchmarkProcessor* tmpProc : processorsList)
    {
        benchModel.register_context_process(
            tmpProc,
            gSampleParams.mContextLength,
            gSampleParams.mBatchLength,
            gSampleParams.mThreadCount,
            gSampleParams.mBatchThreadCount,
            true,
            {}
        );

        while(!tmpProc->is_registered())
        {
            tmpProc->update();
            mbase::sleep(2);
        }
        tmpProc->set_inference_client(&benchClient);
        ++i;
    }

    signal(SIGINT, catching_interrupt_signal);
    llama_model* rawModel = benchModel.get_raw_model();
    mbase::string modelName;
    F32 tmpModelSize = (llama_model_size(rawModel) / (F32)(1024*1024*1024));
    benchModel.get_model_name(modelName);
    printf("==== Session Information ====\n\n");
    printf("- Model Information: \n");
    printf("\tName: %s\n", modelName.c_str());
    printf("\tModel size: %.2f %s", tmpModelSize, "GB\n");
    printf("\tEmbedding length: %d\n", llama_n_embd(rawModel));
    printf("\tHead count: %d\n", llama_n_head(rawModel));
    printf("\tLayer count: %d\n", llama_n_layer(rawModel));
    printf("- Context length: %u\n", gSampleParams.mContextLength);
    printf("- Batch size: %u\n", gSampleParams.mBatchLength);
    printf("- Batch processing threads: %u\n", gSampleParams.mBatchThreadCount);
    printf("- Generation threads: %u\n", gSampleParams.mThreadCount);
    printf("- User count: %u\n", gSampleParams.mUserCount);
    printf("- N Predict: %u\n", gSampleParams.mPredictCount);
    printf("- Compute devices: \n");

    for(InfDeviceDescription& tmpDescription : deviceDescription)
    {
        mbase::string typeString;
        switch (tmpDescription.get_device_type())
        {
        case InfDeviceDescription::device_type::CPU:
            typeString = "CPU";
            break;
        case InfDeviceDescription::device_type::GPU:
            typeString = "GPU";
            break;
        case InfDeviceDescription::device_type::CUSTOM:
            typeString = "CUSTOM";
            break;
        case InfDeviceDescription::device_type::UNKNOWN:
            typeString = "UNKNOWN";
            break;
        }
        printf("\t%s ## Type: %s\n", tmpDescription.get_device_description().c_str(), typeString.c_str());
    }
    for(BenchmarkProcessor* tmpProc : processorsList)
    {
        mbase::inf_text_token_vector tokVec(gSampleParams.mBatchLength, 1);
        tmpProc->execute_input(tokVec);
    }

    U64 frameCounter = 0;
    U64 totalFps = 0;
    U64 diagnosticFps = 0;
    U64 nonResetFrameCounter = 0;

    std::chrono::high_resolution_clock::time_point programBeginTime = std::chrono::high_resolution_clock::now();
    while(gIsProgramRunning)
    {
        std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();
        benchModel.update();
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        
        U64 deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
        if(deltaTime < 2)
        {
            beginTime = std::chrono::high_resolution_clock::now();
            mbase::sleep(2);
            endTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
        }
        frameCounter++;
        totalFps += 1 / (deltaTime / 1000.0f);
        if(frameCounter == 500)
        {
            diagnosticFps += totalFps / frameCounter;
            printf("\rAverage FPS per 500 frames: %lu", totalFps / frameCounter);
            fflush(stdout);
            frameCounter = 0;
            totalFps = 0;
            nonResetFrameCounter++;
        }
    }
    std::chrono::high_resolution_clock::time_point programEndTime = std::chrono::high_resolution_clock::now();
    printf("\n");
    printf("\n==== Useful metrics ====\n");
    printf("- Total elapsed time in seconds: %.2f\n", std::chrono::duration_cast<std::chrono::milliseconds>(programEndTime - programBeginTime).count() / 1000.0f);
    printf("- Average FPS: %.1f\n", diagnosticFps / (F32)nonResetFrameCounter);
    printf("\n==== Processor diagnostics ====\n");
    printf("| Load delay ms| pp t/s | tg t/s |\n");
    
    for(BenchmarkProcessor* tmpProc : processorsList)
    {
        InfProcT2TDiagnostics& t2tDiag = tmpProc->get_diagnostics();
        printf("%lu\t\t%.3f\t\t%.3f\n", t2tDiag.loadTimeInMilliseconds, t2tDiag.ppTokensPerSecond, t2tDiag.evalTokensPerSecond);
        tmpProc->release_inference_client_stacked();
    }


    return 0;
}