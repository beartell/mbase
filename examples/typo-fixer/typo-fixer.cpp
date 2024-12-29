#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_client.h>
#include <mbase/argument_get_value.h>
#include <mbase/filesystem.h>
#include <math.h>
#include <chrono>
#include <signal.h>

#define MBASE_TYPO_FIXER_VERSION "v1.0.2"

using namespace mbase;

class FixerModel;
class FixerProcessor;
class FixerClient;

struct program_parameters {
    mbase::string mModelFile;
    mbase::string mSourceTextContent;
    mbase::string mOutputFile;
    I32 mThreadCount = 16;
    I32 mBatchThreadCount = 8;
    I32 mGpuLayer = 999;
};

mbase::vector<InfDeviceDescription> deviceDescription;
program_parameters gSampleParams;
bool gIsProgramRunning = true;

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
    printf("#Program name:      mbase-typo-fixer\n");
    printf("#Version:           %s\n", MBASE_TYPO_FIXER_VERSION);
    printf("#Type:              Example\n");
    printf("#Further docs: \n");
    printf("***** DESCRIPTION *****\n");
    printf("This is an applied example use case of MBASE Library.\n");
    printf("The program is reading a user-supplied text file and fixing the typos.\n");
    printf("Fixed text will be written to a file specified by option -o or --output-file.\n");
    printf("========================================\n\n");
    printf("Usage: mbase-typo-fixer <model_path> *[<option> [<value>]]\n");
    printf("Options: \n\n");
    printf("--help                            Print usage.\n");
    printf("-v, --version                     Shows program version.\n");
    printf("-t, --thread-count <int>          Amount of threads to use for output processing (default=16).\n");
    printf("-bt, --batch-thread-count <int>   Amount of threads to use for initial batch processing (default=8).\n");
    printf("-gl, --gpu-layers <int>           GPU layers to offload to (default=999).\n");
    printf("-s, --source-file <str>           File containing typos.\n");
    printf("-o, --output-file <str>           Output file where fixed text will be written to.\n\n");
}

class FixerModel : public InfModelTextToText {
public:
    GENERIC on_initialize_fail([[maybe_unused]] init_fail_code out_fail_code) override{}
    GENERIC on_initialize() override{}
    GENERIC on_destroy() override{}
private:
};

class FixerProcessor : public InfProcessorTextToText {
public:
    GENERIC on_initialize() override{}
    GENERIC on_initialize_fail([[maybe_unused]] last_fail_code out_code) override
    {
        fflush(stdout);
        printf("ERR: Context initialization failed.\n");
        printf("ERR: Insufficent memory.\n");
        printf("INFO: Source file is possibly too large.\n");
        exit(1);
    }
    GENERIC on_destroy() override{}
private:
};

class FixerClient : public InfClientTextToText {
public:
    FixerClient(const mbase::string& in_system_prompt, const mbase::string& in_typo_text)
    {
        // Setting up system prompt and user message
        U32 tmpMessageId = 0;
        this->add_message(in_system_prompt, mbase::context_role::SYSTEM, tmpMessageId);
        mChatHistory.push_back(tmpMessageId);
        this->add_message(in_typo_text, mbase::context_role::USER, tmpMessageId);
        mChatHistory.push_back(tmpMessageId);
    }

    GENERIC on_register(InfProcessorBase* out_processor) override
    {
        FixerProcessor* hostProcessor = static_cast<FixerProcessor*>(out_processor);
        mbase::vector<context_line> contextLines;
        this->get_message_array(mChatHistory.data(), mChatHistory.size(), contextLines);

        for(auto& n : contextLines)
        {
            printf("%s\n", n.mMessage.c_str());
        }

        if(hostProcessor->tokenize_input(contextLines.data(), contextLines.size(), mInputTokens) != FixerProcessor::flags::INF_PROC_SUCCESS)
        {
            printf("ERR: Tokenization failed for any reason.\n");
            exit(1);
        }
        hostProcessor->execute_input(mInputTokens);
    }

    GENERIC on_unregister([[maybe_unused]] InfProcessorBase* out_processor) override{}
    
    GENERIC on_batch_processed(InfProcessorTextToText* out_processor, [[maybe_unused]] const U32& out_proc_batch_length) override
    {
        FixerProcessor* hostProcessor = static_cast<FixerProcessor*>(out_processor);
        
        printf("Started fixing...\n");
        mbase::decode_behavior_description dbd;
        dbd.mTokenAtMost = 1;
        dbd.mHaltOnWrite = false;
        hostProcessor->next(dbd);
    }

    GENERIC on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) override
    {
        if(out_is_finish)
        {
            return;
        }

        FixerProcessor* hostProcessor = static_cast<FixerProcessor*>(out_processor);
        mbase::inf_token_description tokenDesc;
        mbase::decode_behavior_description dbd;
        dbd.mTokenAtMost = 1;
        dbd.mHaltOnWrite = false;

        hostProcessor->token_to_description(out_token[0], tokenDesc);

        mGeneratedOutput += tokenDesc.mTokenString;

        hostProcessor->next(dbd);
    }

    GENERIC on_finish([[maybe_unused]] InfProcessorTextToText* out_processor, [[maybe_unused]] size_type out_total_token_size, [[maybe_unused]] InfProcessorTextToText::finish_state out_finish_state) override
    {
        printf("Fixed text is written to file: %s\n", gSampleParams.mOutputFile.c_str());
        mbase::io_file iof;
        iof.open_file(mbase::from_utf8(gSampleParams.mOutputFile));
        iof.write_data(mGeneratedOutput);
        
        gIsProgramRunning = false;
    }
private:
    mbase::inf_text_token_vector mInputTokens;
    mbase::string mGeneratedOutput;
    mbase::vector<U32> mChatHistory;
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
        printf("MBASE Typo fixer %s\n", MBASE_TYPO_FIXER_VERSION);
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
        if(argumentString == "-v" || argumentString == "--version")
        {
            printf("MBASE Typo fixer %s\n", MBASE_TYPO_FIXER_VERSION);
            return 0;
        }

        else if(argumentString == "--thread-count" || argumentString == "-t")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mThreadCount);
            if(gSampleParams.mThreadCount <= 0)
            {
                printf("ERR: Thread must be at least 1 but given: %d\n", gSampleParams.mThreadCount);
                return 1;   
            }
        }

        else if(argumentString == "--batch-thread-count" || argumentString == "-bt")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mBatchThreadCount);
            if(gSampleParams.mBatchThreadCount <= 0)
            {
                printf("ERR: Thread must be at least 1 but given: %d\n", gSampleParams.mBatchThreadCount);
                return 1;
            }
        }

        else if(argumentString == "--gpu-layers" || argumentString == "-gl")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mGpuLayer);
        }
        
        else if(argumentString == "-s" || argumentString == "--source-file")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mSourceTextContent);
        }

        else if(argumentString == "-o" || argumentString == "--output-file")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mOutputFile);
        }

        else if(argumentString == "--help")
        {
            print_usage();
            return 1;
        }
    }
    
    if(!gSampleParams.mSourceTextContent.size())
    {
        printf("ERR: Missing source file.\n");
        return 1;
    }

    if(!gSampleParams.mOutputFile.size())
    {
        gSampleParams.mOutputFile = "fixed_text";
    }

    if(!mbase::is_file_valid(mbase::from_utf8(gSampleParams.mSourceTextContent)))
    {
        printf("ERR: Can't open source file: %s\n", gSampleParams.mSourceTextContent.c_str());
        return 1;
    }

    gSampleParams.mSourceTextContent = mbase::read_file_as_string(mbase::from_utf8(gSampleParams.mSourceTextContent));
    if(!gSampleParams.mSourceTextContent.size())
    {
        printf("ERR: Source file content empty.\n");
        return 1;
    }

    deviceDescription = inf_query_devices();

    FixerModel fixerModel;
    FixerProcessor fixerProcessor;
    FixerClient fixerClient("You are fixing typos in the given message and returning the corrected version to the user besides that do not add any remarks like or what you fixed in the text, just fix it and return.", gSampleParams.mSourceTextContent);
    fixerModel.initialize_model_ex_sync(mbase::from_utf8(gSampleParams.mModelFile), 1200000, gSampleParams.mGpuLayer, true, true, deviceDescription);
    
    if(!fixerModel.is_initialized())
    {
        printf("ERR: Unable to initialize model\n");
        return 1;
    }

    fixerModel.update();

    mbase::inf_text_token_vector tokVec;
    if(fixerModel.tokenize_input(gSampleParams.mSourceTextContent.c_str(), gSampleParams.mSourceTextContent.size(), tokVec) == FixerModel::flags::INF_MODEL_ERR_TOKENIZATION_FAILED)
    {
        printf("ERR: Unable to tokenize input\n");
        return 1;
    }

    U32 topNearPower = std::pow(2, std::ceil(std::log2(tokVec.size())));
    U32 lowNearPower = std::pow(2, std::floor(std::log2(tokVec.size())));

    U32 tmpContextLength = topNearPower * 3;
    U32 tmpBatchLength = lowNearPower;

    fixerModel.register_context_process(
        &fixerProcessor,
        tmpContextLength,
        tmpBatchLength,
        gSampleParams.mThreadCount,
        gSampleParams.mBatchThreadCount,
        true,
        {}
    );

    // Waiting for processor registration
    while(!fixerProcessor.is_registered())
    {
        fixerProcessor.update();
        mbase::sleep(2);
    }

    signal(SIGINT, catching_interrupt_signal);

    mbase::string modelName;
    fixerModel.get_model_name(modelName);
    printf("==== Session Information ====\n\n");
    printf("- Model name: %s\n", modelName.c_str());
    printf("- Context length: %u\n", tmpContextLength);
    printf("- Batch size: %u\n", tmpBatchLength);
    printf("- Batch processing threads: %u\n", gSampleParams.mBatchThreadCount);
    printf("- Generation threads: %u\n", gSampleParams.mThreadCount);
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

    while(gIsProgramRunning)
    {
        std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();
        fixerProcessor.set_inference_client(&fixerClient);
        fixerModel.update();
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        
        if(std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count() < 2)
        {
            mbase::sleep(2);
        }
    }

    fixerProcessor.release_inference_client();
    InfProcT2TDiagnostics& t2tDiag = fixerProcessor.get_diagnostics();
    printf("\n==== Processor diagnostics ====\n");
    printf("| Context load delay         | %lu ms\n", t2tDiag.loadTimeInMilliseconds);
    printf("| Prompt processing rate(pp) | %f tokens/sec\n", t2tDiag.ppTokensPerSecond);
    printf("| Token generation rate(tg)  | %f tokens/sec\n", t2tDiag.evalTokensPerSecond);

    return 0;
}