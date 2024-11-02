#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_sampling.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/maip_parser.h>
#include <mbase/pc/pc_state.h>
#include <mbase/filesystem.h>
#include <random>
#include <iostream>

MBASE_BEGIN

#define MBASE_SESSION_CONTROL \
if(!this->is_session_token_valid(in_session_token))\
{\
	return maip_err_code::INF_SESSION_TOKEN_MISMATCH;\
}\
InfClientSession& clientSession = mSessionMap[in_session_token];

class InfMaipModel : public mbase::InfModelTextToText {
public:
	InfMaipModel(const mbase::string& in_as_name, InfProgram& in_program);
	GENERIC on_initialize_fail(init_fail_code out_fail_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;
private:
	mbase::string mAsName;
	InfProgram* mAssignedProgram;
};

InfMaipModel::InfMaipModel(const mbase::string& in_as_name, InfProgram& in_program) :
	mAsName(in_as_name),
	mAssignedProgram(&in_program)
{

}

GENERIC InfMaipModel::on_initialize_fail(init_fail_code out_fail_code)
{
	mAssignedProgram->get_registered_models().erase(mAsName);
}

GENERIC InfMaipModel::on_initialize()
{

}

GENERIC InfMaipModel::on_destroy()
{
	std::cout << "Model is being destroyed" << std::endl;
	delete this;
}

GENERIC InfMaipTunedT2TProcessor::on_initialize()
{
	set_inference_client(mNomineeClient);

	InfClientSession* managerClient = mNomineeClient->mManagerClient;

	managerClient->mChatSessions[managerClient->mContextCounter] = mNomineeClient;
	mNomineeClient->mCurrentContextIndex = managerClient->mContextCounter;
	managerClient->mContextCounter++;

	mbase::maip_packet_builder tmpPacketBuilder;
	mbase::string outPayload;
	tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_SUCCESS);
	tmpPacketBuilder.set_kval("CTXID", mNomineeClient->mCurrentContextIndex);
	tmpPacketBuilder.generate_payload(outPayload);
	managerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	managerClient->mPeer->send_write_signal();
	managerClient->mPeer->send_read_signal();
}

GENERIC InfMaipTunedT2TProcessor::on_initialize_fail(init_fail_code out_code)
{
	mbase::maip_packet_builder tmpPacketBuilder;
	mbase::string outPayload;
	tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_FAILED_TO_CREATE_CONTEXT);
	tmpPacketBuilder.generate_payload(outPayload);
	mNomineeClient->mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mNomineeClient->mManagerClient->mPeer->send_write_signal();
	mNomineeClient->mManagerClient->mPeer->send_read_signal();

	delete mNomineeClient;
	delete this;
}

GENERIC InfMaipTunedT2TProcessor::on_destroy()
{
	std::cout << "Processor is deleted" << std::endl;
	delete this;
}

GENERIC InfMaipTunedT2TProcessor::set_nominee_client(InfMaipTunedClient* in_nominee)
{
	mNomineeClient = in_nominee;
}

InfMaipTunedClient::InfMaipTunedClient() : lastToken(), mManagerClient(NULL), mCurrentContextIndex(0)
{

}

InfMaipTunedClient::InfMaipTunedClient(InfClientSession& in_client) : mManagerClient(&in_client)
{
}

GENERIC InfMaipTunedClient::on_register(InfTextToTextProcessor* out_processor)
{
}

GENERIC InfMaipTunedClient::on_write(CBYTEBUFFER out_data, size_type out_size, InfTextToTextProcessor::inf_token out_token, bool out_is_special, bool out_is_finish)
{
	if (!mManagerClient->mPeer->is_connected())
	{
		mManagerClient->mPeer = NULL;
		InfTextToTextProcessor* hostProcessor = NULL;
		get_host_processor(hostProcessor);
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
	mManagerClient->mPeer->send_write_signal();
	mManagerClient->mPeer->send_read_signal();
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
	mManagerClient->mPeer->send_write_signal();
	mManagerClient->mPeer->send_read_signal();
}

GENERIC InfMaipTunedClient::on_unregister() 
{
	std::cout << "Unregistered the client" << std::endl;
	if(mIsSessionAlive)
	{
		mManagerClient->mChatSessions.erase(mCurrentContextIndex);
	}
	delete this;
}

InfClientSession::~InfClientSession()
{
	for(chat_session_map::iterator It = mChatSessions.begin(); It != mChatSessions.end(); ++It)
	{
		It->second->mIsSessionAlive = false;
		InfTextToTextProcessor* tProc = NULL;
		It->second->get_host_processor(tProc);
		if(tProc)
		{
			tProc->destroy();
		}
	}
}

bool InfProgram::is_session_token_valid(const mbase::string& in_session_token)
{
	accepted_client_map::iterator It = mSessionMap.find(in_session_token);
	if(It == mSessionMap.end())
	{
		return false;
	}

	return true;
}

typename InfProgram::accepted_client_map& InfProgram::get_accepted_clients()
{
	return mSessionMap;
}

typename InfProgram::registered_model_map& InfProgram::get_registered_models()
{
	return mRegisteredModels;
}

InfProgram::maip_err_code InfProgram::inf_access_request(const mbase::string& in_username, const mbase::string& in_access_token, std::shared_ptr<PcNetPeerClient> in_client, mbase::string& out_session_token)
{
	inference_user_map::iterator It = mUserMap.find(in_username);
	if(It == mUserMap.end())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	InfMaipUser& maipUser = It->second;
	if(maipUser.get_access_key() == in_access_token)
	{
		out_session_token = mbase::string::generate_uuid();
		InfClientSession newClientSession;
		newClientSession.mMaipUser = maipUser;
		newClientSession.mPeer = in_client;
		mSessionMap[out_session_token] = newClientSession;

		return maip_err_code::INF_SUCCESS;
	}

	return maip_err_code::INF_AUTHORIZATION_FAILED;
}

InfProgram::maip_err_code InfProgram::inf_destroy_session(const mbase::string& in_session_token)
{
	if(this->is_session_token_valid(in_session_token))
	{
		// It will destroy context in its destructor
		mSessionMap.erase(in_session_token);
	}
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_accessible_models(const mbase::string& in_session_token, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;

	const InfMaipUser::model_name_vector& accModels = clientSession.mMaipUser.get_accessible_models();
	for(auto& n : accModels)
	{
		out_models.push_back(n);
	}
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_context_ids(const mbase::string& in_session_token, mbase::vector<U64>& out_contexts)
{
	MBASE_SESSION_CONTROL;
	
	for(auto& n : clientSession.mChatSessions)
	{
		out_contexts.push_back(n.first);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_create_context(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_model, const U32& in_ctsize, const mbase::vector<InfSamplingInput>& in_samplers)
{
	MBASE_SESSION_CONTROL;
	
	if(!clientSession.mMaipUser.is_model_accessible(in_model))
	{
		return maip_err_code::INF_MODEL_IS_NOT_ACCESSIBLE;
	}

	if (in_ctsize < 32)
	{
		return maip_err_code::INF_INVALID_TOKEN_LIMIT;
	}

	registered_model_map::iterator It = mRegisteredModels.find(in_model);
	if(It == mRegisteredModels.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	if(in_ctsize > clientSession.mMaipUser.get_maximum_context_length())
	{
		return maip_err_code::INF_USER_CONTEXT_LENGTH_EXCEEDED;
	}

	InfModelTextToText* t2tModel = It->second;
	if(!t2tModel->is_available(in_ctsize))
	{
		return maip_err_code::INF_MODEL_CONTEXT_FULL;
	}

	// means the model is available
	// we can create the context
	InfMaipTunedClient* maipTunedClient = new InfMaipTunedClient;
	InfMaipTunedT2TProcessor* maipNewContext = new InfMaipTunedT2TProcessor;
	
	clientSession.mPeer = in_peer;

	maipTunedClient->mManagerClient = &clientSession;
	maipNewContext->set_nominee_client(maipTunedClient);

	for (mbase::vector<InfSamplingInput>::const_iterator cIt = in_samplers.cbegin(); cIt != in_samplers.cend(); ++cIt)
	{
		maipNewContext->add_sampler(*cIt);
	}

	if(t2tModel->register_context_process(maipNewContext, in_ctsize) != InfModelTextToText::flags::INF_MODEL_INFO_REGISTERING_PROCESSOR)
	{
		// TODO: FIX HERE
		// means there is a problem
		// unknown problem
		delete maipTunedClient;
		delete maipNewContext;
		return maip_err_code::INF_FAILED_TO_CREATE_CONTEXT;
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_clear_context_history(const mbase::string& in_session_token, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;
	
	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	if(It != clientSession.mChatSessions.end())
	{
		It->second->clear_chat_history();
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_context_status(const mbase::string& in_session_token, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	if(It == clientSession.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfTextToTextProcessor* hostProc = NULL;
	It->second->get_host_processor(hostProc); // 100% success

	InfTextToTextProcessor::flags outStatus = hostProc->get_processor_status();

	if(outStatus == InfTextToTextProcessor::flags::INF_PROC_INFO_INITIALIZING)
	{
		return maip_err_code::INF_CONTEXT_INITIALIZING;
	}

	if(hostProc->is_available())
	{
		return maip_err_code::INF_CONTEXT_INACTIVE;
	}

	else
	{
		return maip_err_code::INF_CONTEXT_ACTIVE;
	}
}

InfProgram::maip_err_code InfProgram::inf_destroy_context(const mbase::string& in_session_token, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	if (It == clientSession.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}
	
	InfTextToTextProcessor* hostProc = NULL;
	It->second->get_host_processor(hostProc); // 100% success
	
	hostProc->destroy();
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_program_models(const mbase::string& in_session_token, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;

	for(auto& n : mModelInformationMap)
	{
		out_models.push_back(n.first);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_load_model(const mbase::string& in_session_token, const mbase::string& in_modelname, const U32& in_total_context_size)
{
	MBASE_SESSION_CONTROL;

	if(!clientSession.mMaipUser.is_flags_set(MAIP_MODEL_LOAD_UNLOAD))
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	if(!in_total_context_size)
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}
	
	for(auto& n : mRegisteredModels)
	{
		if(n.first == in_modelname)
		{
			return maip_err_code::INF_SUCCESS;
		}
	}

	for(auto& n : mModelInformationMap)
	{
		if(n.first == in_modelname)
		{
			InfMaipModel* newModel = new InfMaipModel(n.second.mModelName, *this);
			newModel->initialize_model(n.second.mModelPath, in_total_context_size, 999);
			mRegisteredModels[n.second.mModelName] = newModel;
			return maip_err_code::INF_SUCCESS;
		}
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_unload_model(const mbase::string& in_session_token, const mbase::string& in_modelname)
{
	MBASE_SESSION_CONTROL;

	if (!clientSession.mMaipUser.is_flags_set(MAIP_MODEL_LOAD_UNLOAD))
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	registered_model_map::iterator It = mRegisteredModels.find(in_modelname);
	if(It == mRegisteredModels.end())
	{
		return maip_err_code::INF_SUCCESS;
	}

	InfModelTextToText* t2tModel = It->second;
	//mRegisteredModels.erase(It);

	t2tModel->destroy();

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_create_new_user(
	const mbase::string& in_session_token,
	const mbase::string& in_username,
	const U32& in_model_access_limit,
	const U32& in_maximum_context_length,
	const bool& in_superuser,
	const bool& in_authorization_locked,
	const mbase::string& in_access_token,
	const mbase::vector<mbase::string>& in_authority_flags,
	mbase::string& out_access_token
)
{
	MBASE_SESSION_CONTROL;

	if(!in_username.size())
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}
	
	if(!clientSession.mMaipUser.is_flags_set(MAIP_USER_CREATE_DELETE))
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	inference_user_map::iterator It = mUserMap.find(in_username);
	if(It != mUserMap.end())
	{
		return maip_err_code::INF_USER_ALREADY_EXISTS;
	}

	U32 modelAccessLimit = in_model_access_limit;
	U32 maximumContextLength = in_maximum_context_length;
	mbase::string accessToken = in_access_token;

	if (!modelAccessLimit)
	{
		modelAccessLimit = mDefaultModelAccessLimit;
	}

	if(!maximumContextLength)
	{
		maximumContextLength = mDefaultContextLimit;
	}

	if(!accessToken.size())
	{
		accessToken = mbase::string::generate_uuid();
	}

	U32 authorityFlags = 0;

	for(auto& n : in_authority_flags)
	{
		if (n == "LOAD")
		{
			authorityFlags |= MAIP_MODEL_LOAD_UNLOAD;
		}
		else if (n == "ADAPTER")
		{
			authorityFlags |= MAIP_ADAPTER_LOAD_UNLOAD;
		}
		else if(n == "LENGTH")
		{
			authorityFlags |= MAIP_CONTEXT_LENGTH_MODIFICATION;
		}
		else if (n == "ACCESS")
		{
			authorityFlags |= MAIP_USER_ACCESS_MODIFICATION;
		}
		else if(n == "CREATE")
		{
			authorityFlags |= MAIP_USER_CREATE_DELETE;
		}
		else if(n == "MODIFICATION")
		{
			authorityFlags |= MAIP_USER_MODIFICATION;
		}
		else if(n == "STATIC")
		{
			authorityFlags |= MAIP_USER_STATIC;
		}
	}

	if(in_superuser && !clientSession.mMaipUser.is_superuser())
	{
		// if the client session is not the super user but trying to create a super user, stop!
		// only super users can create super users.
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	create_user(in_username, modelAccessLimit, maximumContextLength, in_superuser, in_authorization_locked, accessToken, authorityFlags, true, out_access_token);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_delete_user(const mbase::string& in_session_token, const mbase::string& in_username)
{
	MBASE_SESSION_CONTROL;

	if(!clientSession.mMaipUser.is_flags_set(MAIP_USER_CREATE_DELETE))
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	if(!in_username.size())
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	if(clientSession.mMaipUser.get_username() == in_username)
	{
		return maip_err_code::INF_CANT_DELETE_SELF;
	}

	inference_user_map::iterator It = mUserMap.find(in_username);
	if(It != mUserMap.end())
	{
		InfMaipUser& maipUser = It->second;
		if(maipUser.is_superuser() && !clientSession.mMaipUser.is_superuser())
		{
			// If the client attempts to delete a super user and he is not a super user, 
			// Authorization fails.
			return maip_err_code::INF_AUTHORIZATION_FAILED;
		}
	}
	
	else
	{
		return maip_err_code::INF_USER_NOT_FOUND;
	}

	InfMaipUser& maipUser = It->second;
	
	for(accepted_client_map::iterator acceptedClientIt = mSessionMap.begin(); acceptedClientIt != mSessionMap.end();)
	{
		InfClientSession& tmpSession = acceptedClientIt->second;
		if(tmpSession.mMaipUser.get_username() == in_username)
		{
			// Erase all sessions with the given maip user
			acceptedClientIt = mSessionMap.erase(acceptedClientIt);
			continue;
		}
		++acceptedClientIt;
	}

	mUserMap.erase(It);

	mbase::string usernameSanitized = in_username;
	usernameSanitized.remove_all('/'); // Reason I am doing this is if the user attempts to exploit the file path
	usernameSanitized.remove_all('*'); // Reason I am doing this is if the user attempts to exploit the file path

	mbase::wstring fileToBeDeleted = mClientStateDirectory + mbase::from_utf8(in_username) + L".mbfs";
	mbase::delete_file(fileToBeDeleted);
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_model_access_limit(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_new_access_limit)
{
	MBASE_SESSION_CONTROL;

	if(!clientSession.mMaipUser.is_flags_set(MAIP_USER_ACCESS_MODIFICATION))
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	inference_user_map::iterator It = mUserMap.find(in_username);
	if(It == mUserMap.end())
	{
		return maip_err_code::INF_USER_NOT_FOUND;
	}

	InfMaipUser& maipUser = It->second;
	if(maipUser.is_superuser() && !clientSession.mMaipUser.is_superuser())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	maipUser.set_distinct_model_access_limit(in_new_access_limit);

	maipUser.update_state_file(mClientStateDirectory, true);
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_maximum_context_length(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_maximum_context_length)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_make_superuser(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_access_token)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_unmake_superuser(const mbase::string & in_session_token, const mbase::string& in_username)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_accept_models(const mbase::string & in_session_token, const mbase::string& in_username, const mbase::vector<mbase::string>&in_models)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_set_authority_flags(const mbase::string & in_session_token, const mbase::string& in_username, const mbase::vector<mbase::string>&in_authority_flags)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::exec_set_input(const mbase::string& in_session_token, const U64& in_ctxId, mbase::context_role in_role, const mbase::string& in_input, U32& out_msgid)
{
	MBASE_SESSION_CONTROL;
	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	if(It == clientSession.mChatSessions.end())
	{
		// Couldn't find the chat session
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}
	InfMaipTunedClient* tmpClient = It->second;
	U32 outMsg = 0;
	if(tmpClient->add_message(in_input, in_role, outMsg) == InfClientTextToText::flags::INF_CLIENT_ERR_MISSING_INPUT)
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	out_msgid = outMsg;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::exec_execute_input(const mbase::string& in_session_token, const U64& in_ctxId, mbase::vector<U32>& in_msgid)
{
	MBASE_SESSION_CONTROL;
	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	if (It == clientSession.mChatSessions.end())
	{
		// Couldn't find the chat session
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfMaipTunedClient* tmpClient = It->second;
	InfTextToTextProcessor* hostProcessor;
	tmpClient->get_host_processor(hostProcessor); // 100% success

	mbase::vector<context_line> outMessages;
	if(tmpClient->get_message_array(in_msgid.data(), in_msgid.size(), outMessages) == InfClientTextToText::flags::INF_CLIENT_ERR_MSG_ID_MISMATCH)
	{
		return maip_err_code::EXEC_MESSAGE_ID_MISMATCH;
	}

	mbase::InfClientTextToText::token_vector tokenVector;
	if(hostProcessor->tokenize_input(outMessages.data(), outMessages.size(), tokenVector) != InfProcessorBase::flags::INF_PROC_SUCCESS)
	{
		// This shouldn't happen
		return maip_err_code::EXEC_TOKENIZATION_FAILED;
	}

	InfTextToTextProcessor::flags execErr = hostProcessor->execute_input(tokenVector, true);
	if(execErr == InfTextToTextProcessor::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT)
	{
		return maip_err_code::EXEC_TOKEN_LIMIT_EXCEEDED;
	}

	if(execErr == InfTextToTextProcessor::flags::INF_PROC_INFO_HALTED)
	{
		return maip_err_code::EXEC_PROCESS_HALTED;
	}

	return maip_err_code::EXEC_SUCCESS;
}

InfProgram::maip_err_code InfProgram::exec_next(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;
	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	if (It == clientSession.mChatSessions.end())
	{
		// Couldn't find the chat session
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}

	InfTextToTextProcessor* hostProcessor = NULL;
	clientSession.mPeer = in_peer;
	It->second->get_host_processor(hostProcessor);
	
	InfTextToTextProcessor::flags outRes = hostProcessor->next();
	if(outRes != InfTextToTextProcessor::flags::INF_PROC_SUCCESS)
	{
		return maip_err_code::EXEC_MISSING_MESSAGE;
	}

	return maip_err_code::EXEC_SUCCESS;
}

GENERIC InfProgram::initialize(InfProgramInformation in_program_information)
{
	// TODO: Handle all possibilities

	mInferenceDiagnostics.initialize(in_program_information.mProgramInformation.mProductName + "main_log");
	mInferenceConfigurator.initialize(
		mInferenceDiagnostics,
		in_program_information.mTempPath,
		in_program_information.mExecutionPath,
		in_program_information.mDataPath
	);
	mbase::string mainStateName = in_program_information.mProgramInformation.mProductName;
	mMainProgramState.initialize(mainStateName, in_program_information.mDataPath);

	this->initialize_system(
		in_program_information.mProgramInformation,
		&mInferenceConfigurator,
		&mInferenceDiagnostics,
		NULL,
		&mInferenceNetManager,
		&mMainProgramState
	);

	mClientStateDirectory = mInferenceConfigurator.get_data_path() + L"states/users/";
	mModelsDirectory = mInferenceConfigurator.get_data_path() + L"models/";

	mbase::create_directory(mInferenceConfigurator.get_data_path() + L"states/");
	mbase::create_directory(mClientStateDirectory);
	mbase::create_directory(mModelsDirectory);

	mbase::vector<FS_FILE_INFORMATION> fileInfo;
	mbase::get_directory(mClientStateDirectory, fileInfo);

	for(mbase::vector<FS_FILE_INFORMATION>::iterator It = fileInfo.begin(); It != fileInfo.end(); ++It)
	{
		// Reloading users from state directory
		FS_FILE_INFORMATION fi = *It;
		PcState myState;

		myState.initialize(mbase::to_utf8(fi.fileName), mClientStateDirectory);
		
		U32 authorityFlags;
		U32 modelAccessLimit;
		U32 maxContextLength;
		mbase::vector<mbase::string> accessibleModels;
		mbase::string userName;
		mbase::string accessKey;
		bool superUser;
		bool authLocked;

		if(myState.get_state<U32>("authority_flags", authorityFlags) != PcState::flags::STATE_SUCCESS ||
			myState.get_state<U32>("model_access_limit", modelAccessLimit) != PcState::flags::STATE_SUCCESS ||
			myState.get_state<U32>("max_context_length", maxContextLength) != PcState::flags::STATE_SUCCESS ||
			myState.get_state<mbase::vector<mbase::string>>("accessible_models", accessibleModels) != PcState::flags::STATE_SUCCESS ||
			myState.get_state<mbase::string>("username", userName) != PcState::flags::STATE_SUCCESS ||
			myState.get_state<mbase::string>("access_key", accessKey) != PcState::flags::STATE_SUCCESS ||
			myState.get_state<bool>("is_super", superUser) != PcState::flags::STATE_SUCCESS ||
			myState.get_state<bool>("is_auth_locked", authLocked) != PcState::flags::STATE_SUCCESS)
		{
			continue;
		}

		InfMaipUser maipUser;
		maipUser.add_authority_flags(authorityFlags);
		maipUser.set_distinct_model_access_limit(modelAccessLimit);
		maipUser.set_maximum_context_length(maxContextLength);
		maipUser.set_username(userName);
		maipUser.set_access_key(accessKey);

		if(superUser)
		{
			maipUser.make_superuser();
		}

		if(authLocked)
		{
			maipUser.lock_authorization();
		}

		for(mbase::vector<mbase::string>::iterator It = accessibleModels.begin(); It != accessibleModels.end(); ++It)
		{
			maipUser.add_accessible_model(*It);
		}

		std::cout << "Loading user: " << std::endl;
		std::cout << "- Authority value: " << authorityFlags << std::endl;
		std::cout << "- Model access limit: " << modelAccessLimit << std::endl;
		std::cout << "- Maximum context size: " << maxContextLength << std::endl;
		std::cout << "- Username: " << userName << std::endl;
		std::cout << "- Access key: " << accessKey << std::endl;
		std::cout << "- Is super user:" << superUser << std::endl;
		std::cout << "- Is authorization locked:" << authLocked << std::endl;
		std::cout << "=======\n" << std::endl;
		mUserMap[userName] = maipUser;
	}

	fileInfo.clear();
	mbase::get_directory(mClientStateDirectory, fileInfo);
	for(mbase::vector<FS_FILE_INFORMATION>::iterator It = fileInfo.begin(); It != fileInfo.end(); ++It)
	{
		// Querying models under the model directory

		mbase::GgufMetaConfigurator ggufConfigurator(mClientStateDirectory + It->fileName);
		mbase::string modelName;

		if(!ggufConfigurator.get_key("mbase.model_name", modelName))
		{
			std::cout << "Found GGUF file: " << mbase::to_utf8(It->fileName) << std::endl;
			std::cout << "Applying MBASE parameters..." << std::endl;
			ggufConfigurator.apply_mbase_parameter(""); // TODO, break if this fails
			ggufConfigurator.clear_context();
			std::cout << "MBASE Parameters successfully applied." << std::endl;
		}
		ggufConfigurator.clear_context();

		// TEMPORARY SOLUTION, FIX THIS CODE ASAP
		// IMPLEMENT initialize method on gguf configurator
		// And make it movable if possible

		mbase::GgufMetaConfigurator seriousGgufConfigurator(mModelsDirectory + It->fileName);

		InfRegisteredModelInformation modelInformation;
		modelInformation.mModelPath = mModelsDirectory + It->fileName;

		seriousGgufConfigurator.get_key("mbase.model_name", modelInformation.mModelName);
		seriousGgufConfigurator.get_key("mbase.model_architecture", modelInformation.mModelArchitecture);
		seriousGgufConfigurator.get_key("mbase.quantization_coefficient", modelInformation.mQuantizationCoefficient);
		seriousGgufConfigurator.get_key("mbase.embedded_system_prompt", modelInformation.mSystemPrompt);
		seriousGgufConfigurator.get_key("mbase.block_count", modelInformation.mBlockCount);
		seriousGgufConfigurator.get_key("mbase.head_count", modelInformation.mHeadCount);
		seriousGgufConfigurator.get_key("mbase.embedding_length", modelInformation.mEmdeddingLength);
		seriousGgufConfigurator.get_key("mbase.model_size", modelInformation.mModelSize);

		std::cout << "Model name: " << modelInformation.mModelName << std::endl;
		std::cout << "Model architecture: " << modelInformation.mModelArchitecture << std::endl;
		std::cout << "Quantization Coefficient: " << modelInformation.mQuantizationCoefficient << std::endl;
		std::cout << "Embedded system prompt: " << modelInformation.mSystemPrompt << std::endl;
		std::cout << "Block count: " << modelInformation.mBlockCount << std::endl;
		std::cout << "KV Head count: " << modelInformation.mHeadCount << std::endl;
		std::cout << "Embedding length: " << modelInformation.mEmdeddingLength << std::endl;
		std::cout << "====" << std::endl;
		std::cout << std::endl;

		mModelInformationMap[modelInformation.mModelName] = modelInformation;
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

InfProgram::flags InfProgram::create_user(const mbase::string& in_username,
	const U32& in_model_access_limit,
	const U32& in_maximum_context_length,
	const bool& in_superuser,
	const bool& in_authorization_locked,
	const mbase::string& in_access_token,
	const U32& in_authority_flags,
	const bool& in_is_permanent,
	mbase::string& out_access_token
)
{
	if(!in_username.size())
	{
		return flags::INF_PROGRAM_ERR_USR_NAME_NOT_GIVEN;
	}

	if(in_username.size() > 512)
	{
		return flags::INF_PROGRAM_ERR_USR_NAME_TOO_LONG;
	}

	if(mUserMap.find(in_username) != mUserMap.end())
	{
		return flags::INF_PROGRAM_ERR_USR_ALREADY_EXISTS;
	}

	U32 modelAccessLimit = gMaipSecurityDefaultModelAccessLimit;
	U32 maximumContextLength = gMaipSecurityDefaultMaximumContextLength;
	U32 authorityFlags = in_authority_flags;
	mbase::string accessToken = in_access_token;

	if(in_access_token.size() > 768)
	{
		accessToken = mbase::string::generate_uuid();
	}

	if(in_model_access_limit)
	{
		modelAccessLimit = in_model_access_limit;
	}

	if(in_maximum_context_length)
	{
		maximumContextLength = in_maximum_context_length;
	}

	InfMaipUser newUser;
	newUser.set_distinct_model_access_limit(modelAccessLimit);
	newUser.set_maximum_context_length(maximumContextLength);
	newUser.set_access_key(accessToken);
	newUser.set_username(in_username);
	newUser.add_authority_flags(authorityFlags);

	if(in_superuser)
	{
		newUser.make_superuser();
	}

	if(in_authorization_locked)
	{
		newUser.lock_authorization();
	}

	mUserMap[in_username] = newUser;

	if(in_is_permanent)
	{
		newUser.update_state_file(mClientStateDirectory, true);
		/*PcState userState;
		mbase::wstring userStateFile = mInferenceConfigurator.get_data_path() + L"states/users/";
		
		std::cout << "Created permanent user: " << std::endl;

		std::cout << "Authority value: " << newUser.get_authority_flags() << std::endl;
		std::cout << "Model access limit: " << newUser.get_model_access_limit() << std::endl;
		std::cout << "Username: " << newUser.get_username() << std::endl;
		std::cout << "Access key: " << newUser.get_access_key() << std::endl;
		std::cout << "Is super: " << newUser.is_superuser() << std::endl;
		std::cout << "Is auth locked: " << newUser.is_authorization_locked() << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;

		userState.initialize(in_username, userStateFile);
		userState.set_state<U32>("authority_flags", newUser.get_authority_flags());
		userState.set_state<U32>("model_access_limit", newUser.get_model_access_limit());
		userState.set_state<U32>("max_context_length", newUser.get_maximum_context_length());
		userState.set_state<mbase::vector<mbase::string>>("accessible_models", newUser.get_accessible_models());
		userState.set_state<mbase::string>("username", newUser.get_username());
		userState.set_state<mbase::string>("access_key", newUser.get_access_key());
		userState.set_state<bool>("is_super", newUser.is_superuser());
		userState.set_state<bool>("is_auth_locked", newUser.is_authorization_locked());

		userState.update();*/
	}

	for(auto& n : mUserMap)
	{
		std::cout << "Username: " << n.first << std::endl;

		InfMaipUser& maipUser = n.second;
		std::cout << "- Context length: " << maipUser.get_maximum_context_length() << std::endl;
		std::cout << "- Model access limit: " << maipUser.get_model_access_limit() << std::endl;
		std::cout << "- Username: " << maipUser.get_username() << std::endl;
		std::cout << "- Access key: " << maipUser.get_access_key() << std::endl;
		std::cout << "- Is superuser: " << maipUser.is_superuser() << std::endl;
		std::cout << "- Is authorization locked: " << maipUser.is_authorization_locked() << std::endl;
	}

	return flags::INF_PROGRAM_SUCCESS;
}

InfProgram::flags InfProgram::update_users_model_access_limit(const mbase::string& in_username, const U32& in_new_access_limit)
{
	inference_user_map::iterator It = mUserMap.find(in_username);
	if (It == mUserMap.end()) 
	{
		return flags::INF_PROGRAM_ERR_USR_NOT_FOUND;
	}

	It->second.set_distinct_model_access_limit(in_new_access_limit);

	for(auto& n : mSessionMap)
	{
		// update all active sessions
		n.second.mMaipUser.set_distinct_model_access_limit(in_new_access_limit);
	}

	return flags::INF_PROGRAM_SUCCESS;
}

InfProgram::flags InfProgram::update_users_maximum_context(const mbase::string& in_username, const U32& in_new_context_length)
{
	inference_user_map::iterator It = mUserMap.find(in_username);
	if (It == mUserMap.end())
	{
		return flags::INF_PROGRAM_ERR_USR_NOT_FOUND;
	}

	It->second.set_maximum_context_length(in_new_context_length);

	for (auto& n : mSessionMap)
	{
		// update all active sessions
		n.second.mMaipUser.set_maximum_context_length(in_new_context_length);
	}

	return flags::INF_PROGRAM_SUCCESS;
}

InfProgram::flags InfProgram::authorize_user_on_model(const mbase::string& in_username, const mbase::string& in_model)
{
	inference_user_map::iterator It = mUserMap.find(in_username);
	if (It == mUserMap.end())
	{
		return flags::INF_PROGRAM_ERR_USR_NOT_FOUND;
	}

	It->second.add_accessible_model(in_model);

	for (auto& n : mSessionMap)
	{
		// update all active sessions
		n.second.mMaipUser.add_accessible_model(in_model);
	}

	return flags::INF_PROGRAM_SUCCESS;
}

GENERIC InfProgram::update_maip_user_sessions(const InfMaipUser& in_maip_user)
{
	InfMaipUser myUser = in_maip_user;
	inference_user_map::iterator It = mUserMap.find(myUser.get_username());

	if(It != mUserMap.end())
	{
		for(accepted_client_map::iterator It = mSessionMap.begin(); It != mSessionMap.end(); ++It)
		{
			if(It->second.mMaipUser.get_username() == myUser.get_username())
			{
				It->second.mMaipUser = myUser;
			}
		}
	}
}

GENERIC InfProgram::update()
{
	for(accepted_client_map::iterator It = mSessionMap.begin(); It != mSessionMap.end();)
	{
		if(!It->second.mPeer->is_connected())
		{
			It = mSessionMap.erase(It);
			continue;
		}
		++It;
	}

	for(registered_model_map::iterator It = mRegisteredModels.begin(); It != mRegisteredModels.end();)
	{
		InfModelTextToText* t2tModel = It->second;
		if(t2tModel->signal_destroy_method())
		{
			mRegisteredModels.erase(It);
		}
		t2tModel->update();
		++It;
	}
}

MBASE_END
