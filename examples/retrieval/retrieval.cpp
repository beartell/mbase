#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_embedder_client.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/argument_get_value.h>
#include <mbase/filesystem.h>
#include <mbase/io_file.h>
#include <mbase/json/json.h>
#include <iostream>

#define MBASE_RETRIEVAL_VERSION "v1.0.0"

using namespace mbase;

class EmbedderModel;
class EmbedderProcessor;
class EmbedderClient;

struct program_parameters {
    mbase::string mModelFile; // direct argument
    mbase::vector<mbase::string> mPromptFiles;
    mbase::string mSeperator = "<embd_sep>"; // -sp, --seperator == prompt content seperator (default=<mbembd_sep>)
    mbase::string mQuery; // -q, --query
    I32 mThreadCount = 16; // -t, --thread-count
    I32 mGpuLayer = 999; // -gl, --gpu-layers
};

struct prompt_file_data {
    mbase::string mFileName;
    mbase::string mPromptContent;
    I32 mPromptIndex = 0;
    F32 mSimilarityValue = 0.0f;

    friend bool operator<(const prompt_file_data& in_lhs, const prompt_file_data& in_rhs)
    {
        return in_lhs.mSimilarityValue < in_rhs.mSimilarityValue;
    }
};

mbase::set<int> gPromptFileData;
mbase::vector<F32> gQueryEmbeddingVector;
mbase::vector<mbase::string> gLoadedPrompts;
mbase::vector<mbase::string>::iterator gPromptIt;
program_parameters gSampleParams;
mbase::string gModelName;
bool gQueryEmbedded = false;

GENERIC print_usage()
{
    printf("========================================\n");
    printf("#Program name:      mbase-retrieval\n");
    printf("#Version:           %s\n", MBASE_RETRIEVAL_VERSION);
    printf("#Type:              Example\n");
    printf("#Further docs: \n");
    printf("***** DESCRIPTION *****\n");
    printf("An example program for generating using the embedder to retrieval operation on multiple text seperated by <embd_sep>.\n");
    printf("What this program does is that it takes a query from the user and creates the embeddings of the user input and all other texts.\n");
    printf("After the embeddings are generated, it applies the cosine similarity function to all embeddings and shows the distance of the query to each text.\n");
    printf("The code here is using the embedder.cpp's implementation as a base and makes slight modifications to it.\n");
    printf("========================================\n\n");
    printf("Usage: mbase-retrieval <model_path> *[<option> [<value>]]\n");
    printf("Options: \n\n");
    printf("-h, --help                      Print usage.\n");
    printf("-v, --version                   Shows program version.\n");
    printf("-q, --query <str>               User query.\n");
    printf("-pf, --prompt-file <str>        File containing prompt or prompts seperated by the seperator (default=''). To give multiple prompt files, call this option multiple times.\n");
    printf("-sp, --seperator <str>          Prompt seperator (default=\"<embd_sep>\").\n");
    printf("-t, --thread-count <int>        Threads used to compute embeddings (default=16).\n");
    printf("-gl, --gpu-layers <int>         GPU layers to offload to (default=999).\n\n");
}

class EmbedderModel : public InfModelTextToText {
public:
    GENERIC on_initialize_fail(init_fail_code out_fail_code) override
    {
        printf("ERR: Failed loading model.\n");
        exit(1);
    }
    GENERIC on_initialize() override
    {
        this->get_model_name(gModelName);
    }
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
        gPromptIt = gLoadedPrompts.begin();
        InfEmbedderProcessor* hostProc = static_cast<InfEmbedderProcessor*>(out_processor);
        mbase::inf_text_token_vector tokVec;
        hostProc->tokenize_input(gSampleParams.mQuery, tokVec);
        if(hostProc->execute_input({tokVec}) == InfEmbedderProcessor::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT)
        {
            printf("ERR: Given queries token length is greater than embedder context length.\n");
            exit(1);
        }
    }

	GENERIC on_unregister(InfProcessorBase* out_processor) override
    {
    }

    GENERIC on_batch_processed(InfEmbedderProcessor* out_processor, const U32& out_proc_batch_length) override
    {
        InfEmbedderProcessor* hostProc = static_cast<InfEmbedderProcessor*>(out_processor);
        hostProc->next();
    }

    GENERIC on_write(InfEmbedderProcessor* out_processor, PTRF32 out_embeddings, const U32& out_cursor, bool out_is_finished) override
    {
        const U32& embeddingLength = out_processor->get_embedding_length();
        inf_common_embd_normalize(out_embeddings, out_embeddings, embeddingLength);
        if(!gQueryEmbedded)
        {
            for(I32 i = 0; i < embeddingLength; ++i)
            {
                gQueryEmbeddingVector.push_back(out_embeddings[i]);
            }
            gQueryEmbedded = true;
        }
        else
        {
            printf("Cosine similarity: %f\n", inf_common_cosine_similarity(gQueryEmbeddingVector.data(), out_embeddings, embeddingLength));
        }
    }

    GENERIC on_finish(InfEmbedderProcessor* out_processor, const size_type& out_total_processed_embeddings) override
    {
        InfEmbedderProcessor* hostProc = static_cast<InfEmbedderProcessor*>(out_processor);

        if(gPromptIt != gLoadedPrompts.end())
        {
            mbase::string promptString;
            mbase::inf_text_token_vector tokVec;
            hostProc->tokenize_input(*gPromptIt, tokVec);
            if(hostProc->execute_input({tokVec}) == InfEmbedderProcessor::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT)
            {
                printf("ERR: Given prompt's token length is greater than embedder context length.\n");
                exit(1);
            }
            ++gPromptIt;
        }
        else
        {
            // ALL EMBEDDINGS ARE GENERATED, AND COSINE SIMILARITIES ARE CALCULATED.
            exit(0);
        }
    }
private:
};

int main(int argc, char** argv)
{   
    

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
            printf("MBASE Retrieval %s\n", MBASE_RETRIEVAL_VERSION);
            return 0;
        }

        else if(argumentString == "-q" || argumentString == "--query")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mQuery);
        }

        else if(argumentString == "-pf" || argumentString == "--prompt-file")
        {
            mbase::string tmpPromptFile;
            mbase::argument_get<mbase::string>::value(i, argc, argv, tmpPromptFile);
            gSampleParams.mPromptFiles.push_back(tmpPromptFile);
        }

        else if(argumentString == "-sp" || argumentString == "--seperator")
        {
            mbase::argument_get<mbase::string>::value(i, argc, argv, gSampleParams.mSeperator);
        }

        else if(argumentString == "-t" || argumentString == "--thread-count")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mThreadCount);
        }

        else if(argumentString == "-gl" || argumentString == "--gpu-layers")
        {
            mbase::argument_get<I32>::value(i, argc, argv, gSampleParams.mGpuLayer);
        }
    }

    if(!gSampleParams.mQuery.size())
    {
        printf("ERR: Missing query\n");
        return 1;
    }

    if(!gSampleParams.mPromptFiles.size())
    {
        printf("ERR: Missing prompt files\n");
        return 1;
    }
    else
    {
        for(mbase::string& promptFile : gSampleParams.mPromptFiles)
        {
            mbase::string fileContent = mbase::read_file_as_string(mbase::from_utf8(promptFile));
            if(fileContent.size())
            {
                mbase::vector<mbase::string> filePromptContents;
                fileContent.split_full(gSampleParams.mSeperator, filePromptContents);
                for(mbase::string& promptContent : filePromptContents)
                {
                    gLoadedPrompts.push_back(promptContent);
                }
            }
        }
    }

    if(!gLoadedPrompts.size())
    {
        printf("ERR: No prompts are loaded.\n");
        return 1;
    }

    EmbedderModel embdModel;
    EmbedderProcessor embdProcessor;
    EmbedderClient embdClient;
    embdModel.initialize_model_sync(mbase::from_utf8(gSampleParams.mModelFile), 32000, -1);
    embdModel.update();

    if(!embdModel.is_embedding_model())
    {
        printf("ERR: Loaded model is not embedding model.\n");
        printf("INFO: Embedding simple program does not generate token embeddings\n");
        return 1;
    }

    U32 ctxLength = 0;
    embdModel.get_max_embedding_context(ctxLength);
    embdModel.register_context_process(&embdProcessor, ctxLength, gSampleParams.mThreadCount);

    while(1)
    {
        embdProcessor.set_inference_client(&embdClient);
        embdProcessor.update();    
        mbase::sleep(2);
    }

    return 0;
}