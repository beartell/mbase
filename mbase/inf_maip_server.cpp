#include <mbase/inference/inf_maip_server.h>
#include <mbase/inference/inf_maip_callbacks.h>
#include <mbase/char_stream.h>
#include <mbase/io_file.h>
#include <set>

MBASE_BEGIN

GENERIC InfMaipServerBase::on_accept(std::shared_ptr<PcNetPeerClient> out_peer)
{
	std::cout << "Connection received" << std::endl;
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
	std::cout << "Connection left" << std::endl;
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
	if(cbIt == mRequestCbMap.end())
	{
		std::cout << "Operation name mismatch." << std::endl;
		return;
	}
	else
	{
		maip_packet_builder packetBuilder;
		packetBuilder.set_version(1, 0); // TODO: Get the version from macros
		if(!cbIt->second(out_peer, out_request, out_request.get_kval<mbase::string>("STOK"), packetBuilder))
		{
			// If this is the case, the underlying operation is responsible for network operations for the given client
			return;
		}
		else
		{
			mbase::string outPayload;
			packetBuilder.generate_payload(outPayload);
			out_peer->write_data(outPayload.c_str(), outPayload.size());
			out_peer->send_write_signal();
			out_peer->send_read_signal();
		}
		return;
	}

	if(requestString == "inf_access_request")
	{
		mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
		mbase::string accessToken = out_request.get_kval<mbase::string>("ACCTOKEN");
		mbase::string outToken;
		InfProgram::maip_err_code merr = mHostProgram->inf_access_request(userName, accessToken, out_peer, outToken);

		mbase::string outPayload;
		mbase::maip_packet_builder maipPacketBuilder;
		if(merr == InfProgram::maip_err_code::INF_SUCCESS)
		{
			maipPacketBuilder.set_kval("STOK", outToken);
		}
		maipPacketBuilder.set_response_message((U16)merr);
		maipPacketBuilder.generate_payload(outPayload);

		out_peer->write_data(outPayload.c_str(), outPayload.size());
		out_peer->send_write_signal();
		out_peer->send_read_signal();
		return;
	}
	// Besides access request, access token(STOK) must be supplied

	if(!out_request.has_key("STOK"))
	{
		// Do not even go further
		mbase::string outPayload;
		mbase::maip_packet_builder maipPacketBuilder;
		mbase::InfProgram::maip_err_code maipErr = mbase::InfProgram::maip_err_code::INF_SESSION_TOKEN_MISMATCH;
		maipPacketBuilder.set_response_message((U16)maipErr);
		maipPacketBuilder.generate_payload(outPayload);

		out_peer->write_data(outPayload.c_str(), outPayload.size());
		out_peer->send_write_signal();
		out_peer->send_read_signal();
		return;
	}

	else
	{
		mbase::string sessionToken = out_request.get_kval<mbase::string>("STOK");
		mbase::maip_packet_builder maipPacketBuilder;
		mbase::InfProgram::maip_err_code maipErr;

		if (requestString == "inf_destroy_session")
		{
			maipErr = mHostProgram->inf_destroy_session(sessionToken);
		}

		else if (requestString == "inf_get_accessible_models")
		{
			mbase::vector<mbase::string> outModels;
			maipErr = mHostProgram->inf_get_accessible_models(sessionToken, outModels);
			for (mbase::vector<mbase::string>::iterator It = outModels.begin(); It != outModels.end(); ++It)
			{
				maipPacketBuilder.set_kval("MODEL", *It);
			}
		}

		else if (requestString == "inf_get_context_ids")
		{
			mbase::vector<U64> outContextIds;
			maipErr = mHostProgram->inf_get_context_ids(sessionToken, outContextIds);
			for(mbase::vector<U64>::iterator It = outContextIds.begin(); It != outContextIds.end(); ++It)
			{
				maipPacketBuilder.set_kval("CTXID", *It);
			}
		}

		else if(requestString == "inf_create_context")
		{
			// WILL ACQUIRE THE SOCKET ON SUCCESS
			mbase::string targetModel = out_request.get_kval<mbase::string>("MODEL");
			U32 contextSize = out_request.get_kval<U32>("CTXSIZE");
			maipErr = mHostProgram->inf_create_context(sessionToken, out_peer, targetModel, contextSize);
			if(maipErr == mbase::InfProgram::maip_err_code::INF_SUCCESS)
			{
				return;
			}
		}

		else if(requestString == "inf_clear_context_history")
		{
			U32 contextId = out_request.get_kval<U32>("CTXID");
			maipErr = mHostProgram->inf_clear_context_history(sessionToken, contextId);
		}

		else if(requestString == "inf_get_context_status")
		{
			U32 contextId = out_request.get_kval<U32>("CTXID");
			maipErr = mHostProgram->inf_get_context_status(sessionToken, contextId);
		}

		else if(requestString == "inf_destroy_context")
		{
			U32 contextId = out_request.get_kval<U32>("CTXID");
			maipErr = mHostProgram->inf_destroy_context(sessionToken, contextId);
		}

		else if(requestString == "inf_get_program_models")
		{
			mbase::vector<mbase::string> outModels;
			maipErr = mHostProgram->inf_get_program_models(sessionToken, outModels);
			for (mbase::vector<mbase::string>::iterator It = outModels.begin(); It != outModels.end(); ++It)
			{
				maipPacketBuilder.set_kval("MODEL", *It);
			}
		}

		else if(requestString == "inf_load_model")
		{
			mbase::string modelName = out_request.get_kval<mbase::string>("MODEL");
			U32 contextSize = out_request.get_kval<U32>("CTXSIZE");
			maipErr = mHostProgram->inf_load_model(sessionToken, modelName, contextSize);
		}

		else if(requestString == "inf_unload_model")
		{
			mbase::string modelName = out_request.get_kval<mbase::string>("MODEL");
			maipErr = mHostProgram->inf_unload_model(sessionToken, modelName);
		}

		else if(requestString == "inf_create_new_user")
		{
			// mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			// U32 modelAccessLimit = out_request.get_kval<U32>("ACCESSLIMIT");
			// U32 maximumContextLength = out_request.get_kval<U32>("CTXSIZE");
			// bool isSuperUser = out_request.get_kval<bool>("ISSUPER");
			// mbase::string userAccessToken = out_request.get_kval<mbase::string>("ACCTOKEN");
			// mbase::vector<mbase::string> authorityFlags = out_request.get_kval<mbase::vector<mbase::string>>("AUTH");

			// mbase::string outAccessToken;

			// maipErr = mHostProgram->inf_create_new_user(
			// 	sessionToken,
			// 	userName,
			// 	modelAccessLimit,
			// 	maximumContextLength,
			// 	isSuperUser,
			// 	false,
			// 	userAccessToken,
			// 	authorityFlags,
			// 	outAccessToken
			// );

			// if(maipErr == mbase::InfProgram::maip_err_code::INF_SUCCESS)
			// {
			// 	maipPacketBuilder.set_kval("ACCTOKEN", outAccessToken);
			// }
		}

		else if(requestString == "inf_delete_user")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			//maipErr = mHostProgram->inf_delete_user(sessionToken, userName);
		}
		
		else if(requestString == "inf_modify_user_model_access_limit")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			U32 newAccessLimit = out_request.get_kval<U32>("ACCLIMIT");
			maipErr = mHostProgram->inf_modify_user_model_access_limit(sessionToken, userName, newAccessLimit);
		}

		else if(requestString == "inf_modify_user_maximum_context_length")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			U32 newContextLength = out_request.get_kval<U32>("CTXLENGTH");
			maipErr = mHostProgram->inf_modify_user_maximum_context_length(sessionToken, userName, newContextLength);
		}

		else if(requestString == "inf_modify_user_batch_size")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			U32 newBatchSize = out_request.get_kval<U32>("BATCH");
			maipErr = mHostProgram->inf_modify_user_batch_size(sessionToken, userName, newBatchSize);
		}

		else if(requestString == "inf_modify_user_processor_thread_count")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			U32 newProcessorCount = out_request.get_kval<U32>("PROCCOUNT");
			maipErr = mHostProgram->inf_modify_user_processor_thread_count(sessionToken, userName, newProcessorCount);
		}

		else if(requestString == "inf_modify_user_max_processor_thread_count")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			U32 newMaxProcessorCount = out_request.get_kval<U32>("PROCMAXCOUNT");
			maipErr = mHostProgram->inf_modify_user_max_processor_thread_count(sessionToken, userName, newMaxProcessorCount);
		}

		// else if(requestString == "inf_modify_user_sampling_set")
		// {
		//		mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
		// }

		else if(requestString == "inf_modify_user_system_prompt")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			const mbase::char_stream& cs = out_request.get_data();
			mbase::string newSystemPrompt(out_request.get_data().get_buffer(), out_request.get_content_length());
			maipErr = mHostProgram->inf_modify_user_system_prompt(sessionToken, userName, newSystemPrompt);
		}

		else if(requestString == "inf_modify_user_make_superuser")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			mbase::string userAccessToken = out_request.get_kval<mbase::string>("ACCTOKEN");
			maipErr = mHostProgram->inf_modify_user_make_superuser(sessionToken, userName, userAccessToken);
		}

		else if(requestString == "inf_modify_user_unmake_superuser")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			mbase::string userAccessToken = out_request.get_kval<mbase::string>("ACCTOKEN");
			maipErr = mHostProgram->inf_modify_user_unmake_superuser(sessionToken, userName, userAccessToken);
		}

		else if(requestString == "inf_modify_user_accept_models")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			mbase::vector<mbase::string> acceptedModels = out_request.get_kval<mbase::vector<mbase::string>>("MODEL");
			std::set<mbase::string> modelSet(acceptedModels.begin(), acceptedModels.end());
			
			mbase::vector<mbase::string> missingModels;
			maipErr = mHostProgram->inf_modify_user_accept_models(sessionToken, userName, modelSet, missingModels);
			if(maipErr == mbase::InfProgram::maip_err_code::INF_MODEL_NAME_MISMATCH)
			{
				for(mbase::vector<mbase::string>::const_iterator cIt = missingModels.cbegin(); cIt != missingModels.cend(); ++cIt)
				{
					maipPacketBuilder.set_kval("MODEL", *cIt);	
				}
			}
		}

		else if(requestString == "inf_modify_user_set_authority_flags")
		{
			mbase::string userName = out_request.get_kval<mbase::string>("USERNAME");
			mbase::vector<mbase::string> authorityFlagList = out_request.get_kval<mbase::vector<mbase::string>>("AUTHORITY");
			maipErr = mHostProgram->inf_modify_user_set_authority_flags(sessionToken, userName, authorityFlagList);
		}

		else
		{
			
		}
		mbase::string outPayload;

		maipPacketBuilder.set_response_message((U16)maipErr);
		maipPacketBuilder.generate_payload(outPayload);
		out_peer->write_data(outPayload.c_str(), outPayload.size());
		out_peer->send_write_signal();
		out_peer->send_read_signal();
	}
}

GENERIC InfMaipDefaultServer::on_execution_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer)
{
	const mbase::string& requestString = out_request.get_identification().mOpString;
	mbase::string sessionToken = out_request.get_kval<mbase::string>("STOK");
	U64 contextId = out_request.get_kval<U64>("CTXID");
	mbase::maip_packet_builder maipPacketBuilder;
	mbase::InfProgram::maip_err_code maipErr;

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

GENERIC InfMaipDefaultServer::on_custom_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer)
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
