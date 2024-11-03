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
        this->add_message(L"You are a helpful assistant. You are putting the summarization of the user message between tags [SUM_BEGIN] [SUM_END].", context_role::SYSTEM, outMsgId);
        msgIds.push_back(outMsgId);
        this->add_message(L"What is your name?", context_role::USER, outMsgId);
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
        printf(out_data);
        InfTextToTextProcessor* txtOut;
        get_host_processor(txtOut);
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
        InfSamplerDescription mirostat;

        InfSamplingMirostatV2 mirostatValues;
        mirostatValues.mTau = 5.0;
        mirostatValues.mEta = 0.1;

        topK.mTopK = 40;
        topP.mTopP = 0.9;
        minP.mMinP = 0.1;
        mirostat.mMiroV2 = mirostatValues;

        this->register_context_process(
            &myContext, 
            4000, 
            2000, 
            4, 
            4, 
            true,
            {topK, topP, minP, mirostat}
        );
    }
    void on_destroy() override {}
private:
    my_context myContext;
};

using namespace mbase;

int main()
{
    my_model mm;
    mm.initialize_model(L"./Llama-3.2-1B-Instruct-Q4_K_M.gguf", 12000, 999);
    while(1)
    {
        mbase::sleep(30);
        mm.update();
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