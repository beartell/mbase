#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_client.h>

MBASE_BEGIN

#define MBASE_SIMPLE_CONVERSATION_VERSION "v1.0.0"

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
    printf("Usage: mbase-simple-conversation *[<option> [<value>]]\n");
    printf("Options: \n\n");
    printf("--help                            Print usage.\n");
    printf("-v, --version                     Shows program version.\n");
    printf("-sys, --system-prompt <str>       LLM system prompt (if prompt file is given, this is will be overwritten).\n");
    printf("-fsys, --system-prompt-file <str> LLM system prompt file (will overwrite the system prompt).\n");
    printf("-m, --model-path <str>            Model file to be processed.\n");
    printf("-t, --thread-count <int>          Amount of threads to use for output processing (default=16).\n");
    printf("-bt, --batch-thread-count <int>   Amount of threads to use for initial batch processing (default=8).\n");
    printf("-c, --context-length <int>        Total context length (default=8192).\n");
    printf("-b, --batch-length <int>          Batch length (default=4096).\n");
    printf("-gl, --gpu-layers <int>           GPU layers to offload to (default=999).\n\n");
}

class ConversationModel : public InfModelTextToText {
public:
    GENERIC on_initialize_fail(init_fail_code out_fail_code) override;
    GENERIC on_initialize() override;
    GENERIC on_destroy() override;
private:
};

class ConversationProcessor : public InfProcessorTextToText {
public:
    GENERIC on_initialize() override;
    GENERIC on_destroy() override;
private:
};

class ConversationClient : public InfClientTextToText {
public:
    GENERIC on_register(InfProcessorBase* out_processor) override;
    GENERIC on_unregister(InfProcessorBase* out_processor) override;
    GENERIC on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) override;
    GENERIC on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state) override;
private:
    mbase::vector<U32> mChatHistory;
};

struct program_parameters {
    mbase::wstring mModelFile;
    mbase::wstring mSystemPromptFile;
    mbase::string mSystemPrompt;
    I32 mThreadCount = 16;
    I32 mBatchThreadCount = 8;
    I32 mContextLength = 8192;
    I32 mBatchLength = 4096;
    I32 mGpuLayer = 999;
};

MBASE_END

int main()
{
    mbase::ConversationModel cnvModel;
    
    return 0;
}