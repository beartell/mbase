#include <iostream>
#include <mbase/mbase_std.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_maip_server.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <vector>
#include <set>
#include <unordered_map>
#include <signal.h>

using namespace mbase;

class my_client : public mbase::InfClientTextToText {
public:
    GENERIC on_register(InfTextToTextProcessor* out_processor) override 
    {
        std::cout << "Client is registered" << std::endl;
        U32 outMsgId = 0;
        this->add_message(L"You are a helpful assistant.", context_role::SYSTEM, outMsgId);
        msgIds.push_back(outMsgId);
        this->add_message("Give me a 50 word essay about climate change.", context_role::USER, outMsgId);
        msgIds.push_back(outMsgId);
        
        mbase::vector<context_line> txtMessages;
        this->get_message_array(msgIds.data(), msgIds.size(), txtMessages);
        
        InfTextToTextProcessor* txtOut;
        get_host_processor(txtOut);

        InfClientTextToText::token_vector tv;
        txtOut->tokenize_input(txtMessages.data(), txtMessages.size(), tv);
        txtOut->execute_input(tv, true);
        txtOut->next();
    }

    GENERIC on_write(CBYTEBUFFER out_data, size_type out_size, InfTextToTextProcessor::inf_token out_token, bool out_is_special, bool out_is_finish) override 
    {        
        
        totalMessage += out_data;
        InfTextToTextProcessor* txtOut;
        get_host_processor(txtOut);
        txtOut->next();
    }

    GENERIC on_finish(size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override 
    {
        fflush(stdout);
        printf("%s \n\n\n\n", totalMessage.c_str());
    }
    
    GENERIC on_unregister() override 
    {

    }
private:
    mbase::vector<U32> msgIds;
    mbase::string totalMessage;
};

class my_context : public mbase::InfTextToTextProcessor {
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
        InfSamplerDescription topK;
        InfSamplerDescription topP;
        InfSamplerDescription minP;
        InfSamplerDescription temperature;
        InfSamplerDescription mirostat;
        InfSamplerDescription repeatControl;

        topK.mTopK = 40;
        topK.mSamplerType = InfSamplerDescription::SAMPLER::TOP_K;

        topP.mTopP = 0.9;
        topP.mSamplerType = InfSamplerDescription::SAMPLER::TOP_P;

        minP.mMinP = 0.1;
        minP.mSamplerType = InfSamplerDescription::SAMPLER::MIN_P;

        temperature.mTemp = 0.2;
        temperature.mSamplerType = InfSamplerDescription::SAMPLER::TEMP;

        InfSamplingRepetition repeatSampler;
        repeatSampler.mPenaltyLinefeed = true;
        repeatSampler.mRepeatPenalty = 1.5f;
        repeatSampler.mPenaltyFrequency = 1.0f;
        repeatSampler.mPenaltyPresent = 1.0f;

        repeatControl.mSamplerType = InfSamplerDescription::SAMPLER::REPETITION;
        repeatControl.mRepetition = repeatSampler;

        this->register_context_process(
            &myContext, 
            16000, 
            8000, 
            8, 
            4, 
            true,
            {topK, topP, minP, temperature, mirostat}
        );
        this->register_context_process(
            &myContext1, 
            16000, 
            8000, 
            8, 
            4, 
            true,
            {topK, topP, minP, temperature, mirostat}
        );
        this->register_context_process(
            &myContext2, 
            16000, 
            8000, 
            8, 
            4, 
            true,
            {topK, topP, minP, temperature, mirostat}
        );
        this->register_context_process(
            &myContext3, 
            16000, 
            8000, 
            8, 
            4, 
            true,
            {topK, topP, minP, temperature, mirostat}
        );
    }
    void on_destroy() override {}
private:
    my_context myContext;
    my_context myContext1;
    my_context myContext2;
    my_context myContext3;
};

using namespace mbase;

int main()
{
    while(1)
    {
        int i = 0;
        my_model mm;
        mm.initialize_model(L"./Llama-3.2-1B-Instruct-Q4_K_M.gguf", 120000, 999);
        while(1)
        {
            mm.update();
        }
    
    }
   
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