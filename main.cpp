#include <iostream>
#include <mbase/mbase_std.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_maip_server.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/inference/inf_maip_model_description.h>
#include <mbase/maip_client.h>
#include <vector>
#include <set>
#include <unordered_map>
#include <signal.h>

using namespace mbase;

class my_client : public mbase::InfClientTextToText {
public:
    GENERIC on_register(InfProcessorBase* out_processor) override 
    {
        InfEmbedderProcessor* emdProcessor = static_cast<InfEmbedderProcessor*>(out_processor);
        mbase::string inputString = "Hey, how are you?";

        inf_text_token_vector tokVec;

        emdProcessor->tokenize_input(inputString.c_str(), inputString.size(), tokVec);
        std::cout << tokVec.size() << std::endl;
        emdProcessor->execute_input(tokVec);
    }

    GENERIC on_embedding_data(const F32* out_data, size_type out_size) override
    {
        for(size_type i = 0; i < out_size; i++)
        {
            printf("%f, ", out_data[i]);
        }
        printf("\n\n");
        InfProcessorBase* hostProcessor = NULL;
        get_host_processor(hostProcessor);
        InfEmbedderProcessor* emdProcessor = static_cast<InfEmbedderProcessor*>(hostProcessor);
        mbase::string inputString = "Hey, how are you?";

        inf_text_token_vector tokVec;

        emdProcessor->tokenize_input(inputString.c_str(), inputString.size(), tokVec);
        std::cout << tokVec.size() << std::endl;
        emdProcessor->execute_input(tokVec);

    }

    GENERIC on_write(CBYTEBUFFER out_data, size_type out_size, inf_text_token out_token, bool out_is_special, bool out_is_finish) override 
    {        
        fflush(stdout);
        //printf("%s", out_data);
        totalMessage += out_data;
        InfProcessorBase* procBase;
        get_host_processor(procBase);
        
        InfTextToTextProcessor* txtOut = static_cast<InfTextToTextProcessor*>(procBase);

        txtOut->next();
    }

    GENERIC on_finish(size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override 
    {
        
    }
    
    GENERIC on_unregister() override 
    {
        
    }
private:
    mbase::vector<U32> msgIds;
    mbase::string totalMessage;
};

class my_context : public mbase::InfEmbedderProcessor {
public:
    GENERIC on_initializing() override
    {
        std::cout << "WE ARE INITIALZINING!!" << std::endl;
    }

    GENERIC on_initialize_fail(init_fail_code out_code) override
    {
        std::cout << "INITIALIZE FAILED ;(" << std::endl;
    }

    GENERIC on_initialize() override
    {
        std::cout << "eeeee" << std::endl;
        this->set_inference_client(&myCl);
    }

    GENERIC on_destroy() override
    {
        std::cout << "I am dead" << std::endl;
    }
private:
    my_client myCl;
};

class my_model : public mbase::InfModelTextToText {
public:
    void on_initialize() override 
    {
        
    }
    void on_destroy() override 
    {
        std::cout << "Model is destroyed" << std::endl;
    }
private:
};

using namespace mbase;

int main()
{
    mbase::maip_client mm;
    mbase::string myOutput;

    mm.access_request("sql_agent", "12345678", myOutput);

    std::cout << myOutput << std::endl;

    mbase::InfMaipModelDescription modelDescription;

    modelDescription.load_from_state_file("cuser.mbsf", L"./");

    // modelDescription.set_custom_name("cuser");
    // modelDescription.set_embedding(false);
    // modelDescription.set_maximum_context_length(12000);
    // modelDescription.set_original_name("Qwen 2.5 Instruct");
    // modelDescription.set_tags({"SQL"});

    // modelDescription.update_state_file(L"./");
    // modelDescription.update_state_file(L"./", true);

    // mbase::InfProgram ifp;
    // mbase::InfMaipDefaultServer ids(ifp);
    // PcNetManager pcn;
    // pcn.create_server("127.0.0.1", 4553, ids);
    // ids.listen();
    // while(1)
    // {
    //     ids.update();
    // }

    /*mbase::string customName = modelDescription.get_custom_name();
    mbase::string description = modelDescription.get_description();
    bool isEmbedder = modelDescription.get_embedding();
    U32 maxContextLength = modelDescription.get_maximum_context_length();
    mbase::string modelFile = modelDescription.get_model_file();
    mbase::string originalName = modelDescription.get_original_name();
    mbase::string systemPrompt = modelDescription.get_system_prompt();
    mbase::vector<mbase::string> tags = modelDescription.get_tags();

    std::cout << "Custom name: " << customName << std::endl;
    std::cout << "Description: " << description << std::endl;
    std::cout << "Is embedder: " << isEmbedder << std::endl;
    std::cout << "Max context length: " << maxContextLength << std::endl;
    std::cout << "Model file: " << modelFile << std::endl;
    std::cout << "Original name: " << originalName << std::endl;
    std::cout << "System prompt: " << systemPrompt << std::endl;
    std::cout << "Tags: " << std::endl;
    for(auto& n : tags)
    {
        std::cout << "- " << n << std::endl; 
    }*/

    /*InfProgram mainProgram;
    mbase::InfProgramInformation programInformation;

    programInformation.mProgramInformation.mMbaseVersion = "1.0.0";
    programInformation.mProgramInformation.mProductAuthor = "Mustafa Emre Erdog";
    programInformation.mProgramInformation.mProductId = "12345";
    programInformation.mProgramInformation.mProductLicence = "MIT";
    programInformation.mProgramInformation.mProductName = "MBASE Inference Engine";
    programInformation.mProgramInformation.mProductVersion = "0.1.0";
    programInformation.mProgramInformation.mVersionMajor = 0;
    programInformation.mProgramInformation.mVersionMinor = 1;
    programInformation.mProgramInformation.mVersionPatch = 0;

    programInformation.mDataPath = L"./";
    programInformation.mExecutionPath = L"./";
    programInformation.mTempPath = L"./";

    mainProgram.initialize(programInformation);
    mbase::string outAccToken;
    mainProgram.create_user("admin", 9, 4096, true, false, "yumniye99", MAIP_ADAPTER_LOAD_UNLOAD, true, outAccToken);
    mainProgram.create_user("emreerdog", 3, 4096, false, false, "159753", MAIP_MODEL_LOAD_UNLOAD | MAIP_ADAPTER_LOAD_UNLOAD, true, outAccToken);
    mainProgram.create_user("qwen2-tooler", 1, 4096, false, false, "1df215a571", MAIP_MODEL_LOAD_UNLOAD | MAIP_ADAPTER_LOAD_UNLOAD, true, outAccToken);
    mainProgram.create_user("llama3", 5, 4096, false, false, "yumniye99", MAIP_MODEL_LOAD_UNLOAD | MAIP_ADAPTER_LOAD_UNLOAD, true, outAccToken);

    mainProgram.authorize_user_on_model("emreerdog", "qwen2-7b-instruct-q3_k_m.gguf");

    mbase::InfMaipDefaultServer defaultMaipServer(mainProgram);
    mainProgram.get_net_manager()->create_server("127.0.0.1", 4553, defaultMaipServer);
    defaultMaipServer.listen();
    while(1)
    {
        defaultMaipServer.update();
        mainProgram.update();
    }*/

    getchar();
    return 0;
}