#include <mbase/inference/inf_maip_server.h>
#include <mbase/char_stream.h>
#include <mbase/io_file.h>

MBASE_BEGIN

GENERIC InfMaipServerBase::on_accept(std::shared_ptr<PcNetPeerClient> out_peer)
{

}

GENERIC InfMaipServerBase::on_data(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size)
{
	accumulation_map::iterator It = mAccumulationMap.find(out_peer->get_raw_socket());
	if(It != mAccumulationMap.end())
	{
		accumulated_processing(out_peer, It, out_data, out_size);
	}
	else
	{
		simple_processing(out_peer, out_data, out_size);
	}
}

GENERIC InfMaipServerBase::on_disconnect(std::shared_ptr<PcNetPeerClient> out_peer)
{

}

GENERIC InfMaipServerBase::accumulated_processing(std::shared_ptr<PcNetPeerClient> out_peer, accumulation_map::iterator in_accum_iterator, CBYTEBUFFER out_data, size_type out_size)
{
	InfMaipPacketAccumulator& in_accumulator = in_accum_iterator->second;
	mbase::string& accumulatedData = in_accumulator.mAccumulatedData;
	if(out_size)
	{
		size_type dataToWrite = out_size;
		if(accumulatedData.size() + out_size > in_accumulator.mDataToRead)
		{
			dataToWrite -= (accumulatedData.size() + out_size) - in_accumulator.mDataToRead; // DISCARD THE REST
		}
		accumulatedData.append(out_data, dataToWrite);
		if(accumulatedData.size() == in_accumulator.mDataToRead)
		{
			maip_peer_request& mMaipPeerRequest = in_accumulator.mPeerRequest;
			const maip_request_identification& outIdentification = mMaipPeerRequest.get_identification();
			mbase::char_stream dataStream(accumulatedData.data(), accumulatedData.size());
			mMaipPeerRequest.set_external_data(dataStream);
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
			mAccumulationMap.erase(in_accum_iterator);
			return;
		}
		out_peer->send_read_signal();

	}
}

GENERIC InfMaipServerBase::simple_processing(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size)
{
	mbase::string maipRequest(out_data, out_size);
	mbase::char_stream cs(const_cast<IBYTEBUFFER>(out_data), out_size);

	maip_peer_request mMaipPeerRequest;
	maip_generic_errors tempGenericError = mMaipPeerRequest.parse_request(cs);
	if (tempGenericError == maip_generic_errors::SUCCESS)
	{
		U32 messageContentLength = mMaipPeerRequest.get_content_length();
		if (messageContentLength != 0)
		{
			//cs.advance();
			U32 messageSize = cs.get_difference();
			if (messageSize >= messageContentLength) // rest of the message will be discarded
			{
				mbase::char_stream charStream(cs.get_bufferc(), messageSize);
				tempGenericError = mMaipPeerRequest.parse_data(charStream);
			}
			else
			{
				// means we will accumulate
				if (!mMaipPeerRequest.has_key("CSID") || !mMaipPeerRequest.has_key("CLID"))
				{
					tempGenericError = maip_generic_errors::MISSING_MANDATORY_KEYS;
				}

				else
				{
					// TODO: CHECK IF THE MESSAGE CONTENT LENGTH IS BIGGER THAN 20 MB
					// IF IT IS, RETURN PACKET TOO LARGE
					U64 csId = mMaipPeerRequest.get_kval<U64>("CSID");
					mbase::string clId = mMaipPeerRequest.get_kval<mbase::string>("CLID");
					mbase::string processedDataLength;
					processedDataLength.reserve(messageContentLength);
					if (messageSize)
					{
						cs.advance();
						processedDataLength.append(cs.get_bufferc(), messageSize - 1);
					}
					mAccumulationMap[out_peer->get_raw_socket()] = { messageContentLength, csId, clId, std::move(processedDataLength), mMaipPeerRequest };
					out_peer->send_read_signal();
					return;
				}
			}
		}
	}

	if (tempGenericError == maip_generic_errors::SUCCESS)
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

	maip_packet_builder tmpPacketBuilder;
	U16 genericErrorCode = (U16)tempGenericError;
	mbase::string outMessage;
	tmpPacketBuilder.set_version(1, 0); // make it customizable
	tmpPacketBuilder.set_response_message(genericErrorCode);
	tmpPacketBuilder.generate_payload(outMessage);

	out_peer->write_data(outMessage.c_str(), outMessage.size());
	out_peer->send_write_signal();
	out_peer->send_read_signal();
}

MBASE_END
