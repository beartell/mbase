#include <mbase/inference/inf_maip_peer_t2t.h>
#include <mbase/inference/inf_program.h>
#include <mbase/maip_parser.h>

MBASE_BEGIN

InfMaipTextToTextProcessor::InfMaipTextToTextProcessor(InfMaipPeerTextToText* in_peer) : mClientNominee(in_peer)
{

}

GENERIC InfMaipTextToTextProcessor::on_initialize_fail(last_fail_code out_code)
{
    mClientNominee->on_init_fail(this, out_code);
}

GENERIC InfMaipTextToTextProcessor::on_initialize()
{
    set_inference_client(mClientNominee);
}

GENERIC InfMaipTextToTextProcessor::on_destroy()
{
    delete this;
}

InfMaipPeerTextToText::InfMaipPeerTextToText(std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_maip_user) : 
    InfMaipPeerBase(in_peer, in_maip_user, inf_model_category::TEXT_TO_TEXT)
{

}

InfMaipPeerTextToText::~InfMaipPeerTextToText()
{
    mandatory_processor_cleanup(); // Must be called
}

GENERIC InfMaipPeerTextToText::on_init_fail(InfProcessorBase* out_processor, [[maybe_unused]] InfProcessorTextToText::last_fail_code out_fail_code)
{
    if(mPeer->is_connected())
    {
        mbase::maip_packet_builder tmpPacketBuilder;
        mbase::string outPayload;
        tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_FAILED_TO_CREATE_CONTEXT);
        tmpPacketBuilder.generate_payload(outPayload);
        mPeer->write_data(outPayload.c_str(), outPayload.size());
        mPeer->send_write_signal();
        mPeer->send_read_signal();
    }
    delete out_processor;
}

GENERIC InfMaipPeerTextToText::on_register(InfProcessorBase* out_processor)
{
    // Called if the processor is initialized and client is registered into it
    U64 outProcId;
    add_processor(out_processor, outProcId);

    if(mPeer->is_connected())
    {
        mbase::maip_packet_builder tmpPacketBuilder;
        mbase::string outPayload;
        tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_SUCCESS);
        tmpPacketBuilder.set_kval("CTXID", outProcId);
        tmpPacketBuilder.generate_payload(outPayload);
        mPeer->write_data(outPayload.c_str(), outPayload.size());
        mPeer->send_write_signal();
        mPeer->send_read_signal();
    }
}

GENERIC InfMaipPeerTextToText::on_unregister(InfProcessorBase* out_processor)
{  
    // Called if the processor being destroyed
    remove_processor_by_address(out_processor);
    delete out_processor;
}

GENERIC InfMaipPeerTextToText::on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish)
{
    // Called every time a next token is generated
    inf_token_description tokenDescription;
    out_processor->token_to_description(out_token[0], tokenDescription);
    out_processor->next({1, false});
    if(out_is_finish)
    {
        mLastToken = tokenDescription;
        // After here, on_finish will be called on the next update
        return;
    }

    if(mPeer->is_connected())
    {
        mbase::maip_packet_builder tmpPacketBuilder;
        mbase::string outPayload;
        if(tokenDescription.mIsSpecial)
        {
            tmpPacketBuilder.set_kval("SPECIAL", 1);
        }
        else
        {
            tmpPacketBuilder.set_kval("SPECIAL", 0);
        }

        tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_CONTINUE);
        tmpPacketBuilder.generate_payload(outPayload, tokenDescription.mTokenString);
        mPeer->write_data(outPayload.c_str(), outPayload.size());
        mPeer->send_write_signal();
        mPeer->send_read_signal();
    }
}

GENERIC InfMaipPeerTextToText::on_finish([[maybe_unused]] InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state)
{
    // Called if the token generation is finished for a reason stated in argument out_finish_state
    if(mPeer->is_connected())
    {
        mbase::maip_packet_builder tmpPacketBuilder;
        mbase::string outPayload;
        if(mLastToken.mIsSpecial)
        {
            tmpPacketBuilder.set_kval("SPECIAL", 1);
        }

        else
        {
            tmpPacketBuilder.set_kval("SPECIAL", 0);
        }

        InfProgram::maip_err_code finishCode = InfProgram::maip_err_code::EXEC_MESSAGE_FINISH;
        if(out_finish_state == InfProcessorTextToText::finish_state::FINISHED)
        {
        }

        else if(out_finish_state == InfProcessorTextToText::finish_state::TOKEN_LIMIT_REACHED)
        {
            finishCode = InfProgram::maip_err_code::EXEC_TOKEN_LIMIT_EXCEEDED;
        }

        else if(out_finish_state == InfProcessorTextToText::finish_state::ABANDONED)
        {
            finishCode = InfProgram::maip_err_code::EXEC_ABANDONED;
        }
        tmpPacketBuilder.set_kval("TOTAL", out_total_token_size);
        tmpPacketBuilder.set_response_message((U16)finishCode);
        tmpPacketBuilder.generate_payload(outPayload, mLastToken.mTokenString);
        mPeer->write_data(outPayload.c_str(), outPayload.size());
        mPeer->send_write_signal();
        mPeer->send_read_signal();
    }
}

MBASE_END