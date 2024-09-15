#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_sampling.h>
#include <mbase/maip_parser.h>
#include <random>
#include <iostream>

MBASE_BEGIN

#define MBASE_SESSION_CONTROL \
if(!this->is_session_match(in_csid, in_clid))\
{\
	return maip_err_code::INF_UNAUTHORIZED_ACCESS;\
}\
InfAcceptedClient& mAccClient = mActiveClients[in_csid];

GENERIC InfMaipTunedT2TProcessor::on_initialize()
{
	set_inference_client(mNomineeClient);
}

GENERIC InfMaipTunedT2TProcessor::on_destroy()
{
	delete this;
}

GENERIC InfMaipTunedT2TProcessor::set_nominee_client(InfMaipTunedClient* in_nominee)
{
	mNomineeClient = in_nominee;
}

InfMaipTunedClient::InfMaipTunedClient() : lastToken(), mManagerClient(NULL), mIsDeadClient(false)
{

}

InfMaipTunedClient::InfMaipTunedClient(InfAcceptedClient& in_client) : mManagerClient(&in_client), mIsDeadClient(false)
{
}

GENERIC InfMaipTunedClient::on_register(InfTextToTextProcessor* out_processor)
{
}

GENERIC InfMaipTunedClient::on_write(CBYTEBUFFER out_data, size_type out_size, InfTextToTextProcessor::inf_token out_token, bool out_is_special, bool out_is_finish)
{
	if (!mManagerClient->mPeer->is_connected())
	{
		// destroy the context if the client is disconnected
		mManagerClient->mPeer = NULL;
		InfTextToTextProcessor* hostProcessor = NULL;
		get_host_processor(hostProcessor); // 100% success;
		hostProcessor->destroy();
		return;
	}

	mbase::string outData(out_data, out_size);
	mbase::maip_packet_builder tmpPacketBuilder;
	
	if(!out_is_finish)
	{
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_CONTINUE);
	}

	else
	{
		// on_finish will be called
		lastToken = outData;
		return;
	}

	mbase::string outPayload;

	if(out_is_special)
	{
		tmpPacketBuilder.set_kval("SPECIAL", 1);
	}

	else
	{
		tmpPacketBuilder.set_kval("SPECIAL", 0);
	}

	if(outData.size())
	{
		tmpPacketBuilder.generate_payload(outPayload, outData);
	}

	else
	{
		tmpPacketBuilder.generate_payload(outPayload);
	}

	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->finish_and_ready();
}

GENERIC InfMaipTunedClient::on_finish(size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state)
{
	if (!mManagerClient->mPeer->is_connected())
	{
		// destroy the context if the client is disconnected
		mManagerClient->mPeer = NULL;
		InfTextToTextProcessor* hostProcessor = NULL;
		get_host_processor(hostProcessor); // 100% success;
		hostProcessor->destroy();
		return;
	}

	mbase::maip_packet_builder tmpPacketBuilder;
	if(out_finish_state == InfTextToTextProcessor::finish_state::FINISHED)
	{
		tmpPacketBuilder.set_kval("TOKCOUNT", out_total_token_size);
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_FINISH);
	}

	else if(out_finish_state == InfTextToTextProcessor::finish_state::TOKEN_LIMIT_REACHED)
	{
		InfTextToTextProcessor* hostProcessor = NULL;
		get_host_processor(hostProcessor); // 100% success;
		tmpPacketBuilder.set_kval("MAXTOK", hostProcessor->get_max_token_length());
		tmpPacketBuilder.set_kval("TOKCOUNT", out_total_token_size);
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_TOKEN_LIMIT_EXCEEDED);
	}

	else if(out_finish_state == InfTextToTextProcessor::finish_state::ABANDONED)
	{
		tmpPacketBuilder.set_kval("TOKCOUNT", out_total_token_size);
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_ABANDONED);
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

	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->finish_and_ready();
}

GENERIC InfMaipTunedClient::on_unregister() 
{
	if (mIsDeadClient)
	{
		delete this;
	}
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
	// COLLISION IS NOT CHECKED AND IT WILL LIKELY TO HAPPEN
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

	for(mbase::unordered_map<U64, InfMaipTunedClient*>::iterator It = mAccClient.mChatSessions.begin(); It != mAccClient.mChatSessions.end();)
	{
		// destroy all chats
		InfTextToTextProcessor* tmpProc = NULL;
		It->second->get_host_processor(tmpProc);
		if (tmpProc)
		{
			It->second->mIsDeadClient = true;
			InfMaipTunedT2TProcessor* t2tProc = static_cast<InfMaipTunedT2TProcessor*>(tmpProc);
			t2tProc->destroy();
			It = mAccClient.mChatSessions.erase(It);
		}
		else
		{
			delete It->second;
			It = mAccClient.mChatSessions.erase(It);
		}
	}

	mActiveClients.erase(in_csid);

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

InfProgram::maip_err_code InfProgram::inf_create_context(MBASE_MAIP_CL_AUTH, const mbase::string& in_model, const U32& in_ctsize, U64& out_ctxId, const mbase::vector<InfSamplingInit>& in_samplers)
{
	MBASE_SESSION_CONTROL;

	// TODO: CHECK IF THERE IS ENOUGH MEMORY FOR CONTEXTS

	if (std::find(mAccClient.mAcceptedModels.begin(), mAccClient.mAcceptedModels.end(), in_model) == mAccClient.mAcceptedModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	if(mRegisteredModels.find(in_model) == mRegisteredModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	if(in_ctsize < 32)
	{
		return maip_err_code::INF_INVALID_TOKEN_LIMIT;
	}

	// temp
	// top_k
	// typical_p
	// top_p
	// min_p
	// tailfree
	// softmax
	InfMaipTunedClient* currentClient = new InfMaipTunedClient();
	currentClient->mManagerClient = &mAccClient;
	currentClient->mIsDeadClient = false;
	U64 tmpSessionIdCounter = mAccClient.mChatSessionIdCounter;

	InfModelTextToText* registeredModel = mRegisteredModels[in_model];
	InfMaipTunedT2TProcessor* newProcessor = new InfMaipTunedT2TProcessor;
	newProcessor->set_nominee_client(currentClient);
	for(mbase::vector<InfSamplingInit>::const_iterator cIt = in_samplers.cbegin(); cIt != in_samplers.cend(); ++cIt)
	{
		if(cIt->mSamplerName == "temp")
		{
			newProcessor->add_sampler<InfSamplingTemperature>(cIt->mCommonFloat);
		}
		else if(cIt->mSamplerName == "top_k")
		{
			newProcessor->add_sampler<InfSamplingTopK>(cIt->mCommonFloat);
		}
		else if(cIt->mSamplerName == "typical_p")
		{
			newProcessor->add_sampler<InfSamplingTypicalP>(cIt->mCommonFloat);
		}
		else if(cIt->mSamplerName == "top_p")
		{
			newProcessor->add_sampler<InfSamplingTopP>(cIt->mCommonFloat);
		}
		else if(cIt->mSamplerName == "min_p")
		{
			newProcessor->add_sampler<InfSamplingMinP>(cIt->mCommonFloat);
		}
		else if(cIt->mSamplerName == "tailfree")
		{
			newProcessor->add_sampler<InfSamplingTailFree>(cIt->mCommonFloat);
		}
	}
	out_ctxId = ++mAccClient.mChatSessionIdCounter;
	mAccClient.mChatSessions[out_ctxId] = currentClient;
	InfModelTextToText::flags outFlag = registeredModel->register_context_process(newProcessor, in_ctsize);

	return inf_proc_err_to_maip(newProcessor->get_processor_status());
}

InfProgram::maip_err_code InfProgram::inf_activate_context(MBASE_MAIP_CL_AUTH, const mbase::string& in_model, const U64& in_ctxId, const U32& in_ctsize, const mbase::vector<InfSamplingInit>& in_samplers)
{
	MBASE_SESSION_CONTROL;

	if (std::find(mAccClient.mAcceptedModels.begin(), mAccClient.mAcceptedModels.end(), in_model) == mAccClient.mAcceptedModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	if (mRegisteredModels.find(in_model) == mRegisteredModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	if (mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}
	
	InfMaipTunedClient* mySession = mAccClient.mChatSessions[in_ctxId];

	if(mySession->is_registered())
	{
		return maip_err_code::INF_CONTEXT_ACTIVE;
	}

	mySession->mIsDeadClient = false;
	InfModelTextToText* registeredModel = mRegisteredModels[in_model];
	InfMaipTunedT2TProcessor* newProcessor = new InfMaipTunedT2TProcessor;
	newProcessor->set_nominee_client(mySession);
	for (mbase::vector<InfSamplingInit>::const_iterator cIt = in_samplers.cbegin(); cIt != in_samplers.cend(); ++cIt)
	{
		if (cIt->mSamplerName == "temp")
		{
			newProcessor->add_sampler<InfSamplingTemperature>(cIt->mCommonFloat);
		}
		else if (cIt->mSamplerName == "top_k")
		{
			newProcessor->add_sampler<InfSamplingTopK>(cIt->mCommonFloat);
		}
		else if (cIt->mSamplerName == "typical_p")
		{
			newProcessor->add_sampler<InfSamplingTypicalP>(cIt->mCommonFloat);
		}
		else if (cIt->mSamplerName == "top_p")
		{
			newProcessor->add_sampler<InfSamplingTopP>(cIt->mCommonFloat);
		}
		else if (cIt->mSamplerName == "min_p")
		{
			newProcessor->add_sampler<InfSamplingMinP>(cIt->mCommonFloat);
		}
		else if (cIt->mSamplerName == "tailfree")
		{
			newProcessor->add_sampler<InfSamplingTailFree>(cIt->mCommonFloat);
		}
	}

	InfModelTextToText::flags outFlag = registeredModel->register_context_process(newProcessor, in_ctsize);
	return inf_proc_err_to_maip(newProcessor->get_processor_status());
	// TODO: CREATE CONTEXT
}

InfProgram::maip_err_code InfProgram::inf_clear_short_term_history(MBASE_MAIP_CL_AUTH, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;
	if (mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}
	InfMaipTunedClient* mySession = mAccClient.mChatSessions[in_ctxId];
	mySession->clear_chat_history();
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_context_status(MBASE_MAIP_CL_AUTH, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	if (mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient* mySession = mAccClient.mChatSessions[in_ctxId];
	if(mySession->is_registered())
	{
		return maip_err_code::INF_CONTEXT_ACTIVE;
	}

	mySession->mIsDeadClient = false;

	return maip_err_code::INF_CONTEXT_INACTIVE;
}

InfProgram::maip_err_code InfProgram::inf_destroy_context(MBASE_MAIP_CL_AUTH, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	if(mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient* mySession = mAccClient.mChatSessions[in_ctxId];
	InfTextToTextProcessor* tmpProc = NULL;
	mySession->get_host_processor(tmpProc);
	if(tmpProc)
	{
		mySession->mIsDeadClient = true;
		tmpProc->destroy();
	}
	else
	{
		delete mySession;
	}

	mAccClient.mChatSessions.erase(in_ctxId);
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_release_context(MBASE_MAIP_CL_AUTH, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;
	if (mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient* mySession = mAccClient.mChatSessions[in_ctxId];
	mySession->mIsDeadClient = false;
	InfTextToTextProcessor* tmpProc = NULL;

	mySession->get_host_processor(tmpProc);
	if (tmpProc)
	{
		tmpProc->destroy();
	}

	return maip_err_code::INF_SUCCESS;
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

InfProgram::maip_err_code InfProgram::exec_set_input(MBASE_MAIP_CL_AUTH, const U64& in_ctxId, context_role in_role, const mbase::string& in_input, U32& out_msgid)
{
	MBASE_SESSION_CONTROL;

	if(mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient* mySession = mAccClient.mChatSessions[in_ctxId];
	InfClientTextToText::flags errCode = mySession->add_message(in_input, in_role, out_msgid);
	
	if(errCode == InfClientTextToText::flags::INF_CLIENT_ERR_MISSING_INPUT)
	{
		return maip_err_code::EXEC_MISSING_MESSAGE;
	}
	return maip_err_code::EXEC_SUCCESS;
}

InfProgram::maip_err_code InfProgram::exec_execute_input(MBASE_MAIP_CL_AUTH, const U64& in_ctxId, mbase::vector<U32>& in_msgid, mbase::vector<mbase::string> in_sampler_order)
{
	MBASE_SESSION_CONTROL;
	
	if (mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient* mySession = mAccClient.mChatSessions[in_ctxId];
	InfTextToTextProcessor* myProcessor = NULL;
	mySession->get_host_processor(myProcessor);

	if (!myProcessor)
	{
		return maip_err_code::EXEC_CONTEXT_INACTIVE;
	}

	if(!in_msgid.size())
	{
		return maip_err_code::EXEC_MESSAGE_ID_MISMATCH;
	}

	mbase::vector<context_line> outContexts;
	if(mySession->get_message_array(in_msgid.data(), in_msgid.size(), outContexts) == InfClientTextToText::flags::INF_CLIENT_ERR_MSG_ID_MISMATCH)
	{
		return maip_err_code::EXEC_MESSAGE_ID_MISMATCH;
	}
	
	mbase::InfClientTextToText::token_vector tokenVec;
	mbase::InfTextToTextProcessor::flags errCode = myProcessor->tokenize_input(outContexts.data(), outContexts.size(), tokenVec);
	maip_err_code outErrCode = inf_exec_err_to_maip(errCode);
	if(outErrCode != maip_err_code::EXEC_SUCCESS)
	{
		return outErrCode;
	}

	errCode = myProcessor->execute_input(tokenVec, true, in_sampler_order);
	outErrCode = inf_exec_err_to_maip(errCode);

	return outErrCode;
}

InfProgram::maip_err_code InfProgram::exec_next(MBASE_MAIP_CL_AUTH, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	if (mAccClient.mChatSessions.find(in_ctxId) == mAccClient.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient* mySession = mAccClient.mChatSessions[in_ctxId];
	InfTextToTextProcessor* myProcessor = NULL;
	mySession->get_host_processor(myProcessor);
	if(myProcessor)
	{
		mAccClient.mPeer = in_peer;
		mbase::InfTextToTextProcessor::flags errCode = myProcessor->next();
		maip_err_code outErrCode = inf_exec_err_to_maip(errCode);
		return outErrCode;
	}
	else
	{
		return maip_err_code::INF_CONTEXT_INACTIVE;
	}
}

InfProgram::flags InfProgram::host_model(InfModelTextToText* in_model)
{
	if(!in_model)
	{
		return flags::INF_PROGRAM_ERR_MODEL_MISSING;
	}

	if(!in_model->is_initialized())
	{
		return flags::INF_PROGRAM_ERR_MODEL_IS_NOT_INITIALIZED;
	}

	mbase::string currentModelName;
	in_model->get_model_name(currentModelName); // 100% success

	for (auto& tmpModels : mRegisteredModels) 
	{
		if(tmpModels.first == currentModelName)
		{
			return flags::INF_PROGRAM_ERR_MODEL_ALREADY_BEING_HOSTED;
		}
	}

	mRegisteredModels[currentModelName] = in_model;
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

InfProgram::maip_err_code InfProgram::inf_proc_err_to_maip(InfProcessorBase::flags in_flag)
{
	switch (in_flag)
	{
	case mbase::InfProcessorBase::flags::INF_PROC_SUCCESS:
		return maip_err_code::INF_SUCCESS;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR:
		return maip_err_code::INF_CONTEXT_INACTIVE;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_ALREADY_INITIALIZED:
		return maip_err_code::INF_SUCCESS;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_MODEL_IS_NOT_INITIALIZED:
		return maip_err_code::INF_CONTEXT_HOST_MODEL_SYSTEM_ERROR;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_TOKEN_LIMIT_IS_TOO_LOW:
		return maip_err_code::INF_INVALID_TOKEN_LIMIT;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_INPUT_IS_EMPTY:
		return maip_err_code::INF_CONTEXT_INPUT_IS_EMPTY;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_INFO_INITIALIZING:
		return maip_err_code::INF_CONTEXT_INITIALIZING;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_INFO_DESTROYING:
		return maip_err_code::INF_CONTEXT_DESTROYING;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_INFO_HALTED:
		return maip_err_code::INF_CONTEXT_HALTED;
		break;
	default:
		return maip_err_code::INF_UNKNOWN_STATUS;
		break;
	}
}

InfProgram::maip_err_code InfProgram::inf_exec_err_to_maip(InfProcessorBase::flags in_flag)
{
	switch (in_flag)
	{
	case mbase::InfProcessorBase::flags::INF_PROC_SUCCESS:
		return maip_err_code::EXEC_SUCCESS;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_ALREADY_PROCESSING:
		return maip_err_code::EXEC_ALREADY_PROCESSING;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT:
		return maip_err_code::EXEC_TOKEN_LIMIT_EXCEEDED;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT:
		return maip_err_code::EXEC_TOKENIZATION_FAILED;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_ERR_INPUT_IS_EMPTY:
		return maip_err_code::EXEC_MISSING_MESSAGE;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_INFO_INITIALIZING:
		return maip_err_code::INF_CONTEXT_INITIALIZING;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_INFO_DESTROYING:
		return maip_err_code::INF_CONTEXT_DESTROYING;
		break;
	case mbase::InfProcessorBase::flags::INF_PROC_INFO_HALTED:
		return maip_err_code::INF_CONTEXT_HALTED;
		break;
	default:
		return maip_err_code::EXEC_UNKNOWN_STATUS;
		break;
	}
}

GENERIC InfProgram::update()
{
	for(auto& modelMap : mRegisteredModels)
	{
		modelMap.second->update();
	}
}

MBASE_END
