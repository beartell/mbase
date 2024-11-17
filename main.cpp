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
#include <mbase/smart_conversion.h>
#include <mbase/maip_client.h>
#include <vector>
#include <set>
#include <unordered_map>
#include <signal.h>
#include <string>

using namespace mbase;

class my_client : public mbase::InfClientTextToText {
public:
    GENERIC on_register(InfProcessorBase* out_processor) override 
    {
        InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(out_processor);
        mbase::string inputString;

        U32 outMsgId = 0;
        this->add_message("You are a helpful assistant.", mbase::context_role::SYSTEM, outMsgId);
        msgIds.push_back(outMsgId);
        std::string prompt;
        std::getline(std::cin, prompt);
        this->add_message(prompt.c_str(), prompt.size(), mbase::context_role::USER, outMsgId);
        msgIds.push_back(outMsgId);

        mbase::vector<mbase::context_line> msgArray;
        get_message_array(msgIds.data(), msgIds.size(), msgArray);
        
        inf_text_token_vector tokenVector;

        decodeBehavior.mTokenAtMost = 1;
        decodeBehavior.mHaltOnWrite = false;

        t2tProcessor->tokenize_input(msgArray.data(), msgArray.size(), tokenVector);
        t2tProcessor->execute_input(tokenVector);
        t2tProcessor->next(decodeBehavior);
    }

    GENERIC on_write(const inf_text_token_vector& out_token_vector, bool out_is_finish) override 
    {        
        InfProcessorBase* procBase;
        get_host_processor(procBase);
        
        InfTextToTextProcessor* txtOut = static_cast<InfTextToTextProcessor*>(procBase);
        fflush(stdout);
        mbase::vector<inf_token_description> tokenDesc;
        txtOut->tokens_to_description_vector(out_token_vector, tokenDesc);
        for(auto& n : tokenDesc)
        {
            if(!n.mIsSpecial)
            {
                totalMessage += n.mTokenString;
            }
            
            printf("%s", n.mTokenString.c_str());
        }
        if(!out_is_finish)
        {
            txtOut->next(decodeBehavior);
        }
        
    }

    GENERIC on_finish(size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override 
    {   
        std::cout << std::endl;
        U32 outMsgId = 0;
        this->add_message(totalMessage, mbase::context_role::ASSISTANT, outMsgId);
        msgIds.push_back(outMsgId);

        std::string prompt;
        std::getline(std::cin, prompt);
        this->add_message(prompt.c_str(), prompt.size(), mbase::context_role::USER, outMsgId);
        msgIds.push_back(outMsgId);

        totalMessage.clear();

        InfProcessorBase* procBase;
        get_host_processor(procBase);
        
        InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(procBase);
        mbase::vector<mbase::context_line> msgArray;
        get_message_array(msgIds.data(), msgIds.size(), msgArray);
        
        inf_text_token_vector tokenVector;

        decodeBehavior.mTokenAtMost = 1;
        decodeBehavior.mHaltOnWrite = false;

        t2tProcessor->tokenize_input(msgArray.data(), msgArray.size(), tokenVector);
        t2tProcessor->execute_input(tokenVector);
        t2tProcessor->next(decodeBehavior);
    }   
    
    GENERIC on_unregister() override 
    {

    }
private:
    decode_behavior_description decodeBehavior;
    mbase::vector<U32> msgIds;
    mbase::string totalMessage;
};

class my_context : public mbase::InfTextToTextProcessor {
public:
    GENERIC on_initializing() override
    {
        std::cout << "WE ARE INITIALZINING!!" << std::endl;
    }

    GENERIC on_initialize_fail(last_fail_code out_code) override
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
        register_context_process(&c1, 12000, 512, 16, true, {});
    }
    void on_destroy() override 
    {
        std::cout << "Model is destroyed" << std::endl;
    }
private:
    my_context c1;
    my_context c2;
    my_context c3;
    my_context c4;
    my_context c5;
    my_context c6;
};

using namespace mbase;

int main()
{
    F32 conversionResult = mbase::smart_conversion<F32>::apply("15");
    printf("%f\n", conversionResult);
    std::cout << conversionResult << std::endl;
    // my_model sampleModel;
    // sampleModel.initialize_model(L"./Llama-3.2-1B-Instruct-Q4_K_M.gguf", 36000, 999);

    // while(1)
    // {
    //     sampleModel.update();
    // }
    
    // InfProgram ifp;
    // InfMaipDefaultServer IDS(ifp);

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