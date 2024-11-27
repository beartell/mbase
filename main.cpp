#include <iostream>
#include <mbase/mbase_std.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_t2t_client.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_maip_server.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/inference/inf_maip_model_description.h>
#include <mbase/argument_get_value.h>
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

        decodeBehavior.mTokenAtMost = 5;
        decodeBehavior.mHaltOnWrite = true;

        t2tProcessor->tokenize_input(msgArray.data(), msgArray.size(), tokenVector);
        t2tProcessor->execute_input(tokenVector);
        t2tProcessor->next(decodeBehavior);
    }

    GENERIC on_write(InfTextToTextProcessor* out_processor, const inf_text_token_vector& out_token_vector, bool out_is_finish) override
    {        
        InfTextToTextProcessor* txtOut = static_cast<InfTextToTextProcessor*>(out_processor);
        mbase::vector<inf_token_description> tokenDesc;
        txtOut->tokens_to_description_vector(out_token_vector, tokenDesc);

        fflush(stdout);
        
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

    GENERIC on_finish(InfTextToTextProcessor* out_processor, size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override
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
        
        InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(out_processor);
        mbase::vector<mbase::context_line> msgArray;
        get_message_array(msgIds.data(), msgIds.size(), msgArray);
        
        inf_text_token_vector tokenVector;

        decodeBehavior.mTokenAtMost = 1;
        decodeBehavior.mHaltOnWrite = false;

        t2tProcessor->tokenize_input(msgArray.data(), msgArray.size(), tokenVector);
        t2tProcessor->execute_input(tokenVector);
        t2tProcessor->next(decodeBehavior);
    }   
    
    GENERIC on_unregister(InfProcessorBase* out_processor) override
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
    void on_initialize_fail(init_fail_code out_fail_code) override
    {
        
    }
    void on_initialize() override 
    {
        register_context_process(&c1, 16000, 512, 16, true, {});
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

int main(int argc, char** argv)
{
    mbase::InfSamplerDescription topKSampling;
    mbase::InfSamplerDescription topPSampling;
    mbase::InfSamplerDescription minPSampling;
    mbase::InfSamplerDescription tempSampling;
    mbase::InfSamplerDescription mirov2;

    topKSampling.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TOP_K;
    topKSampling.mTopK = 40;

    topPSampling.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TOP_P;
    topPSampling.mTopP = 0.9;

    minPSampling.mSamplerType = mbase::InfSamplerDescription::SAMPLER::MIN_P;
    minPSampling.mMinP = 0.1;

    tempSampling.mSamplerType = mbase::InfSamplerDescription::SAMPLER::TEMP;
    tempSampling.mTemp = 0.1;

    mirov2.mSamplerType = mbase::InfSamplerDescription::SAMPLER::MIROSTAT_V2;
    mirov2.mMiroV2.mTau = 1.0;
    mirov2.mMiroV2.mEta = 0.01;

    //// maipUser.update_state_file(L"./");

    mbase::vector<mbase::string> userNameList = {"admin", "john", "sql_executor", "mustafa", "web_assistant"};
    mbase::vector<mbase::string> acckeyList = {"1234", "chris", "yumniye", "hasankere88", "1df215a571"};
    mbase::vector<U32> batchSizeList = {1024, 1024, 2048, 512, 8192};
    mbase::vector<U32> accLimitList = {10, 3, 4, 3, 3};
    mbase::vector<U32> maxContextLengthList = {16000, 12000, 4096, 4096, 7000};
    mbase::vector<U32> maxProcThreadCount = {16, 16, 8, 4, 4};
    mbase::vector<U32> procThreadCount = {4, 8, 12, 6, 8};

    mbase::InfProgramInformation programInfo;
    programInfo.mConfigPath = L"./";
    programInfo.mDataPath = L"./";
    programInfo.mTempPath = L"./";
    
    mbase::InfProgram instanceProgram;
    instanceProgram.initialize(programInfo);

    for(I32 i = 0; i < 5; i++)
    {
       mbase::string accTok;
       mbase::InfProgram::flags result = instanceProgram.create_user(
           userNameList[i],
           acckeyList[i],
           "",
           MAIP_MODEL_LOAD_UNLOAD,
           accLimitList[i],
           maxContextLengthList[i],
           batchSizeList[i],
           maxProcThreadCount[i],
           procThreadCount[i],
           true,
           true,
           {topKSampling, topPSampling, minPSampling, tempSampling, mirov2},
           accTok
       );
    }
    mbase::string outToken;
    instanceProgram.inf_access_request(
       "admin",
       "1234",
       NULL,
       "T2T",
       outToken
    );
    instanceProgram.inf_create_model_description(
       outToken,
       "llama_model",
       "custom_lama",
       "Website assitant",
       "",
       "Llama3.1.gguf",
       {"SQL", "Web", "Assistant"},
       "T2T",
       false,
       true,
       4096
    );
    
    InfProgram::maip_err_code errCode = instanceProgram.inf_load_model(
        outToken,
        "custom_lama",
        60000
    );

    instanceProgram.inf_modify_model_context_length(
        outToken,
        "custom_lama",
        60000
    );

    instanceProgram.inf_modify_model_model_file(
        outToken,
        "custom_lama",
        "Llama-3.2-1B-Instruct-Q4_0.gguf",
        false,
        "T2T"
    );

    
    // mbase::sleep(2000);
    // instanceProgram.update();

    // instanceProgram.inf_create_context(
    //     outToken,
    //     NULL,
    //     "custom_lama",
    //     4096
    // );
    // mbase::sleep(2000);
    // instanceProgram.update();

    // mbase::vector<U32> messageIds;

    // U32 msgId = 0;
    // instanceProgram.exec_set_input(outToken, 1, mbase::context_role::SYSTEM, "You are a helpful assistant.", msgId);
    // messageIds.push_back(msgId);
    // instanceProgram.exec_set_input(outToken, 1, mbase::context_role::USER, "Give me a 100 word essay about climate change.", msgId);
    // messageIds.push_back(msgId);

    // errCode = instanceProgram.exec_execute_input(outToken, 1, messageIds);

    // while(1)
    // {
    //     //instanceProgram.exec_next(outToken, NULL, 1);
    //     errCode = instanceProgram.exec_next(outToken, NULL, 1);
    //     instanceProgram.update();
    // }

    getchar();
    return 0;
}