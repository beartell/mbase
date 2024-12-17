#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_embedder_client.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/argument_get_value.h>
#include <mbase/filesystem.h>
#include <mbase/io_file.h>
#include <iostream>

#define MBASE_EMBEDDING_SIMPLE_VERSION "v1.0.0"

using namespace mbase;

class EmbedderModel;
class EmbedderProcessor;
class EmbedderClient;

struct program_parameters {
    mbase::string mModelFile; // direct argument
    mbase::string mPromptFile; // -pf, --prompt-file
    mbase::string mPrompt; // -p , --prompt
    mbase::string mSeperator = "<embd_sep>"; // -sp, --seperator == prompt content seperator (default=<mbembd_sep>)
    mbase::string mJout; // -jout, --json-output-path
    I32 mThreadCount = 16; // -t, --thread-count
    I32 mGpuLayer = 999;
};

mbase::vector<mbase::string> gLoadedPrompts;
program_parameters gSampleParams;

GENERIC print_usage()
{
    printf("========================================\n");
    printf("#Program name:      mbase-embedding-simple\n");
    printf("#Version:           %s\n", MBASE_EMBEDDING_SIMPLE_VERSION);
    printf("#Type:              Example\n");
    printf("#Further docs: \n");
    printf("***** DESCRIPTION *****\n");
    printf("An example program for generating the embeddings of the given prompt or prompts.\n");
    printf("The given implementation is stable and shows the basics of how to generate embeddings using MBASE embedding procesor.\n");
    printf("========================================\n\n");
    printf("Usage: mbase-embedding-simple <model_path> *[<option> [<value>]]\n");
    printf("Options: \n\n");
    printf("-h, --help                      Print usage.\n");
    printf("-v, --version                   Shows program version.\n");
    printf("-pf, --prompt-file <str>        File containing prompt or prompts seperated by the seperator (default='').\n");
    printf("-p, --prompt <str>              Prompt or prompts seperated by the seperator (default='').\n");
    printf("-sp, --seperator <str>          Prompt seperator (default=\"<mbembd_sep>\").\n");
    printf("-t, --thread-count <int>        Threads used to compute embeddings (default=16).\n");
    printf("-gl, --gpu-layers <int>         GPU layers to offload to (default=999).\n");
    printf("-jout, --json-output-path <str> If the json output path is specified, result will be written there in file(generated_embeddings.json) (default='').\n\n");
}

class EmbedderModel : public InfModelTextToText {
public:
    GENERIC on_initialize_fail(init_fail_code out_fail_code) override{}
    GENERIC on_initialize() override{}
    GENERIC on_destroy() override{}
private:
};

class EmbedderProcessor : public InfEmbedderProcessor {
public:
    GENERIC on_initialize() override{}
    GENERIC on_destroy() override{}
private:

};

class EmbedderClient : public InfClientEmbedder {
public:
    GENERIC on_register(InfProcessorBase* out_processor) override
    {
        InfEmbedderProcessor* hostProc = static_cast<InfEmbedderProcessor*>(out_processor);
        printf("Client registered!!!\n");
        mbase::inf_text_token_vector tokVec;
        hostProc->tokenize_input("What is TSNE", tokVec);
        hostProc->execute_input({tokVec});
    }

	GENERIC on_unregister(InfProcessorBase* out_processor) override
    {

    }

    GENERIC on_batch_processed(InfEmbedderProcessor* out_processor, const U32& out_proc_batch_length) override
    {
        printf("Batch is processed: %d\n", out_proc_batch_length);
        InfEmbedderProcessor* hostProc = static_cast<InfEmbedderProcessor*>(out_processor);
        hostProc->next();
    }

    GENERIC on_write(InfEmbedderProcessor* out_processor, PTRF32 out_embeddings, const U32& out_cursor, bool out_is_finished) override
    {
        for(I32 i = 0; i < out_processor->get_embedding_length(); i++)
        {
            printf("%.6f ", out_embeddings[i]);
        }
        printf("\n\n");
        InfEmbedderProcessor* hostProc = static_cast<InfEmbedderProcessor*>(out_processor);
        mbase::inf_text_token_vector tokVec;
        hostProc->tokenize_input("What is TSNE", tokVec);
        hostProc->execute_input({tokVec});
    }

    GENERIC on_finish(InfEmbedderProcessor* out_processor, const size_type& out_total_processed_embeddings) override
    {
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
            printf("MBASE Embedding Simple %s\n", MBASE_EMBEDDING_SIMPLE_VERSION);
            return 0;
        }

        else if(argumentString == "-pf" || argumentString == "--prompt-file")
        {
            
        }
    }

    // EmbedderModel embdModel;
    // EmbedderProcessor embdProcessor;
    // EmbedderClient embdClient;
    // embdModel.initialize_model_sync(L"/home/erdog/Downloads/nomic-embed-text-v1.Q8_0.gguf", 32000, -1);
    // embdModel.update();
    // U32 ctxLength = 0;
    // embdModel.get_max_embedding_context(ctxLength);
    // embdModel.register_context_process(&embdProcessor, ctxLength, 8);

    // printf("EMBEDDING CONTEXT: %d\n", ctxLength);

    // while(1)
    // {
    //     embdProcessor.set_inference_client(&embdClient);
    //     embdProcessor.update();    
    // }

    return 0;
}