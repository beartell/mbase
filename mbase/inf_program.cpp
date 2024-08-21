#include <mbase/inference/inf_program.h>
#include <mbase/maip_parser.h>
#include <random>

MBASE_BEGIN

#define MBASE_SESSION_CONTROL \
if(!this->is_session_match(in_csid, in_clid))\
{\
	return maip_err_code::INF_UNAUTHORIZED_ACCESS;\
}\
InfAcceptedClient& mAccClient = mActiveClients[in_csid];

InfMaipTunedClient::InfMaipTunedClient() : lastToken(), mManagerClient(NULL)
{

}

InfMaipTunedClient::InfMaipTunedClient(InfAcceptedClient& in_client) : mManagerClient(&in_client)
{
}

GENERIC InfMaipTunedClient::on_register() 
{

}

GENERIC InfMaipTunedClient::on_write(CBYTEBUFFER out_data, size_type out_size) 
{
	mbase::string outData(out_data, out_size);
	mbase::maip_packet_builder tmpPacketBuilder;
	if(mFs == finish_state::INF_FINISH_STATE_CONTINUE)
	{
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_CONTINUE);
	}
	else if(mFs == finish_state::INF_FINISH_STATE_SUCCESS)
	{
		// on_finish will be called
		lastToken = outData;
		return;
	}
	else
	{
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_UNKNOWN_STATUS);
	}

	mbase::string outPayload;

	if(outData.size())
	{
		tmpPacketBuilder.generate_payload(outPayload, outData);
	}
	else
	{
		tmpPacketBuilder.generate_payload(outPayload);
	}

	if(!mManagerClient->mPeer->is_connected())
	{
		mManagerClient->mPeer = NULL;
		return;
	}

	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->finish_and_ready();
}

GENERIC InfMaipTunedClient::on_finish(size_type out_total_token_size) 
{
	mbase::maip_packet_builder tmpPacketBuilder;
	if(mFs == finish_state::INF_FINISH_STATE_SUCCESS)
	{
		tmpPacketBuilder.set_kval("TOKCOUNT", out_total_token_size);
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_FINISH);
	}
	else if(mFs == finish_state::INF_FINISH_STATE_TOKEN_LIMIT_REACHED)
	{
		tmpPacketBuilder.set_kval("MAXTOK", mfrMaxTokenCount);
		tmpPacketBuilder.set_kval("TOKCOUNT", out_total_token_size);
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_TOKEN_LIMIT_EXCEEDED);
	}
	else if(mFs == finish_state::INF_FINISH_STATE_ABANDONED)
	{
		tmpPacketBuilder.set_kval("TOKCOUNT", out_total_token_size);
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_ABANDONED);
	}
	else 
	{
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_UNKNOWN_STATUS);
	}

	mbase::string outPayload;

	if(lastToken.size())
	{
		tmpPacketBuilder.generate_payload(outPayload, lastToken);
		lastToken.clear();
	}
	else
	{
		tmpPacketBuilder.generate_payload(outPayload);
	}

	if (!mManagerClient->mPeer->is_connected())
	{
		mManagerClient->mPeer = NULL;
		return;
	}

	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->finish_and_ready();
}

GENERIC InfMaipTunedClient::on_unregister() 
{

}

bool InfProgram::is_session_match(MBASE_MAIP_CL_AUTH)
{
	if(mActiveClients.find(in_csid) == mActiveClients.end())
	{
		return false;
	}

	if(mActiveClients[in_csid].mClid != in_clid)
	{
		return false;
	}

	return true;
}

InfProgram::maip_err_code InfProgram::inf_create_session(const mbase::string& in_clid, U64& out_csid, mbase::string& out_clid)
{
	if(in_clid.size())
	{
		// FIND THE CLID AND RETURN ITS CSID
		for(mbase::unordered_map<U64, InfAcceptedClient>::iterator It = mActiveClients.begin(); It != mActiveClients.end(); ++It)
		{
			if(It->second.mClid == in_clid)
			{
				out_csid = It->first;
				return maip_err_code::INF_SUCCESS;
			}
		}

		return maip_err_code::INF_CLIENT_ID_MISMATCH;
	}

	for(mClientSessionIdCounter; mActiveClients.find(mClientSessionIdCounter) != mActiveClients.end(); ++mClientSessionIdCounter)
	{
	}
	
	out_csid = mClientSessionIdCounter;

	// NOTE: THIS IS A TEMPORARY SOLUTION FOR GENERATING CLIENT IDS
	// COLLISION IS NOT CHECKED AND IT WILL LIKELY HAPPEN
	// TODO: USE UUIDs or something else to assign client ids 
	std::mt19937 randomGenerator(out_csid); // session id is the seed for now
	InfAcceptedClient iac;
	iac.mClid = mbase::to_string(randomGenerator());
	iac.mCsId = out_csid;
	iac.mPeer = NULL;
	mActiveClients[out_csid] = iac;
	++mClientSessionIdCounter;

	out_clid = iac.mClid;

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_destroy_client(MBASE_MAIP_CL_AUTH)
{
	MBASE_SESSION_CONTROL;
	for(auto& activeProcessorMap : mAccClient.mChatSessions)
	{
		InfProcessor* tmpProc = NULL;
		activeProcessorMap.second->get_host_processor(tmpProc);
		if(tmpProc)
		{
			return proc_err_to_maip(tmpProc->unregister_client(*activeProcessorMap.second));
		}
	}
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_acquired_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;
	out_models.clear();
	for(auto& tempModel : mAccClient.mAcceptedModels)
	{
		out_models.push_back(tempModel);
	}
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_created_context_ids(MBASE_MAIP_CL_AUTH, mbase::vector<U64>& out_contexts)
{
	MBASE_SESSION_CONTROL;

	out_contexts.clear();
	for(auto& tmpSessions : mAccClient.mChatSessions)
	{
		out_contexts.push_back(tmpSessions.first);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_create_context(MBASE_MAIP_CL_AUTH, const mbase::string& in_model, const U32& in_ctsize, U64& out_ctxId)
{
	MBASE_SESSION_CONTROL;

	if (std::find(mAccClient.mAcceptedModels.begin(), mAccClient.mAcceptedModels.end(), in_model) == mAccClient.mAcceptedModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	if(mRegisteredModels.find(in_model) == mRegisteredModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	InfMaipTunedClient* currentClient = new InfMaipTunedClient();
	currentClient->mManagerClient = &mAccClient;
	mAccClient.mChatSessions[mAccClient.mChatSessionIdCounter] = currentClient;

	InfModel* tempModel = mRegisteredModels[in_model];
	InfProcessor::flags procErr = InfProcessor::flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;
	for (auto& tmpProc : *tempModel)
	{
		InfProcessor* activeProcessor = tmpProc->mProcessor;
		procErr = activeProcessor->register_client(*mAccClient.mChatSessions[mAccClient.mChatSessionIdCounter], in_ctsize);
		if(procErr == InfProcessor::flags::INF_PROC_SUCCESS)
		{
			out_ctxId = mAccClient.mChatSessionIdCounter;
			++mAccClient.mChatSessionIdCounter;
			return maip_err_code::INF_SUCCESS;
		}
	}

	return proc_err_to_maip(procErr);
}

InfProgram::maip_err_code InfProgram::inf_destroy_context(MBASE_MAIP_CL_AUTH, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	if(mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient& mySession = *mAccClient.mChatSessions[in_ctxId];
	InfProcessor* myProcessor = NULL;
	InfClient::flags clientResultFlag = mySession.get_host_processor(myProcessor);
	if(clientResultFlag != InfClient::flags::INF_CLIENT_SUCCESS)
	{
		return client_err_to_maip(clientResultFlag);
	}

	return proc_err_to_maip(myProcessor->unregister_client(mySession));
}

InfProgram::maip_err_code InfProgram::inf_acquire_model(MBASE_MAIP_CL_AUTH, const mbase::string& in_model)
{
	MBASE_SESSION_CONTROL;

	if (mRegisteredModels.find(in_model) == mRegisteredModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}
	
	if (std::find(mAccClient.mAcceptedModels.begin(), mAccClient.mAcceptedModels.end(), in_model) == mAccClient.mAcceptedModels.end())
	{
		// push the model if not exists
		mAccClient.mAcceptedModels.push_back(in_model);
	}
	
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_release_model(MBASE_MAIP_CL_AUTH, const mbase::string& in_model)
{
	MBASE_SESSION_CONTROL;

	mbase::vector<mbase::string>::iterator resultIt = std::find(mAccClient.mAcceptedModels.begin(), mAccClient.mAcceptedModels.end(), in_model);
	if (resultIt == mAccClient.mAcceptedModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}
	mAccClient.mAcceptedModels.erase(resultIt);
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;
	out_models.clear();
	out_models = mAccClient.mAcceptedModels;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_model_params(MBASE_MAIP_CL_AUTH, const mbase::string& in_model)
{
	MBASE_SESSION_CONTROL;

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_program_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;

	for(auto& tmpModel : mRegisteredModels)
	{
		out_models.push_back(tmpModel.first);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::exec_set_input(MBASE_MAIP_CL_AUTH, const U64& in_ctxId, InfClient::input_role in_role, const mbase::string& in_input, U32& out_msgid)
{
	MBASE_SESSION_CONTROL;

	if(mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient& mySession = *mAccClient.mChatSessions[in_ctxId];
	InfClient::flags errCode = mySession.set_input(in_input, in_role, out_msgid);
	if(errCode != InfClient::flags::INF_CLIENT_SUCCESS)
	{
		return client_err_to_maip(errCode);
	}

	return maip_err_code::EXEC_SUCCESS;
}

InfProgram::maip_err_code InfProgram::exec_execute_input(MBASE_MAIP_CL_AUTH, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const U64& in_ctxId, const mbase::vector<U32>& in_msgid)
{
	MBASE_SESSION_CONTROL;
	if (mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient& mySession = *mAccClient.mChatSessions[in_ctxId];
	mAccClient.mPeer = in_peer;
	InfClient::flags errCode = mySession.execute_prompt(in_msgid);

	return client_err_to_maip(errCode);
}

InfProgram::maip_err_code InfProgram::exec_next(MBASE_MAIP_CL_AUTH, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	if (mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient& mySession = *mAccClient.mChatSessions[in_ctxId];
	if(!mySession.is_registered())
	{
		// TODO: re-register to a processor
	}

	mAccClient.mPeer = in_peer;
	mySession.next();

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::exec_terminate_generation(MBASE_MAIP_CL_AUTH, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	return maip_err_code::EXEC_SUCCESS;
}

InfProgram::flags InfProgram::host_model(InfModel& in_model)
{
	if(!in_model.is_initialized())
	{
		return flags::INF_PROGRAM_ERR_MODEL_IS_NOT_INITIALIZED;
	}

	mbase::string currentModelName;
	in_model.get_model_name(currentModelName); // 100% success

	for (auto& tmpModels : mRegisteredModels) 
	{
		if(tmpModels.first == currentModelName)
		{
			return flags::INF_PROGRAM_ERR_MODEL_ALREADY_BEING_HOSTED;
		}
	}

	mRegisteredModels[currentModelName] = &in_model;
	return flags::INF_PROGRAM_SUCCESS;
}

InfProgram::flags InfProgram::release_model(const mbase::string& in_model_name)
{
	if(mRegisteredModels.find(in_model_name) == mRegisteredModels.end())
	{
		return flags::INF_PROGRAM_ERR_MODEL_NAME_MISMATCH;
	}
	mRegisteredModels.erase(in_model_name);
	return flags::INF_PROGRAM_SUCCESS;
}

InfProgram::maip_err_code InfProgram::proc_err_to_maip(InfProcessor::flags in_flag)
{
	switch (in_flag)
	{
	case mbase::InfProcessor::flags::INF_PROC_SUCCESS:
		return maip_err_code::INF_SUCCESS;
		break;
	case mbase::InfProcessor::flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR:
		return maip_err_code::INF_UNABLE_TO_FIND_SUITABLE_PROCESSOR;
		break;
	case mbase::InfProcessor::flags::INF_PROC_ERR_ALREADY_PROCESSING:
		return maip_err_code::INF_SUCCESS; // change it into already processing
		break;
	case mbase::InfProcessor::flags::INF_PROC_ERR_CLIENT_ALREADY_REGISTERED:
		return maip_err_code::INF_SUCCESS; // change it into already registered
		break;
	case mbase::InfProcessor::flags::INF_PROC_ERR_MODEL_IS_NOT_INITIALIZED:
		return maip_err_code::INF_SUCCESS; // change it into model is halted
		break;
	case mbase::InfProcessor::flags::INF_PROC_ERR_HALTED:
		return maip_err_code::INF_PROCESSOR_UNAVAILABLE;
		break;
	case mbase::InfProcessor::flags::INF_PROC_ERR_CLIENT_LIMIT_REACHED:
		return maip_err_code::INF_UNABLE_TO_FIND_SUITABLE_PROCESSOR;
		break;
	case mbase::InfProcessor::flags::INF_PROC_ERR_EXCEED_TOKEN_LIMIT:
		return maip_err_code::INF_SUCCESS; // change it into given token is bigger than context batch size
		break;
	case mbase::InfProcessor::flags::INF_PROC_ERR_CONTEXT_IS_FULL:
		return maip_err_code::INF_SUCCESS; // change it into processor context is full
		break;
	default:
		return maip_err_code::INF_UNKNOWN_STATUS;
		break;
	}
}

InfProgram::maip_err_code InfProgram::client_err_to_maip(InfClient::flags in_flag)
{
	switch (in_flag)
	{
	case mbase::InfClient::flags::INF_CLIENT_SUCCESS:
		return maip_err_code::INF_SUCCESS;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_NOT_REGISTERED:
		return maip_err_code::INF_CLIENT_NOT_REGISTERED;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_PROCESSING:
		return maip_err_code::EXEC_PROCESSING;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_MSG_ID_MISMATCH:
		return maip_err_code::EXEC_MESSAGE_ID_MISMATCH;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_MISSING_CHAT:
		return maip_err_code::EXEC_MISSING_MESSAGE;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_TOKENIZATION_FAILED:
		return maip_err_code::EXEC_TOKENIZATION_FAILED;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_TOKEN_LIMIT_EXCEEDED:
		return maip_err_code::EXEC_TOKEN_LIMIT_EXCEEDED;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_UNKNOWN:
		return maip_err_code::INF_UNKNOWN_STATUS;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_UNREGISTERATION_IN_PROGRESS:
		return maip_err_code::INF_CLIENT_UNREGISTERING;
		break;
	case mbase::InfClient::flags::INF_CLIENT_ERR_MISSING_PROCESSOR:
		return maip_err_code::INF_UNABLE_TO_FIND_SUITABLE_PROCESSOR;
		break;
	default:
		return maip_err_code::INF_UNKNOWN_STATUS;
		break;
	}
	return maip_err_code::INF_SUCCESS;
}

MBASE_END
