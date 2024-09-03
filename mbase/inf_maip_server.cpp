#include <mbase/inference/inf_maip_server.h>
#include <mbase/char_stream.h>
#include <mbase/io_file.h>

MBASE_BEGIN

GENERIC InfMaipServerBase::on_accept(std::shared_ptr<PcNetPeerClient> out_peer)
{

}

GENERIC InfMaipServerBase::on_data(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size)
{
	mbase::string maipRequest(out_data, out_size);
	mbase::char_stream cs(const_cast<IBYTEBUFFER>(out_data), out_size);
	maip_generic_errors tempGenericError = mMaipPeerRequest.parse_request(cs);
	if(tempGenericError == maip_generic_errors::SUCCESS)
	{
		U32 messageContentLength = mMaipPeerRequest.get_content_length();
		if(messageContentLength != 0)
		{
			if(cs.is_cursor_end())
			{
				// means the data is present but it is not included in message, 
				// todo: handle this problem later
			}
			else
			{
				cs.advance();
				tempGenericError = mMaipPeerRequest.parse_data(cs, messageContentLength);
			}
		}

		if(tempGenericError == maip_generic_errors::SUCCESS)
		{
			const maip_request_identification& outIdentification = mMaipPeerRequest.get_identification();

			if (outIdentification.mOpType == MBASE_MAIP_INF_OP_TYPE)
			{
				on_informatic_request(mMaipPeerRequest, out_peer);
			}
			else if (outIdentification.mOpType == MBASE_MAIP_EXEC_OP_TYPE)
			{
				on_execution_request(mMaipPeerRequest, out_peer);
			}
			else
			{
				on_custom_request(mMaipPeerRequest, out_peer);
			}
			return;
		}
	}
	maip_packet_builder tmpPacketBuilder;
	U16 genericErrorCode = (U16)tempGenericError;
	mbase::string outMessage;
	tmpPacketBuilder.set_version(1, 0); // make it customizable
	tmpPacketBuilder.set_response_message(genericErrorCode);
	tmpPacketBuilder.generate_payload(outMessage);

	out_peer->write_data(outMessage.c_str(), outMessage.size());
	out_peer->finish_and_ready();
}

GENERIC InfMaipServerBase::on_disconnect(std::shared_ptr<PcNetPeerClient> out_peer)
{

}

MBASE_END
