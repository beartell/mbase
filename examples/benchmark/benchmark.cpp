#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_client.h>
#include <mbase/argument_get_value.h>
#include <mbase/filesystem.h>
#include <mbase/json/json.h>
#include <math.h>
#include <chrono>
#include <signal.h>

#define MBASE_BENCHMARK_VERSION "v1.4.0"

using namespace mbase;

class BenchmarkModel;
class BenchmarkProcessor;
class BenchmarkClient;

struct program_parameters {
    mbase::string mModelFile;
    mbase::string mJsonOut;
    mbase::string mMdOut;
    I32 mThreadCount = 16;
    I32 mBatchThreadCount = 8;
    I32 mContextLength = 2048;
    I32 mBatchLength = 256;
    I32 mGpuLayer = 999;
    I32 mPredictCount = 256;
    I32 mPromptLength = 1024;
    I32 mFps = 500;
    I32 mUserCount = 1;
    bool mFlashAttention = true;
};

mbase::vector<InfDeviceDescription> deviceDescription;
program_parameters gSampleParams;
bool gIsProgramRunning = true;
I32 gFinishedProcessors = 0;

GENERIC catching_interrupt_signal(I32 out_sig_id);
GENERIC print_usage();

GENERIC catching_interrupt_signal([[maybe_unused]] I32 out_sig_id)
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
    printf("At the end of the inference, it will display the following metrics along with the model and session information:\n");
    printf("- Total elapsed time in seconds.\n");
    printf("- Average FPS.\n");
    printf("- For each processor:\n");
    printf("\tThe time it took to initialize the context in milliseconds.\n");
    printf("\tPrompt processing tokens per second(pp t/s).\n");
    printf("\tToken generation tokens per second(tg t/s).\n");
    printf("### NOTE ###\n");
    printf("If the context kv cache is filled and there still are tokens to predict, they will not be processed, since we are not doing context window shifting.\n");
    printf("========================================\n\n");
    printf("Usage: mbase-benchmark-t2t <model_path> *[<option> [<value>]]\n");
    printf("Options: \n\n");
    printf("-h, --help                           Print usage.\n");
    printf("-v, --version                        Shows program version.\n");
    printf("-dfa, --disable-flash-attention      Disables the flash attention, which is enabled by default.\n");
    printf("-t, --thread-count <int>             Amount of threads to use for output processing (default=16).\n");
    printf("-bt, --batch-thread-count <int>      Amount of threads to use for initial batch processing (default=8).\n");
    printf("-c, --context-length <int>           Total context length (default=2048).\n");
    printf("-b, --batch-length <int>             Batch length (default=512).\n");
    printf("-pr, --prompt-length <int>           Prompt length (default=1024).\n");
    printf("-gl, --gpu-layers <int>              GPU layers to offload to (default=999).\n");
    printf("-np, --n-predict <int>               Number of tokens to predict (default=256).\n");
    printf("-uc, --user-count <int>              Number of users that will be processed in parallel. (default=1)\n");
    printf("-fps, --frame-per-second <int>       Max FPS of the main loop. This is for measuring the effects of inference engine on main application loop(default=500, min=10, max=1000).\n");
    printf("-jout, --json-output-path <str>      If the json output path is specified, result will be written there in file(mbase_bench.json) (default='').\n");
    printf("-mdout, --markdown-output-path <str> If the markdown output path is specified, result will be written there in file(mbase_bench.md) (default='').\n\n");
}

class BenchmarkModel : public InfModelTextToText {
public:
    GENERIC on_initialize_fail([[maybe_unused]] init_fail_code out_fail_code) override{}
    GENERIC on_initialize() override{}
    GENERIC on_destroy() override{}
private:
};

class BenchmarkProcessor : public InfProcessorTextToText {
public:
    BenchmarkProcessor() : mNPredict(gSampleParams.mPredictCount) {}
    GENERIC on_initialize() override{}
    GENERIC on_initialize_fail([[maybe_unused]] last_fail_code out_code) override
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
    GENERIC on_register([[maybe_unused]] InfProcessorBase* out_processor) override
    {
    }

    GENERIC on_unregister([[maybe_unused]] InfProcessorBase* out_processor) override
    {
    }
    
    GENERIC on_batch_processed(InfProcessorTextToText* out_processor, [[maybe_unused]] const U32& out_proc_batch_length) override
    {
        BenchmarkProcessor* hostProcessor = static_cast<BenchmarkProcessor*>(out_processor);
        mbase::decode_behavior_description dbd;
        dbd.mTokenAtMost = 1;
        dbd.mHaltOnWrite = false;
        hostProcessor->next(dbd);
    }

    GENERIC on_write(InfProcessorTextToText* out_processor,[[maybe_unused]] const inf_text_token_vector& out_token, bool out_is_finish) override
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

    GENERIC on_finish(InfProcessorTextToText* out_processor, [[maybe_unused]] size_type out_total_token_size, [[maybe_unused]] InfProcessorTextToText::finish_state out_finish_state) override
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
    
    if(gSampleParams.mModelFile == "-v" || gSampleParams.mModelFile == "--version")
    {
        printf("MBASE Benchmark %s\n", MBASE_BENCHMARK_VERSION);
        return 0;
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

        else if(argumentString == "-dfa" || argumentString == "--disable-flash-attention")
        {
            gSampleParams.mFlashAttention = false;
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

        else if(argumentString == "-pr" || argumentString == "--prompt-length")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mPromptLength);
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

        else if(argumentString == "-fps" || argumentString == "--frame-per-second")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mFps);
        }

        else if(argumentString == "-jout" || argumentString == "--json-output-path")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mJsonOut);
        }

        else if(argumentString == "-mdout" || argumentString == "--markdown-output-path")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mMdOut);
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

    if(gSampleParams.mFps < 10 || gSampleParams.mFps > 1000)
    {
        printf("ERR: Invalid FPS value(%d). It must be between [10, 1000]\n", gSampleParams.mFps);
    }
    F32 roundedSeconds = std::round((1 / (F32)gSampleParams.mFps) * (1000));
    gSampleParams.mFps = 1 / (roundedSeconds / 1000.0f);
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

    for(BenchmarkProcessor* tmpProc : processorsList)
    {
        benchModel.register_context_process(
            tmpProc,
            gSampleParams.mContextLength,
            gSampleParams.mBatchLength,
            gSampleParams.mThreadCount,
            gSampleParams.mBatchThreadCount,
            gSampleParams.mFlashAttention,
            {}
        );

        while(!tmpProc->is_registered())
        {
            tmpProc->update();
            mbase::sleep(2);
        }
        tmpProc->set_inference_client(&benchClient);
    }

    signal(SIGINT, catching_interrupt_signal);
    
    SIZE_T modelSize = 0;
    U32 embeddingLength = 0;
    U32 headCount = 0;
    U32 layerCount = 0;
    benchModel.get_size(modelSize);
    benchModel.get_embedding_length(embeddingLength);
    benchModel.get_head_count(headCount);
    benchModel.get_layer_count(layerCount);
    mbase::string modelName;
    F32 tmpModelSize = (modelSize / (F32)(1024*1024*1024));
    benchModel.get_model_name(modelName);
    printf("==== Session Information ====\n\n");
    printf("- Model Information: \n");
    printf("\tName: %s\n", modelName.c_str());
    printf("\tModel size: %.2f %s", tmpModelSize, "GB\n");
    printf("\tQuantization: %s\n", benchModel.get_quantization_string().c_str());
    printf("\tEmbedding length: %d\n", embeddingLength);
    printf("\tHead count: %d\n", headCount);
    printf("\tLayer count: %d\n", layerCount);
    printf("- Context length: %u\n", gSampleParams.mContextLength);
    printf("- Batch size: %u\n", gSampleParams.mBatchLength);
    printf("- Prompt length: %u\n", gSampleParams.mPromptLength);
    printf("- Batch processing threads: %u\n", gSampleParams.mBatchThreadCount);
    printf("- Generation threads: %u\n", gSampleParams.mThreadCount);
    printf("- User count: %u\n", gSampleParams.mUserCount);
    printf("- N Predict: %u\n", gSampleParams.mPredictCount);
    printf("- Flash attention: %s\n", gSampleParams.mFlashAttention ? "Enabled" : "Disabled");
    printf("- GPU Offload layers: %d\n", gSampleParams.mGpuLayer);
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
        default:
            break;
        }
        printf("\t%s ## Type: %s\n", tmpDescription.get_device_description().c_str(), typeString.c_str());
    }
    for(BenchmarkProcessor* tmpProc : processorsList)
    {
        mbase::inf_text_token_vector tokVec(gSampleParams.mPromptLength, 1);
        tmpProc->execute_input(tokVec);
    }

    I32 frameCounter = 0;
    I32 totalFps = 0;
    U64 diagnosticFps = 0;
    U64 nonResetFrameCounter = 0;
    I32 sleepInterval = roundedSeconds;
    
    std::chrono::high_resolution_clock::time_point programBeginTime = std::chrono::high_resolution_clock::now();
    while(gIsProgramRunning)
    {
        std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();
        benchModel.update();
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        
        I32 deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
        if(deltaTime < sleepInterval)
        {
            beginTime = std::chrono::high_resolution_clock::now();
            mbase::sleep(sleepInterval);
            endTime = std::chrono::high_resolution_clock::now();
            deltaTime += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
        }
        frameCounter++;
        totalFps += 1 / (deltaTime / 1000.0f);
        if(frameCounter == gSampleParams.mFps)
        {
            diagnosticFps += totalFps / frameCounter;
            printf("\rAverage FPS per %d frames: %d", gSampleParams.mFps, totalFps / frameCounter);
            fflush(stdout);
            frameCounter = 0;
            totalFps = 0;
            nonResetFrameCounter++;
        }
    }
    std::chrono::high_resolution_clock::time_point programEndTime = std::chrono::high_resolution_clock::now();

    F32 averageFps = diagnosticFps / (F32)nonResetFrameCounter;
    F32 totalElapsedTimeInSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(programEndTime - programBeginTime).count() / 1000.0f;

    printf("\n");
    printf("\n==== Useful metrics ====\n");
    printf("- Total elapsed time in seconds: %.2f\n", totalElapsedTimeInSeconds);
    printf("- Average FPS: %.1f\n", averageFps);
    printf("\n==== Processor diagnostics ====\n");
    printf("| Load delay ms | pp t/s | tg t/s |\n");
    
    for(BenchmarkProcessor* tmpProc : processorsList)
    {
        InfProcT2TDiagnostics& t2tDiag = tmpProc->get_diagnostics();
        printf("|%" PRId64 "\t\t| %.2f |\t %.2f \t|\n", t2tDiag.loadTimeInMilliseconds, t2tDiag.ppTokensPerSecond, t2tDiag.evalTokensPerSecond);
        tmpProc->release_inference_client_stacked();
    }

    if(gSampleParams.mJsonOut.size())
    {
        gSampleParams.mJsonOut.push_back('/');
        mbase::string jsonFile = gSampleParams.mJsonOut + "mbase_bench.json";

        mbase::Json jsOut;
        jsOut.setObject();
        jsOut["model_information"]["name"] = modelName;
        jsOut["model_information"]["model_size_gb"] = tmpModelSize;
        jsOut["model_information"]["embedding_length"] = embeddingLength;
        jsOut["model_information"]["head_count"] = headCount;
        jsOut["model_information"]["layer_count"] = layerCount;
        jsOut["model_information"]["quantization"] = benchModel.get_quantization_string();
        jsOut["session_information"]["context_length"] = gSampleParams.mContextLength;
        jsOut["session_information"]["batch_size"] = gSampleParams.mBatchLength;
        jsOut["session_information"]["prompt_length"] = gSampleParams.mPromptLength;
        jsOut["session_information"]["batch_proc_threads"] = gSampleParams.mBatchThreadCount;
        jsOut["session_information"]["generation_threads"] = gSampleParams.mThreadCount;
        jsOut["session_information"]["user_count"] = gSampleParams.mUserCount;
        jsOut["session_information"]["predict"] = gSampleParams.mPredictCount;
        jsOut["session_information"]["gpu_layers"] = gSampleParams.mGpuLayer;
        jsOut["session_information"]["flash_attention"] = gSampleParams.mFlashAttention;

        mbase::Json computeDevices;
        computeDevices.setArray();

        I32 i = 0;

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
            default:
                break;
            }
            computeDevices[i]["device_name"] = tmpDescription.get_device_description();
            computeDevices[i]["type"] = typeString;
            ++i;
        }
        i = 0;
        jsOut["session_information"]["compute_devices"] = computeDevices;
        jsOut["useful_metrics"]["total_elapsed_time_seconds"] = totalElapsedTimeInSeconds;
        jsOut["useful_metrics"]["average_fps"] = averageFps;
        
        mbase::Json processorDiagnostics;
        processorDiagnostics.setArray();

        for(BenchmarkProcessor* tmpProc : processorsList)
        {
            InfProcT2TDiagnostics& t2tDiag = tmpProc->get_diagnostics();
            processorDiagnostics[i]["load_delay_ms"] = t2tDiag.loadTimeInMilliseconds;
            processorDiagnostics[i]["pp tokens per sec"] = t2tDiag.ppTokensPerSecond;
            processorDiagnostics[i]["tg tokens per sec"] = t2tDiag.evalTokensPerSecond;
            ++i;
        }

        jsOut["processor_diagnostics"] = processorDiagnostics;

        mbase::string outString = jsOut.toStringPretty();
        mbase::io_file iof;
        iof.open_file(mbase::from_utf8(jsonFile));
        if(!iof.is_file_open())
        {
            printf("ERR: Unable to output mbase_bench.json on path: %s\n", gSampleParams.mJsonOut.c_str());
        }
        else
        {
            iof.write_data(outString);
        }
    }

    if(gSampleParams.mMdOut.size())
    {
        gSampleParams.mMdOut.push_back('/');
        mbase::string markdownFile = gSampleParams.mJsonOut + "mbase_bench.md";

        mbase::string modelInfoMd = "### Model Information\n" 
        + mbase::string::from_format("__Name__: %s<br>\n", modelName.c_str()) 
        + mbase::string::from_format("__Model size__: %.2f %s", tmpModelSize, "GB <br>\n")
        + mbase::string::from_format("__Quantization__: %s<br>\n", benchModel.get_quantization_string().c_str())
        + mbase::string::from_format("__Embedding length__: %ld<br>\n", embeddingLength)
        + mbase::string::from_format("__Head count__: %ld<br>\n", headCount)
        + mbase::string::from_format("__Layer count__: %ld<br>\n", layerCount);

        mbase::string sessionInfoMd = "### Session Information\n"
        + mbase::string::from_format("__Context length__: %ld<br>\n", gSampleParams.mContextLength)
        + mbase::string::from_format("__Batch size__: %ld<br>\n", gSampleParams.mBatchLength)
        + mbase::string::from_format("__Prompt length__: %ld<br>\n", gSampleParams.mPromptLength)
        + mbase::string::from_format("__Batch processing threads__: %ld<br>\n", gSampleParams.mBatchThreadCount)
        + mbase::string::from_format("__Generation threads__: %ld<br>\n", gSampleParams.mThreadCount)
        + mbase::string::from_format("__User count__: %ld<br>\n", gSampleParams.mUserCount)
        + mbase::string::from_format("__Flash attention__: %s<br>\n", gSampleParams.mFlashAttention ? "Enabled" : "Disabled")
        + mbase::string::from_format("__GPU offload layers__: %ld<br>\n", gSampleParams.mGpuLayer)
        + mbase::string::from_format("__N Predict__: %ld<br>\n", gSampleParams.mPredictCount) + "__Compute devices__:\n";

        for(InfDeviceDescription& tmpDescription : deviceDescription)
        {
            sessionInfoMd += "- " + tmpDescription.get_device_description() + '\n';
        }

        mbase::string usefulMetricsMd = "### Useful Metrics\n"
        + mbase::string::from_format("__Total elapsed time in seconds__: %.2f<br>\n", totalElapsedTimeInSeconds)
        + mbase::string::from_format("__Average FPS__: %1.f<br>\n", averageFps);

        mbase::string processorDiagnostics = "### Performance Table\n"
        + mbase::string("| Load delay ms | pp t/s | tg t/s |\n")
        + mbase::string("| ------------- | ------ | ------ |\n");
        for(BenchmarkProcessor* tmpProc : processorsList)
        {
            InfProcT2TDiagnostics& t2tDiag = tmpProc->get_diagnostics();
            processorDiagnostics += mbase::string::from_format("| %lld | %.2f | %.2f |\n", t2tDiag.loadTimeInMilliseconds, t2tDiag.ppTokensPerSecond, t2tDiag.evalTokensPerSecond);
        }
        mbase::string totalMdContent = modelInfoMd + sessionInfoMd + usefulMetricsMd + processorDiagnostics;
        
        mbase::io_file iof;
        iof.open_file(mbase::from_utf8(markdownFile));
        if(!iof.is_file_open())
        {
            printf("ERR: Unable to output mbase_bench.md on path: %s\n", gSampleParams.mMdOut.c_str());
        }
        iof.write_data(totalMdContent);
    }
    return 0;
}