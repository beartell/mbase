#include <mbase/inference/inf_program.h>
#include <random>

MBASE_BEGIN

#define MBASE_SESSION_CONTROL \
if(!this->is_session_match(in_csid, in_clid))\
{\
	return maip_err_code::INF_UNAUTHORIZED_ACCESS;\
}\
InfAcceptedClient& mAccClient = mActiveClients[in_csid];

InfMaipTunedClient::InfMaipTunedClient(InfAcceptedClient & in_client) : mManagerClient(&in_client)
{
}

GENERIC InfMaipTunedClient::on_register() 
{

}

GENERIC InfMaipTunedClient::on_write(CBYTEBUFFER out_data, size_type out_size) 
{
	if(mManagerClient->mPeer->is_connected())
	{
		mManagerClient->mPeer->write_data(out_data, out_size);
		mManagerClient->mPeer->finish_and_ready();
	}
}

GENERIC InfMaipTunedClient::on_finish(size_type out_total_token_size) 
{

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

InfProgram::maip_err_code InfProgram::inf_create_session(const mbase::string& in_clid = mbase::string(), U64& out_csid, mbase::string& out_clid)
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

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_destroy_client(MBASE_MAIP_CL_AUTH)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_get_acquired_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;
	for(auto& tempModel : mAccClient.mAcceptedModels)
	{
		out_models.push_back(tempModel);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_created_context_ids(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_contexts)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_create_context(MBASE_MAIP_CL_AUTH, const mbase::string& in_model, const U32& in_ctsize)
{
	MBASE_SESSION_CONTROL;

	if (std::find(mAccClient.mAcceptedModels.begin(), mAccClient.mAcceptedModels.end(), in_model) == mAccClient.mAcceptedModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	InfModel& tempModel = mAccClient.mAcceptedModels[in_model];
	InfProcessor::flags procErr;
	for (auto& tmpProc : tempModel)
	{
		procErr = tmpProc->register_client(mAccClient.mChatSessions[mClientSessionIdCounter], in_ctsize);
		if(procErr == InfProcessor::flags::INF_PROC_SUCCESS)
		{
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

	InfMaipTunedClient& mySession = mAccClient.mChatSessions[in_ctxId];
	InfProcessor* myProcessor = NULL;
	InfClient::flags clientResultFlag = mySession.get_host_processor(myProcessor);
	if(clientResultFlag != InfClient::flags::INF_CLIENT_SUCCESS)
	{
		return client_err_to_maip(clientResultFlag);
	}

	clientResultFlag = myProcessor->unregister_client(mySession);

	if (clientResultFlag != InfClient::flags::INF_CLIENT_SUCCESS)
	{
		return client_err_to_maip(clientResultFlag);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_acquire_model(MBASE_MAIP_CL_AUTH, const mbase::string& in_model)
{
	MBASE_SESSION_CONTROL;

	if (mHostedModels.find(in_model) == mHostedModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}
	
	if (std::find(mAccClient.mAcceptedModels.begin(), mAccClient.mAcceptedModels.end(), in_model) != mAccClient.mAcceptedModels.end())
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
	out_models = mAccClient.mAcceptedModels;
	return 0;
}

InfProgram::maip_err_code InfProgram::inf_get_model_params(MBASE_MAIP_CL_AUTH, const mbase::string& in_model)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::exec_set_input(MBASE_MAIP_CL_AUTH, const U64& in_ctxId, InfClient::input_role in_role, const mbase::string& in_input, U32& out_msgid)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::exec_execute_input(MBASE_MAIP_CL_AUTH, const U64& in_ctxId, const U32& in_msgid)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::exec_next(MBASE_MAIP_CL_AUTH, const U64& in_ctxId, mbase::string& out_message)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::exec_terminate_generation(MBASE_MAIP_CL_AUTH, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

GENERIC InfProgram::host_model(InfModel& in_model)
{

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
	return maip_err_code::INF_SUCCESS;
}

MBASE_END
