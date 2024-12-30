#include <mbase/inference/inf_maip_server.h>
#include <mbase/inference/inf_maip_callbacks.h>
#include <mbase/char_stream.h>
#include <mbase/io_file.h>
#include <set>

MBASE_BEGIN

GENERIC InfMaipServerBase::on_accept(std::shared_ptr<PcNetPeerClient> out_peer)
{
	out_peer->send_read_signal();
}

GENERIC InfMaipServerBase::on_data(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size)
{
	mbase::string receivedData(out_data, out_size);
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
	// Remove the peer from accumulation map if it exists
	mAccumulationMap.erase(out_peer->get_raw_socket());
}

GENERIC InfMaipServerBase::register_request_callback(const mbase::string& in_operation, maip_request_callback in_callback)
{
	mRequestCbMap[in_operation] = in_callback;
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
				// if the client does not have a session token,
				// accumulation is not allowed.
				if (!mMaipPeerRequest.has_key("STOK"))
				{
					tempGenericError = maip_generic_errors::MISSING_MANDATORY_KEYS;
				}

				else
				{
					// TODO: CHECK IF THE MESSAGE CONTENT LENGTH IS BIGGER THAN 20 MB
					// IF IT IS, RETURN PACKET TOO LARGE
					mbase::string sessionToken = mMaipPeerRequest.get_kval<mbase::string>("STOK");
					mbase::string processedDataLength;
					processedDataLength.reserve(messageContentLength);
					if (messageSize)
					{
						cs.advance();
						processedDataLength.append(cs.get_bufferc(), messageSize - 1);
					}
					mAccumulationMap[out_peer->get_raw_socket()] = { messageContentLength, sessionToken, std::move(processedDataLength), mMaipPeerRequest };
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

InfMaipDefaultServer::InfMaipDefaultServer(InfProgram& in_program)
{
	register_request_callback("inf_access_request", access_request_cb);
	register_request_callback("inf_destroy_session", destroy_session_cb);
	register_request_callback("inf_get_accessible_models", get_accessible_models_cb);
	register_request_callback("inf_get_context_ids", get_context_ids_cb);
	register_request_callback("inf_create_context", create_context_cb);
	register_request_callback("inf_clear_context_history", clear_context_history_cb);
	register_request_callback("inf_get_context_status", get_context_status_cb);
	register_request_callback("inf_destroy_context", destroy_context_cb);
	register_request_callback("inf_get_program_models", get_program_models_cb);
	register_request_callback("inf_load_model", load_model_cb);
	register_request_callback("inf_unload_model", unload_model_cb);
	register_request_callback("inf_create_new_user", create_new_user_cb);
	register_request_callback("inf_delete_user", delete_user_cb);
	register_request_callback("inf_modify_user_model_access_limit", modify_user_model_access_limit_cb);
	register_request_callback("inf_modify_user_maximum_context_length", modify_user_maximum_context_length_cb);
	register_request_callback("inf_modify_user_batch_size", modify_user_batch_size_cb);
	register_request_callback("inf_modify_user_processor_thread_count", modify_user_processor_thread_count_cb);
	register_request_callback("inf_modify_user_max_processor_thread_count", modify_user_max_processor_thread_count_cb);
	register_request_callback("inf_modify_user_system_prompt", modify_user_system_prompt_cb);
	register_request_callback("inf_modify_user_make_superuser", modify_user_make_superuser_cb);
	register_request_callback("inf_modify_user_unmake_superuser", modify_user_unmake_superuser_cb);
	register_request_callback("inf_modify_user_accept_models", modify_user_accept_models_cb);
	register_request_callback("inf_modify_inf_modify_user_set_authority_flags", modify_user_set_authority_flags_cb);
	register_request_callback("inf_create_model_description", create_model_description_cb);
	register_request_callback("inf_modify_original_model_name", modify_model_original_model_name_cb);
	register_request_callback("inf_modify_custom_model_name", modify_model_custom_model_name_cb);
	register_request_callback("inf_modify_model_description", modify_model_description_cb);
	register_request_callback("inf_modify_model_system_prompt", modify_model_system_prompt_cb);
	register_request_callback("inf_modify_model_model_file", modify_model_model_file_cb);
	register_request_callback("inf_modify_model_tags", modify_model_tags_cb);
	register_request_callback("inf_modify_model_context_length", modify_model_context_length_cb);
	register_request_callback("exec_set_input", execution_set_input_cb);
	register_request_callback("exec_execute_input", execution_execute_input_cb);
	register_request_callback("exec_next", execution_next_cb);	

	mHostProgram = &in_program;
}

GENERIC InfMaipDefaultServer::on_informatic_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer)
{
	const mbase::string& requestString = out_request.get_identification().mOpString;
	request_callback_map::iterator cbIt = mRequestCbMap.find(requestString);
	maip_packet_builder packetBuilder;
	packetBuilder.set_version(1, 0); // TODO: Get the version from macros
	
	// TODO: Add timestamp fields to protocol parameters

	if(cbIt == mRequestCbMap.end())
	{
		packetBuilder.set_response_message((U16)maip_generic_errors::UNDEFINED_OP_STRING);
	}
	else
	{
		if(!cbIt->second(*mHostProgram, out_peer, out_request, out_request.get_kval<mbase::string>("STOK"), packetBuilder))
		{
			// If this is the case, the underlying operation is responsible for network operations for the given client
			return;
		}
	}

	mbase::string outPayload;
	packetBuilder.generate_payload(outPayload);
	out_peer->write_data(outPayload.c_str(), outPayload.size());
	out_peer->send_write_signal();
	out_peer->send_read_signal();
}

GENERIC InfMaipDefaultServer::on_execution_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer)
{
	const mbase::string& requestString = out_request.get_identification().mOpString;
	mbase::string sessionToken = out_request.get_kval<mbase::string>("STOK");
	U64 contextId = out_request.get_kval<U64>("CTXID");
	mbase::maip_packet_builder maipPacketBuilder;
	mbase::InfProgram::maip_err_code maipErr = mbase::InfProgram::maip_err_code::EXEC_SUCCESS;

	// All operations must have a session token associated with it

	if(requestString == "exec_set_input")
	{
		mbase::string contextRole = out_request.get_kval<mbase::string>("ROLE");
		//mbase::string givenPrompt(out_request.get_data().get_buffer(), out_request.get_content_length());
		context_role ctxRole = context_role::NONE;

		if(contextRole == "System")
		{
			ctxRole = context_role::SYSTEM;
		}

		else if(contextRole == "Assistant")
		{
			ctxRole = context_role::ASSISTANT;
		}

		else if(contextRole == "User")
		{
			ctxRole = context_role::USER;
		}

		U32 outMsgId;
		maipErr = mHostProgram->exec_set_input(sessionToken, contextId, ctxRole, out_request.get_data().get_buffer(), out_request.get_content_length(), outMsgId);
		if(maipErr == mbase::InfProgram::maip_err_code::INF_SUCCESS)
		{
			maipPacketBuilder.set_kval("MSGID", outMsgId);
		}
	}

	else if(requestString == "exec_execute_input")
	{
		mbase::vector<U32> msgIdVector = out_request.get_kval<mbase::vector<U32>>("MSGID");
		maipErr = mHostProgram->exec_execute_input(sessionToken, contextId, msgIdVector);
	}

	else if(requestString == "exec_next")
	{
		// WILL ACQUIRE THE SOCKET ON SUCCESS
		maipErr = mHostProgram->exec_next(sessionToken, out_peer, contextId);
		if(maipErr == mbase::InfProgram::maip_err_code::EXEC_SUCCESS)
		{
			return;
		}
	}
	mbase::string outPayload;
	maipPacketBuilder.set_response_message((U16)maipErr);
	maipPacketBuilder.generate_payload(outPayload);
	out_peer->write_data(outPayload.c_str(), outPayload.size());
	out_peer->send_write_signal();
	out_peer->send_read_signal();
}

GENERIC InfMaipDefaultServer::on_custom_request([[maybe_unused]] const maip_peer_request& out_request, [[maybe_unused]] std::shared_ptr<PcNetPeerClient> out_peer)
{

}

GENERIC InfMaipDefaultServer::on_listen()
{
	std::cout << "Maip server started listening" << std::endl;
}

GENERIC InfMaipDefaultServer::on_stop()
{

}

MBASE_END
