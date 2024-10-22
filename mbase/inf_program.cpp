#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_sampling.h>
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

#define MBASE_BUILD_CSCL_STRING mbase::string::from_format("cs%llu-cl%s", mAccClient.mCsId, mAccClient.mClid.c_str())

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
	managerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	managerClient->mPeer->send_write_signal();
	managerClient->mPeer->send_read_signal();
}

GENERIC InfMaipTunedT2TProcessor::on_initialize_fail(init_fail_code out_code)
{
	mbase::maip_packet_builder tmpPacketBuilder;
	mbase::string outPayload;
	tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_FAILED_TO_CREATE_CONTEXT);
	mNomineeClient->mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mNomineeClient->mManagerClient->mPeer->send_write_signal();
	mNomineeClient->mManagerClient->mPeer->send_read_signal();

	delete mNomineeClient;
	delete this;
}

GENERIC InfMaipTunedT2TProcessor::on_destroy()
{
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
	if(mIsSessionAlive)
	{
		mManagerClient->mChatSessions.erase(mCurrentContextIndex);
	}
	delete this;
}

InfClientSession::~InfClientSession()
{
	chat_session_map::iterator It = mChatSessions.begin();
	for(It; It != mChatSessions.end(); ++It)
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

InfProgram::maip_err_code InfProgram::inf_access_request(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_session_token)
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

InfProgram::maip_err_code InfProgram::inf_create_context(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_model, const U32& in_ctsize, U64& out_ctxId, const mbase::vector<InfSamplingInput>& in_samplers)
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
		return maip_err_code::INF_CONTEXT_LENGTH_EXCEEDED;
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

	for(auto& n : mRegisteredModels)
	{
		out_models.push_back(n.first);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_load_model(const mbase::string& in_session_token, const mbase::string& in_modelname)
{
	MBASE_SESSION_CONTROL;

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_unload_model(const mbase::string& in_session_token, const mbase::string& in_modelname)
{
	MBASE_SESSION_CONTROL;

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

	InfMaipUser newUser;
	newUser.add_authority_flags(authorityFlags);
	newUser.set_username(in_username);
	newUser.set_distinct_model_access_limit(modelAccessLimit);
	newUser.set_maximum_context_length(maximumContextLength);
	newUser.set_access_key(accessToken);

	if(in_superuser)
	{
		// only the super user has authority to make others super user.
		if(clientSession.mMaipUser.is_superuser())
		{
			newUser.make_superuser();
		}
	}
	mUserMap[in_username] = newUser;
	out_access_token = accessToken;
	mbase::string userStateFile = mInferenceConfigurator.get_data_path() + "/states/users/" + in_username;
	
	PcState userState;
	userState.initialize(in_username, mInferenceConfigurator.get_data_path() + "/states/users/");
	
	userState.set_state("authority_flags", newUser.get_authority_flags());
	userState.set_state("model_access_limit", newUser.get_model_access_limit());
	userState.set_state("accessible_models", newUser.get_accessible_models());
	userState.set_state("username", newUser.get_username());
	userState.set_state("access_key", newUser.get_access_key());
	userState.set_state("is_super", newUser.is_superuser());
	userState.set_state("is_auth_locked", newUser.is_authorization_locked());

	userState.update();
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
	InfMaipUser& maipUser = It->second;
	
	accepted_client_map::iterator acceptedClientIt = mSessionMap.begin();
	for(acceptedClientIt; acceptedClientIt != mSessionMap.end();)
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

	mbase::string fileToBeDeleted = mInferenceConfigurator.get_data_path() + "states/users/" + in_username + ".mbfs";
	mbase::delete_file(fileToBeDeleted);
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_model_access_limit(const mbase::string& in_session_token, const U32& in_new_access_limit)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_maximum_context_length(const mbase::string& in_session_token, const U32& in_maximum_context_length)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_make_superuser(const mbase::string& in_session_token)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_unmake_superuser(const mbase::string & in_session_token)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_accept_models(const mbase::string & in_session_token, const mbase::vector<mbase::string>&in_models)
{
	MBASE_SESSION_CONTROL;
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_set_authority_flags(const mbase::string & in_session_token, const mbase::vector<mbase::string>&in_authority_flags)
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
	if(hostProcessor->next() == InfProcessorBase::flags::INF_PROC_ERR_INPUT_IS_EMPTY)
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

GENERIC InfProgram::update()
{
	for(auto& modelMap : mRegisteredModels)
	{
		modelMap.second->update();
	}
}

MBASE_END
