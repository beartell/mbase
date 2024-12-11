#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_client.h>
#include <mbase/argument_get_value.h>
#include <mbase/filesystem.h>
#include <chrono>
#include <signal.h>

#define MBASE_SIMPLE_CONVERSATION_VERSION "v1.0.0"

using namespace mbase;

class ConversationModel;
class ConversationProcessor;
class ConversationClient;

struct program_parameters {
    mbase::string mModelFile;
    mbase::string mSystemPromptFile;
    mbase::string mSystemPrompt;
    I32 mThreadCount = 16;
    I32 mBatchThreadCount = 8;
    I32 mContextLength = 1024;
    I32 mBatchLength = 512;
    I32 mGpuLayer = 999;
};

mbase::vector<InfDeviceDescription> deviceDescription;
program_parameters gSampleParams;
bool gIsProgramRunning = true;

GENERIC catching_interrupt_signal(I32 out_sig_id)
{
    printf("Program interrupted\n");
    gIsProgramRunning = false;
}

GENERIC print_usage()
{
    printf("========================================\n");
    printf("#Program name:      mbase-simple-conversation\n");
    printf("#Version:           %s\n", MBASE_SIMPLE_CONVERSATION_VERSION);
    printf("#Type:              Example\n");
    printf("#Further docs: \n");
    printf("***** DESCRIPTION *****\n");
    printf("An example program for creating a conversation with the given TextToText LLM model.\n");
    printf("The given implementation is stable and shows the basics of non-blocking LLM inference.\n");
    printf("The code section here will mostly be used by other example programs.\n");
    printf("========================================\n\n");
    printf("Usage: mbase-simple-conversation <model_path> *[<option> [<value>]]\n");
    printf("Options: \n\n");
    printf("--help                            Print usage.\n");
    printf("-v, --version                     Shows program version.\n");
    printf("-sys, --system-prompt <str>       LLM system prompt (if this option is given after -fsys, it will overwrite it).\n");
    printf("-fsys, --system-prompt-file <str> LLM system prompt file (if this option is given after -sys, it will overwrite it).\n");
    printf("-t, --thread-count <int>          Amount of threads to use for output processing (default=16).\n");
    printf("-bt, --batch-thread-count <int>   Amount of threads to use for initial batch processing (default=8).\n");
    printf("-c, --context-length <int>        Total context length (default=8192).\n");
    printf("-b, --batch-length <int>          Batch length (default=4096).\n");
    printf("-gl, --gpu-layers <int>           GPU layers to offload to (default=999).\n\n");
}

class ConversationModel : public InfModelTextToText {
public:
    GENERIC on_initialize_fail(init_fail_code out_fail_code) override{}
    GENERIC on_initialize() override{}
    GENERIC on_destroy() override{}
private:
};

class ConversationProcessor : public InfProcessorTextToText {
public:
    GENERIC on_initialize() override{}
    GENERIC on_initialize_fail(last_fail_code out_code) override
    {
        fflush(stdout);
        printf("ERR: Context initialization failed.\n");
        printf("ERR: Insufficent memory.\n");
        exit(1);
    }
    GENERIC on_destroy() override{}
private:
};

class ConversationClient : public InfClientTextToText {
public:
    ConversationClient(const mbase::string& in_system_prompt)
    {
        // Setting up system prompt on constructor
        if(in_system_prompt.size())
        {
            mSystemPromptString = in_system_prompt;
            this->add_message(in_system_prompt, mbase::context_role::SYSTEM, mSystemPromptId);
        }
    }

    GENERIC add_assistant_message(const mbase::string& in_message)
    {
        U32 outMessageId = 0;
        this->add_message(in_message, mbase::context_role::ASSISTANT, outMessageId);
        mChatHistory.push_back(outMessageId);
    }

    GENERIC add_user_message(const mbase::string& in_message)
    {
        U32 outMessageId = 0;
        this->add_message(in_message, mbase::context_role::USER, outMessageId);
        mChatHistory.push_back(outMessageId);
    }

    GENERIC start_conversation(ConversationProcessor* in_processor)
    {
        printf("User >> ");
        mbase::string userPrompt = mbase::get_line();
        if(userPrompt.size())
        {
            add_user_message(userPrompt);
        }
        mbase::vector<U32> messageArray(mChatHistory.begin(), mChatHistory.end());
        mbase::vector<context_line> contextLines;
        if(mSystemPromptId)
        {
            messageArray.insert(messageArray.begin(), mSystemPromptId);
        }

        this->get_message_array(messageArray.data(), messageArray.size(), contextLines);

        mbase::inf_text_token_vector tokenVector;
        in_processor->tokenize_input(contextLines.data(), contextLines.size(), tokenVector);
        if(in_processor->execute_input(tokenVector) == ConversationProcessor::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT)
        {   
        }
    }

    GENERIC on_register(InfProcessorBase* out_processor) override
    {
        ConversationProcessor* hostProcessor = static_cast<ConversationProcessor*>(out_processor);
        printf("System >> %s\n", mSystemPromptString.c_str());
        start_conversation(hostProcessor);
    }

    GENERIC on_unregister(InfProcessorBase* out_processor) override{}
    
    GENERIC on_batch_processed(InfProcessorTextToText* out_processor, const U32& out_proc_batch_length) override
    {
        ConversationProcessor* hostProcessor = static_cast<ConversationProcessor*>(out_processor);
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

        ConversationProcessor* hostProcessor = static_cast<ConversationProcessor*>(out_processor);
        mbase::inf_token_description tokenDesc;
        mbase::decode_behavior_description dbd;
        dbd.mTokenAtMost = 1;
        dbd.mHaltOnWrite = false;

        hostProcessor->token_to_description(out_token[0], tokenDesc);

        mGeneratedOutput += tokenDesc.mTokenString;

        fflush(stdout);
        printf("%s", tokenDesc.mTokenString.c_str());
        hostProcessor->next(dbd);
    }

    GENERIC on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state) override
    {
        printf("\n\n");
        ConversationProcessor* hostProcessor = static_cast<ConversationProcessor*>(out_processor);
        add_assistant_message(mGeneratedOutput);
        mGeneratedOutput.clear();
        start_conversation(hostProcessor);
    }
private:
    mbase::string mSystemPromptString;
    mbase::string mGeneratedOutput;
    U32 mSystemPromptId = 0;
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
            printf("MBASE Simple conversation %s\n", MBASE_SIMPLE_CONVERSATION_VERSION);
            return 0;
        }

        else if(argumentString == "-sys" || argumentString == "--system-prompt")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mSystemPrompt);
        }

        else if(argumentString == "-fsys" || argumentString == "--system-prompt-file")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mSystemPromptFile);
            if(!mbase::is_file_valid(mbase::from_utf8(gSampleParams.mSystemPromptFile)))
            {
                printf("ERR: Can't open system prompt file: %s\n", gSampleParams.mSystemPromptFile.c_str());
                return 1;
            }
            gSampleParams.mSystemPrompt = mbase::read_file_as_string(mbase::from_utf8(gSampleParams.mSystemPromptFile));
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

        else if(argumentString == "--batch-thread-count" || argumentString == "-bt")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mBatchThreadCount);
            if(gSampleParams.mBatchThreadCount <= 0)
            {
                printf("ERR: Thread must be at least 1 but given: %d\n", gSampleParams.mBatchThreadCount);
                return 1;
            }
        }

        else if(argumentString == "--help")
        {
            print_usage();
            return 1;
        }
    }
    
    deviceDescription = inf_query_devices();

    ConversationModel cnvModel;
    ConversationProcessor cnvProcessor;
    ConversationClient cnvClient(gSampleParams.mSystemPrompt);
    
    cnvModel.initialize_model_ex_sync(mbase::from_utf8(gSampleParams.mModelFile), 1200000, gSampleParams.mGpuLayer, true, true, deviceDescription);
    
    if(!cnvModel.is_initialized())
    {
        printf("ERR: Unable to initialize model\n");
        return 1;
    }

    cnvModel.register_context_process(
        &cnvProcessor,
        gSampleParams.mContextLength,
        gSampleParams.mBatchLength,
        gSampleParams.mThreadCount,
        gSampleParams.mBatchThreadCount,
        true,
        {}
    );

    // Waiting for processor registration
    while(!cnvProcessor.is_registered())
    {
        cnvProcessor.update();
        mbase::sleep(2);
    }

    signal(SIGINT, catching_interrupt_signal);

    printf("==== Session Information ====\n\n");
    printf("- Context length: %u\n", gSampleParams.mContextLength);
    printf("- Batch size: %u\n", gSampleParams.mBatchLength);
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
        }
        printf("\t%s ## Type: %s\n", tmpDescription.get_device_description().c_str(), typeString.c_str());
    }


    while(gIsProgramRunning)
    {
        std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();
        cnvProcessor.set_inference_client(&cnvClient);
        cnvModel.update();
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        
        if(std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count() < 2)
        {
            mbase::sleep(2);
        }
    }

    cnvProcessor.release_inference_client();
    InfProcT2TDiagnostics& t2tDiag = cnvProcessor.get_diagnostics();
    printf("\n==== Processor diagnostics ====\n");
    printf("| Context load delay         | %u ms\n", t2tDiag.loadTimeInMilliseconds);
    printf("| Prompt processing rate(pp) | %f token/sec\n", t2tDiag.ppTokensPerSecond);
    printf("| Token generation rate(tg)  | %f token/sec\n", t2tDiag.evalTokensPerSecond);

    return 0;
}