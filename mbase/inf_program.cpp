#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_sampling.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/maip_parser.h>
#include <mbase/pc/pc_state.h>
#include <mbase/filesystem.h>
#include <random>
#include <algorithm>
#include <iostream>

MBASE_BEGIN

#define MBASE_SESSION_CONTROL \
if(!this->is_session_token_valid(in_session_token))\
{\
	return maip_err_code::INF_SESSION_TOKEN_MISMATCH;\
}\
InfClientSession& clientSession = mSessionMap[in_session_token];

#define MBASE_DESC_MODIF_CONTROL \
maip_err_code result = common_description_modification_control(clientSession, in_model_target);\
if(result != maip_err_code::INF_SUCCESS)\
{\
	return result;\
}

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

InfMaipTunedT2TProcessor::InfMaipTunedT2TProcessor(InfClientSession& in_client)
{
    mManagerClient = &in_client;
}

GENERIC InfMaipTunedT2TProcessor::on_initialize()
{
    mProcessorClient.set_session(*mManagerClient);
	set_inference_client(&mProcessorClient);
}

GENERIC InfMaipTunedT2TProcessor::on_initialize_fail(last_fail_code out_code)
{
	mbase::maip_packet_builder tmpPacketBuilder;
	mbase::string outPayload;

    tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_NOT_ENOUGH_MEMORY);
	tmpPacketBuilder.generate_payload(outPayload);
	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->send_write_signal();
	mManagerClient->mPeer->send_read_signal();

	delete this;
}

GENERIC InfMaipTunedT2TProcessor::on_destroy()
{
    // If this guy is called,
    // It means that the model it belongs to is being destroyed 

	mbase::maip_packet_builder tmpPacketBuilder;
	mbase::string outPayload;
	tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_ABANDONED);
	tmpPacketBuilder.generate_payload(outPayload);
	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->send_write_signal();
	mManagerClient->mPeer->send_read_signal();
	delete this;
}

InfMaipTunedEmbedderProcessor::InfMaipTunedEmbedderProcessor(InfClientSession& in_client)
{
    mManagerClient = &in_client;    
}

GENERIC InfMaipTunedEmbedderProcessor::on_initialize_fail(init_fail_code out_code)
{
    mbase::maip_packet_builder tmpPacketBuilder;
	mbase::string outPayload;

    tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_NOT_ENOUGH_MEMORY);
	tmpPacketBuilder.generate_payload(outPayload);
	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->send_write_signal();
	mManagerClient->mPeer->send_read_signal();

	delete this;
}

GENERIC InfMaipTunedEmbedderProcessor::on_initialize()
{   
    mProcessorClient.set_session(*mManagerClient);
	set_inference_client(&mProcessorClient);
}

GENERIC InfMaipTunedEmbedderProcessor::on_destroy()
{
    mbase::maip_packet_builder tmpPacketBuilder;
	mbase::string outPayload;
	tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_ABANDONED);
	tmpPacketBuilder.generate_payload(outPayload);
	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->send_write_signal();
	mManagerClient->mPeer->send_read_signal();
	delete this;
}

InfMaipTunedClient::InfMaipTunedClient() : lastToken(), mManagerClient(NULL), mCurrentContextIndex(0)
{
}

GENERIC InfMaipTunedClient::set_session(InfClientSession& in_session)
{
    mManagerClient = &in_session;
}

GENERIC InfMaipTunedClient::set_embedder_message_queue(mbase::vector<U32>& in_msg_ids)
{
    while(mEmbeddingMessageIndexes.size())
    {
        // Empty the embedding message index queue
        mEmbeddingMessageIndexes.pop();
    }

    for(mbase::vector<U32>::iterator It = in_msg_ids.begin(); It != in_msg_ids.end(); ++It)
    {
        mEmbeddingMessageIndexes.push(*It);
    }
}

bool InfMaipTunedClient::proc_next_embedding()
{
    if(!mEmbeddingMessageIndexes.size())
    {
        return false;
    }

    InfProcessorBase* hostProc = NULL;
    get_host_processor(hostProc);

    InfEmbedderProcessor* embedProc = static_cast<InfEmbedderProcessor*>(hostProc);
    mbase::context_line cl;
    get_message(mEmbeddingMessageIndexes.front(), cl);

    mEmbeddingMessageIndexes.pop();

    inf_text_token_vector tokVec;
    embedProc->tokenize_input(cl.mMessage.c_str(), cl.mMessage.size(), tokVec);
    embedProc->execute_input(tokVec, true);

    return true;
}

GENERIC InfMaipTunedClient::on_register(InfProcessorBase* out_processor)
{
    mManagerClient->mChatSessions[mManagerClient->mContextCounter] = out_processor;
    mManagerClient->mContextCounter++;

    mbase::maip_packet_builder tmpPacketBuilder;
	mbase::string outPayload;
	tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::INF_SUCCESS);
	tmpPacketBuilder.set_kval("CTXID", mManagerClient->mContextCounter - 1);
	tmpPacketBuilder.generate_payload(outPayload);
	mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->send_write_signal();
	mManagerClient->mPeer->send_read_signal();
}

GENERIC InfMaipTunedClient::on_embedding_data(const F32* out_data, size_type out_size)
{
    mbase::string totalEmbeddingString;
    for(size_type i = 0; i < out_size; i++)
    {
        totalEmbeddingString += mbase::string::from_format("%f;", out_data[i]);
    }

    if(totalEmbeddingString.size())
    {
        totalEmbeddingString.pop_back(); // Remove the last semicolon
    }
    
    mbase::maip_packet_builder tmpPacketBuilder;
    tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_FINISH);
    if(mEmbeddingMessageIndexes.size())
    {
        // There are still messages in the embedding message list
        tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_CONTINUE);
    }

    mbase::string outPayload;
    tmpPacketBuilder.generate_payload(outPayload, totalEmbeddingString);
    mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	mManagerClient->mPeer->send_write_signal();
	mManagerClient->mPeer->send_read_signal();
}

GENERIC InfMaipTunedClient::on_write(const inf_text_token_vector& out_token, bool out_is_finish)
{	
	// mbase::string outData(out_data, out_size);
	// mbase::maip_packet_builder tmpPacketBuilder;
	
	// if(!out_is_finish)
	// {
	// 	tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_CONTINUE);
	// }

	// else
	// {
	// 	// on_finish will be called
	// 	lastToken = outData;
	// 	return;
	// }

	// mbase::string outPayload;

	// if(out_is_special)
	// {
	// 	tmpPacketBuilder.set_kval("SPECIAL", 1);
	// }

	// else
	// {
	// 	tmpPacketBuilder.set_kval("SPECIAL", 0);
	// }

	// if(outData.size())
	// {
	// 	tmpPacketBuilder.generate_payload(outPayload, outData);
	// }

	// else
	// {
	// 	tmpPacketBuilder.generate_payload(outPayload);
	// }

	// mManagerClient->mPeer->write_data(outPayload.c_str(), outPayload.size());
	// mManagerClient->mPeer->send_write_signal();
	// mManagerClient->mPeer->send_read_signal();
}

GENERIC InfMaipTunedClient::on_finish(size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state)
{
	mbase::maip_packet_builder tmpPacketBuilder;
	if(out_finish_state == InfTextToTextProcessor::finish_state::FINISHED)
	{
		tmpPacketBuilder.set_kval("TOKCOUNT", out_total_token_size);
		tmpPacketBuilder.set_response_message((U16)InfProgram::maip_err_code::EXEC_MESSAGE_FINISH);
	}

	else if(out_finish_state == InfTextToTextProcessor::finish_state::TOKEN_LIMIT_REACHED)
	{
		InfProcessorBase* hostProcessor = NULL;
		get_host_processor(hostProcessor); // 100% success;
        InfTextToTextProcessor* t2tProc = static_cast<InfTextToTextProcessor*>(hostProcessor);
		tmpPacketBuilder.set_kval("MAXTOK", t2tProc->get_max_token_length());
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
}

InfClientSession::~InfClientSession()
{
	for(chat_session_map::iterator It = mChatSessions.begin(); It != mChatSessions.end(); ++It)
	{
		InfProcessorBase* tProc = It->second;
		if(tProc)
		{
			delete tProc;
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
	// === Writing behavior of the method verbally to detect bugs or problems === 
	// *** inf_access_request ***
	// 1- Looking for a given username in inference_user_map structure
	// 2- If the user does not exists, return INF_AUTHORIZATION_FAILED(2008)
	// 3- Checking if the supplied access token match the access token of the user group.
	// 4- If it does not match the user group's access token, return INF_AUTHORIZATION_FAILED(2008)
	// 5- Creating a new InfClientSession.
	
	// CHECK NOTE: DO NOT NEED TO CHECK ANYMORE, IT WORKS AS INTENDED

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
	// === Writing behavior of the method verbally to detect bugs or problems === 
	// *** inf_destroy_session ***
	// 1- Checking if the supplied session token matche any InfClientSession
	// 2- If it matches, erase the corresponding session from the session map
	// 3- If it does not match, return INF_SUCCESS without doing anything

	// CHECK NOTE: DO NOT NEED TO CHECK ANYMORE, IT WORKS AS INTENDED

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

	// === Writing behavior of the method verbally to detect bugs or problems ===
	// *** inf_get_accessible_models ***
	// 1- Get accessible models set and push it to the out models vector
	
	// CHECK NOTE: DO NOT NEED TO CHECK ANYMORE, IT WORKS AS INTENDED

	const InfMaipUser::model_name_set& accModels = clientSession.mMaipUser.get_accessible_models();
	for(auto& n : accModels)
	{
		out_models.push_back(n);
	}
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_context_ids(const mbase::string& in_session_token, mbase::vector<U64>& out_contexts)
{
	MBASE_SESSION_CONTROL;
	
	// === Writing behavior of the method verbally to detect bugs or problems ===
	// *** inf_get_accessible_models ***
	// 1- Iterating over chat session map and push each 'key(context id)' to the U64 vector
	
	// CHECK NOTE: DO NOT NEED TO CHECK ANYMORE, IT WORKS AS INTENDED

	for(auto& n : clientSession.mChatSessions)
	{
		out_contexts.push_back(n.first);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_create_context(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_model, const U32& in_ctsize, const mbase::vector<InfSamplingInput>& in_samplers)
{
	MBASE_SESSION_CONTROL;
	
	// === Writing behavior of the method verbally to detect bugs or problems ===
	// *** inf_create_context ***
	// 1- Check if the requested model is in the accessible models list of the user
	// 2- If not, return INF_MODEL_IS_NOT_ACCESSIBLE(2014)
	// 3- Check if the given context size is less then the minimum threshold(32 in this case)
	// 4- If it is, return INF_INVALID_TOKEN_LIMIT(2006)
	// 5- Check if the given model name exists in registered_model_map.
	// 6- If not, return INF_MODEL_NAME_MISMATCH(2004)
	// 7- Check if the given context size is greater than the user group's context size limit
	// 8- If it is, return INF_USER_CONTEXT_LENGTH_EXCEEDED(2020)
	// 9- Check if the available context length of the model is sufficent to create the given context
	// 10- If not, return INF_MODEL_CONTEXT_FULL(2019)
	// 11- If the model is embedding model, create and register embedder processor
	// 12- If the model is not embedding model, create and register T2T processor

	// CHECK NOTE: CHECK THE handlers on_initialize and on_initialize_fail then come back here again.

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
    clientSession.mPeer = in_peer;

	if(t2tModel->is_embedding_model())
	{
		InfMaipTunedEmbedderProcessor* maipEmbedderContext = new InfMaipTunedEmbedderProcessor(clientSession);
		t2tModel->register_context_process(
			maipEmbedderContext,
			in_ctsize,
			clientSession.mMaipUser.get_batch_size(),
			clientSession.mMaipUser.get_processor_thread_count()
		); // 100% SUCCESS
	}
	else
	{
		InfMaipTunedT2TProcessor* maipT2tContext = new InfMaipTunedT2TProcessor(clientSession);
		t2tModel->register_context_process(
			maipT2tContext,
			in_ctsize,
			clientSession.mMaipUser.get_batch_size(),
			clientSession.mMaipUser.get_processor_thread_count(),
			true,
			{}
		); // 100% SUCCESS
	}
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_clear_context_history(const mbase::string& in_session_token, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;
	
	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	// if(It != clientSession.mChatSessions.end())
	// {
	// 	It->second->clear_chat_history();
	// }

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_context_status(const mbase::string& in_session_token, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	// InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	// if(It == clientSession.mChatSessions.end())
	// {
	// 	return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	// }

	// InfTextToTextProcessor* hostProc = NULL;
	// It->second->get_host_processor(hostProc); // 100% success

	// InfTextToTextProcessor::flags outStatus = hostProc->get_processor_status();

	// if(outStatus == InfTextToTextProcessor::flags::INF_PROC_INFO_INITIALIZING)
	// {
	// 	return maip_err_code::INF_CONTEXT_INITIALIZING;
	// }

	// if(hostProc->is_available())
	// {
	// 	return maip_err_code::INF_CONTEXT_INACTIVE;
	// }

	// else
	// {
	// 	return maip_err_code::INF_CONTEXT_ACTIVE;
	// }
	return maip_err_code::INF_CONTEXT_ACTIVE;
}

InfProgram::maip_err_code InfProgram::inf_destroy_context(const mbase::string& in_session_token, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	// === Writing behavior of the method verbally to detect bugs or problems ===
	// *** inf_destroy_context ***
	// 1- Check if the given context id match any chat session
	// 2- If not, return INF_CONTEXT_ID_MISMATCH(2005)
	// 3- Delete the corresponding processor
	// 4- Erase that context id from chat_session_map

	// CHECK NOTE: DO NOT NEED TO CHECK ANYMORE, IT WORKS AS INTENDED

	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	if (It == clientSession.mChatSessions.end())
	{
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}
	
	delete It->second;
    clientSession.mChatSessions.erase(in_ctxId);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_get_program_models(const mbase::string& in_session_token, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;

	// CHECK NOTE: DO NOT NEED TO CHECK ANYMORE, IT WORKS AS INTENDED

	for(auto& n : mModelDescriptionMap)
	{
		out_models.push_back(n.first);
	}

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_load_model(const mbase::string& in_session_token, const mbase::string& in_modelname, const U32& in_total_context_size)
{
	MBASE_SESSION_CONTROL;

	// === Writing behavior of the method verbally to detect bugs or problems ===
	// *** inf_load_model ***
	// 1- Check if the client user have an authority to load/unload models
	// 2- If not, return INF_AUTHORIZATION_FAILED(2008)
	// 3- If the total context size is not supplied, return INF_INVALID_PARAMS(2016)
	// 4- Check if the given model name is in the registered models map
	// 5- If so, return INF_MODEL_ALREADY_LOADED(2025)
	// 6- If the given model name is not found in the model description map, return INF_MODEL_NAME_MISMATCH(2004)

	// CHECK NOTE: This method must own the client socket, implement that and come back here again

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
			return maip_err_code::INF_MODEL_ALREADY_LOADED;
		}
	}

	for(auto& n : mModelDescriptionMap)
	{
		if(n.first == in_modelname)
		{
			InfMaipModel* newModel = new InfMaipModel(n.second.get_custom_name(), *this);
			newModel->initialize_model(mbase::from_utf8(n.second.get_model_file()), in_total_context_size, 999);
			mRegisteredModels[n.second.get_custom_name()] = newModel;
			return maip_err_code::INF_SUCCESS;
		}
	}

	return maip_err_code::INF_MODEL_NAME_MISMATCH;
}

InfProgram::maip_err_code InfProgram::inf_unload_model(const mbase::string& in_session_token, const mbase::string& in_modelname)
{
	MBASE_SESSION_CONTROL;

	// === Writing behavior of the method verbally to detect bugs or problems ===
	// *** inf_unload_model ***
	// 1- Check if the client user have an authority to load/unload models
	// 2- If not, return INF_AUTHORIZATION_FAILED(2008)

	// CHECK NOTE: Works fine, do not come back

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
	t2tModel->destroy();

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_create_new_user(
	const mbase::string& in_session_token,
	const mbase::string& in_username,
	const mbase::string& in_access_token,
	const mbase::string& in_system_prompt,
	const U32& in_model_access_limit,
	const U32& in_maximum_context_length,
	const U32& in_batch_length,
	const U32& in_max_proc_threads,
	const U32& in_proc_threads,
	const bool& in_superuser,
	const bool& in_is_static,
	const mbase::vector<mbase::string>& in_authority_flags,
	mbase::string& out_access_token
)
{
	MBASE_SESSION_CONTROL;

	// === Writing behavior of the method verbally to detect bugs or problems ===
	// *** inf_create_user ***
	// 1- If the username is not given, return INF_INVALID_PARAMS(2016)
	// 2- If the client have no authority to create/delete users, return INF_AUTHORIZATION_FAILED(2008)
	// 3- If the given username already exists on the user map, reutrn INF_USER_ALREADY_EXISTS(2015)
	// 4- If the access token is not supplied, generate a uuid and assign it as an access token
	// 5- If the in_superuser is set and the client is not a superuser, return INF_AUTHORIZATION_FAILED(2008)

	// CHECK NOTE: Works fine, do not come back

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
		else if(n == "USER_CD")
		{
			authorityFlags |= MAIP_USER_CREATE_DELETE;
		}
		else if(n == "STATIC")
		{
			authorityFlags |= MAIP_USER_STATIC;
		}
		else if(n == "USER_MODIFY")
		{
			authorityFlags |= MAIP_USER_MODIFICATION;
		}
		else if(n == "CTX_MODIFY")
		{
			authorityFlags |= MAIP_USER_CONTEXT_LENGTH_MODIFICATION;
		}
		else if(n == "ACCESIBILITY")
		{
			authorityFlags |= MAIP_USER_ACCESS_MODIFICATION;
		}
		else if(n == "SYS_PROMPT")
		{
			authorityFlags |= MAIP_USER_SYS_PROMPT_MODIFICATION;
		}
		else if(n == "SAMPLING_SET")
		{
			authorityFlags |= MAIP_USER_SAMPLER_MODIFICATION;
		}
		else if(n == "MAX_THREAD_SET")
		{
			authorityFlags |= MAIP_USER_MAX_PROCESSOR_THREAD_COUNT_MODIFICATION;
		}
		else if(n == "THREAD_SET")
		{
			authorityFlags |= MAIP_USER_PROCESSOR_THREAD_COUNT_MODIFICATION;
		}
		else if(n == "BATCH_SET")
		{
			authorityFlags |= MAIP_USER_BATCH_LENGTH_MODIFICATION;
		}
	}

	if(in_superuser && !clientSession.mMaipUser.is_superuser())
	{
		// if the client session is not the super user but trying to create a super user, stop!
		// only super users can create super users.
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	create_user( // 100% success
		in_username, 
		accessToken,
		in_system_prompt,
		authorityFlags,
		in_model_access_limit,
		in_maximum_context_length,
		in_batch_length,
		in_max_proc_threads,
		in_proc_threads,
		in_superuser,
		in_is_static,
		out_access_token
	);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_delete_user(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_access_token)
{
	MBASE_SESSION_CONTROL;

	// 1- Check if the MAIP_USER_CREATE_DELETE flag is set. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the deleted user is self. If so, INF_CANT_DELETE_SELF
	// 3- Check if the deleted user is super user. If so and the current session is not a super user, INF_AUTHORIZATION_FAILED
	// 4- Check if the specified user exists. If not, INF_USER_NOT_FOUND
	// 5- Erase all active sessions for the given user.
	// 6- Remove the user from the user map
	// 7- Delete the corresponding user state file.

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
	if(It == mUserMap.end())
	{		
		return maip_err_code::INF_USER_NOT_FOUND;
	}

	InfMaipUser& maipUser = It->second;
	if(maipUser.is_superuser() && !clientSession.mMaipUser.is_superuser())
	{
		// If the client attempts to delete a super user and he is not a super user, 
		// Authorization fails.
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	if(maipUser.get_access_key() != in_access_token)
	{
		// Means access key is invalid
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

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

	mbase::wstring fileToBeDeleted = mClientStateDirectory + mbase::from_utf8(usernameSanitized) + L".mbfs";
	mbase::delete_file(fileToBeDeleted);
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_model_access_limit(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_new_access_limit)
{
	// 1- Check if the MAIP_USER_ACCESS_MODIFICATION flag is set. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Check if the specified user is static. If so and if the current session is not superuser, INF_AUTHORIZATION_FAILED
	// 3- Check if the specified user is superuser. If so and if the current session is not a superuser, INF_AUTHORIZATION_FAILED
	// 4- Set model access limit for the given user
	// 5- Update user's state file
	// 6- Update all active sessions associated with the given user.

	MBASE_SESSION_CONTROL;

	maip_err_code result = common_modification_control(clientSession, in_username, MAIP_USER_ACCESS_MODIFICATION);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	InfMaipUser& maipUser = mUserMap[in_username]; // Guaranteed success

	maipUser.set_distinct_model_access_limit(in_new_access_limit);
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_maximum_context_length(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_maximum_context_length)
{
	// 1- Check if the MAIP_USER_CONTEXT_LENGTH_MODIFICATION is set. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Check if the specified user is static. If so and if the current session is not superuser, INF_AUTHORIZATION_FAILED
	// 3- Check if the specified user is superuser. If so and if the current session is not a superuser INF_AUTHORIZATION_FAILED
	// 4- Set context length limit for the given user.
	// 5- Update user's state file
	// 6- Update all active sessions associated with the given user.

	MBASE_SESSION_CONTROL;

	maip_err_code result = common_modification_control(clientSession, in_username, MAIP_USER_CONTEXT_LENGTH_MODIFICATION);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	InfMaipUser& maipUser = mUserMap[in_username];
	maipUser.set_maximum_context_length(in_maximum_context_length);
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_batch_size(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_batch_size)
{
	// 1- Check if the MAIP_USER_BATCH_LENGTH_MODIFICATION is set. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Check if the specified user is static. If so and if the current session is not superuser, INF_AUTHORIZATION_FAILED
	// 4- Check if the specified user is superuser. If so and if the current session is not a superuser INF_AUTHOIZATION_FAILED
	// 5- Check if the given batch size is greater than context length of the group. If so, batch_length = context_length
	// 6- Set batch size for the group.
	// 7- Update user's state file
	// 8- Update all active sessions associated with the given user.

	MBASE_SESSION_CONTROL;

	maip_err_code result = common_modification_control(clientSession, in_username, MAIP_USER_BATCH_LENGTH_MODIFICATION);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	InfMaipUser& maipUser = mUserMap[in_username];
	U32 tmpBatchSize = in_batch_size;
	if(tmpBatchSize > maipUser.get_maximum_context_length())
	{
		tmpBatchSize = maipUser.get_maximum_context_length();
	}

	maipUser.set_batch_size(tmpBatchSize);
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_processor_thread_count(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_thread_count)
{
	// 1- Check if the MAIP_USER_PROCESSOR_MODIFICATION is set. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Check if the specified user is static. If so and if the current session is not superuser, INF_AUTHORIZATION_FAILED
	// 4- Check if the specified user is superuser. If so and if the current session is not superuser, INF_AUTHORIZATION_FAILED
	// 5- Set processor thread count for the group
	// 6- Update user's state file
	// 7- Update all active sessions associated with the given user.

	MBASE_SESSION_CONTROL;

	maip_err_code result = common_modification_control(clientSession, in_username, MAIP_USER_PROCESSOR_THREAD_COUNT_MODIFICATION);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	InfMaipUser& maipUser = mUserMap[in_username];
	U32 tmpThreadCount = in_thread_count;

	if(tmpThreadCount > maipUser.get_processor_max_thread_count())
	{
		tmpThreadCount = maipUser.get_processor_max_thread_count();
	}

	maipUser.set_processor_thread_count(tmpThreadCount);
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_max_processor_thread_count(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_thread_count)
{
	MBASE_SESSION_CONTROL;

	maip_err_code result = common_modification_control(clientSession, in_username, MAIP_USER_MAX_PROCESSOR_THREAD_COUNT_MODIFICATION);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	InfMaipUser& maipUser = mUserMap[in_username];

	maipUser.set_processor_max_thread_count(in_thread_count);
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

// maip_err_code inf_modify_user_sampling_set(const mbase::string& in_session_token, const mbase::string& in_username, const inf_sampling_set& in_sampling_set) /* Implement */
InfProgram::maip_err_code InfProgram::inf_modify_user_system_prompt(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_system_prompt)
{
	MBASE_SESSION_CONTROL;

	maip_err_code result = common_modification_control(clientSession, in_username, MAIP_USER_SYS_PROMPT_MODIFICATION);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	InfMaipUser& maipUser = mUserMap[in_username];

	maipUser.set_system_prompt(in_system_prompt);
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_make_superuser(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_access_token)
{
	// 1- Check if the current session is superuser. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Check if the access token match the user's access token
	// 3- Make the given user a superuser.
	// 4- Update user's state file
	// 5- Update all active sessions associated with the given user.

	MBASE_SESSION_CONTROL;

	if(!clientSession.mMaipUser.is_superuser())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	inference_user_map::iterator It = mUserMap.find(in_username);
	if(It == mUserMap.end())
	{
		return maip_err_code::INF_USER_NOT_FOUND;
	}

	InfMaipUser& maipUser = It->second;

	if(maipUser.get_access_key() != in_access_token)
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	maipUser.make_superuser();
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_unmake_superuser(const mbase::string & in_session_token, const mbase::string& in_username, const mbase::string& in_access_token)
{
	// 1- Check if the current session is superuser. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Take away the superuser status of the given user.
	// 4- Update user's state file
	// 5- Update all active sessions associated with the given user.

	MBASE_SESSION_CONTROL;

	if(!clientSession.mMaipUser.is_superuser())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	inference_user_map::iterator It = mUserMap.find(in_username);
	if(It == mUserMap.end())
	{
		return maip_err_code::INF_USER_NOT_FOUND;
	}

	InfMaipUser& maipUser = It->second;

	if(maipUser.get_access_key() != in_access_token)
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	maipUser.unmake_superuser();
	update_maip_user_sessions(maipUser);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_accept_models(const mbase::string& in_session_token, const mbase::string& in_username, const std::set<mbase::string>& in_models, mbase::vector<mbase::string>& out_missing_models)
{
	// 1- Check if the MAIP_USER_ACCESS_MODIFICATION flag is set. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Check if the specified user is static. If so and if the current session is not superuser, INF_AUTHORIZATION_FAILED
	// 3- Check if the specified model already exists in the accepted models list. If so, INF_SUCCESS
	// 4- Check if the specified model is in the model information map. If not, INF_MODEL_NAME_MISMATCH
	// 5- Check if the user group is prohibited to access the target model. If so, INF_TARGET_MODEL_ACCESS_PROHIBITED
	// 6- Append the given model to users model access list
	// 7- Update user's state file
	// 8- Update all active sessions associated with the given user.

	MBASE_SESSION_CONTROL;

	maip_err_code result = common_modification_control(clientSession, in_username, MAIP_USER_ACCESS_MODIFICATION);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	InfMaipUser& maipUser = mUserMap[in_username];
	for(std::set<mbase::string>::iterator It = in_models.begin(); It != in_models.end(); It++)
	{
		if(mModelDescriptionMap.find(*It) == mModelDescriptionMap.end())
		{
			out_missing_models.push_back(*It);
		}
	}

	if(out_missing_models.size())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	for(std::set<mbase::string>::iterator It = in_models.begin(); It != in_models.end(); It++)
	{
		maipUser.add_accessible_model(*It);
	}

	update_maip_user_sessions(maipUser);
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_user_set_authority_flags(const mbase::string & in_session_token, const mbase::string& in_username, const mbase::vector<mbase::string>&in_authority_flags)
{
	// 1- Check if the current session is super user. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Update user authority flags
	// 4- Update user's state file
	// 5- Update all active sessions associated with the given user

	MBASE_SESSION_CONTROL;

	if(!clientSession.mMaipUser.is_superuser())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	inference_user_map::iterator It = mUserMap.find(in_username);
	if(It == mUserMap.end())
	{
		return maip_err_code::INF_USER_NOT_FOUND;
	}

	InfMaipUser& maipUser = It->second;

	for(mbase::vector<mbase::string>::const_iterator cIt = in_authority_flags.cbegin(); cIt != in_authority_flags.cend(); ++cIt)
	{
		const mbase::string& n = *cIt;
		if (n == "LOAD")
		{
			maipUser.add_authority_flags(MAIP_MODEL_LOAD_UNLOAD);
		}
		else if (n == "ADAPTER")
		{
			maipUser.add_authority_flags(MAIP_ADAPTER_LOAD_UNLOAD);
		}
		else if(n == "USER_CD")
		{
			maipUser.add_authority_flags(MAIP_USER_CREATE_DELETE);
		}
		else if(n == "STATIC")
		{
			maipUser.add_authority_flags(MAIP_USER_STATIC);
		}
		else if(n == "USER_MODIF")
		{
			maipUser.add_authority_flags(MAIP_USER_MODIFICATION);
		}
		else if(n == "CTX_MODIF")
		{
			maipUser.add_authority_flags(MAIP_USER_CONTEXT_LENGTH_MODIFICATION);
		}
		else if(n == "ACCESIBILITY")
		{
			maipUser.add_authority_flags(MAIP_USER_ACCESS_MODIFICATION);
		}
		else if(n == "SYS_PROMPT")
		{
			maipUser.add_authority_flags(MAIP_USER_SYS_PROMPT_MODIFICATION);
		}
		else if(n == "SAMPLING_SET")
		{
			maipUser.add_authority_flags(MAIP_USER_SAMPLER_MODIFICATION);
		}
		else if(n == "MAX_THREAD_SET")
		{
			maipUser.add_authority_flags(MAIP_USER_MAX_PROCESSOR_THREAD_COUNT_MODIFICATION);
		}
		else if(n == "THREAD_SET")
		{
			maipUser.add_authority_flags(MAIP_USER_PROCESSOR_THREAD_COUNT_MODIFICATION);
		}
		else if(n == "BATCH_SET")
		{
			maipUser.add_authority_flags(MAIP_USER_BATCH_LENGTH_MODIFICATION);
		}
	}

	update_maip_user_sessions(maipUser);
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_create_model_description(
		const mbase::string& in_session_token,
		const mbase::string& in_original_name,
		const mbase::string& in_custom_name,
		const mbase::string& in_description,
		const mbase::string& in_system_prompt,
		const mbase::string& in_model_file,
		const mbase::vector<mbase::string>& in_tags,
		const mbase::string& in_category,
		const bool& in_is_embedding_model,
		const bool& in_force_system_prompt,
		const U32& in_total_context_length
)
{
	MBASE_SESSION_CONTROL;

	maip_err_code result = common_modification_control(clientSession, clientSession.mMaipUser.get_username(), MAIP_MODEL_LOAD_UNLOAD);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	if(!in_original_name.size() || !in_model_file.size())
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	mbase::string tmpCustomName = in_custom_name;
	InfMaipModelDescription::CATEGORY ctg;
	if(!tmpCustomName.size())
	{
		// If the custom name does not exists,
		// custom name is equal to the original name
		tmpCustomName = in_original_name;
	}

	if(in_category == "T2T")
	{
		ctg = InfMaipModelDescription::CATEGORY::TEXT_TO_TEXT;
	}

	else if(in_category == "EMBD")
	{
		ctg = InfMaipModelDescription::CATEGORY::EMBEDDING;
	}

	else 
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	if(mModelDescriptionMap.find(tmpCustomName) != mModelDescriptionMap.end())
	{
		return maip_err_code::INF_DESCRIPTION_ALREADY_EXISTS;
	}

	InfMaipModelDescription imd;
	imd.set_original_name(in_original_name);
	imd.set_custom_name(in_custom_name);
	imd.set_description(in_description);
	imd.set_system_prompt(in_system_prompt);
	imd.set_model_file(in_model_file);
	imd.set_tags(in_tags);
	imd.set_category(ctg);
	imd.set_embedding(in_is_embedding_model);
	imd.set_force_system_prompt(in_force_system_prompt);
	imd.set_maximum_context_length(in_total_context_length);
	imd.update_state_file(mDescriptionDirectory);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_original_model_name(
	const mbase::string& in_session_token,
	const mbase::string& in_model_target,
	const mbase::string& in_name
)
{
	MBASE_SESSION_CONTROL;

	if(!in_name.size())
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	MBASE_DESC_MODIF_CONTROL;

	InfMaipModelDescription& currentDescription = mModelDescriptionMap[in_model_target];
	currentDescription.set_original_name(in_name);
	currentDescription.update_state_file(mDescriptionDirectory);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_custom_model_name(
	const mbase::string& in_session_token,
	const mbase::string& in_model_target,
	const mbase::string& in_name
)
{
	MBASE_SESSION_CONTROL;

	if(!in_name.size())
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	MBASE_DESC_MODIF_CONTROL;
	
	InfMaipModelDescription& currentDescription = mModelDescriptionMap[in_model_target];
	if(in_name == currentDescription.get_custom_name())
	{
		return maip_err_code::INF_SUCCESS;
	}

	currentDescription.set_custom_name(in_name);
	mbase::delete_file(mDescriptionDirectory + mbase::from_utf8(in_name)); // Delete the old description file
	currentDescription.update_state_file(mDescriptionDirectory);

	InfMaipModelDescription newDescription = currentDescription;
	mModelDescriptionMap.erase(in_model_target);
	mModelDescriptionMap[in_name] = newDescription;

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_model_description(
	const mbase::string& in_session_token,
	const mbase::string& in_model_target,
	const mbase::string& in_description
)
{
	MBASE_SESSION_CONTROL;
	MBASE_DESC_MODIF_CONTROL;

	InfMaipModelDescription& currentDescription = mModelDescriptionMap[in_model_target];
	if(currentDescription.get_description() == in_description)
	{
		return maip_err_code::INF_SUCCESS;
	}

	currentDescription.set_description(in_description);
	currentDescription.update_state_file(mDescriptionDirectory);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_model_system_prompt(
	const mbase::string& in_session_token,
	const mbase::string& in_model_target,
	const mbase::string& in_system_prompt
)
{
	MBASE_SESSION_CONTROL;
	MBASE_DESC_MODIF_CONTROL;

	InfMaipModelDescription& currentDescription = mModelDescriptionMap[in_model_target];
	if(currentDescription.get_system_prompt() == in_system_prompt)
	{
		return maip_err_code::INF_SUCCESS;
	}

	currentDescription.set_system_prompt(in_system_prompt);
	currentDescription.update_state_file(mDescriptionDirectory);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_model_model_file(
	const mbase::string& in_session_token,
	const mbase::string& in_model_target,
	const mbase::string& in_model_file,
	const bool& in_is_embedding,
	const mbase::string& in_model_category
)
{
	MBASE_SESSION_CONTROL;
	MBASE_DESC_MODIF_CONTROL;

	if(!in_model_file.size())
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	InfMaipModelDescription::CATEGORY ctg;
	if(in_model_category == "T2T")
	{
		ctg = InfMaipModelDescription::CATEGORY::TEXT_TO_TEXT;
	}

	else if(in_model_category == "EMBD")
	{
		ctg = InfMaipModelDescription::CATEGORY::EMBEDDING;
	}

	else
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	InfMaipModelDescription& currentDescription = mModelDescriptionMap[in_model_target];
	currentDescription.set_model_file(in_model_file);
	currentDescription.set_category(ctg);
	currentDescription.set_embedding(in_is_embedding);
	currentDescription.update_state_file(mDescriptionDirectory);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_model_tags(
	const mbase::string& in_session_token,
	const mbase::string& in_model_target,
	const mbase::vector<mbase::string>& in_tags
)
{
	MBASE_SESSION_CONTROL;
	MBASE_DESC_MODIF_CONTROL;

	InfMaipModelDescription& currentDescription = mModelDescriptionMap[in_model_target];
	currentDescription.set_tags(in_tags);
	currentDescription.update_state_file(mDescriptionDirectory);

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_modify_model_context_length(
	const mbase::string& in_session_token,
	const mbase::string& in_model_target,
	const U32& in_maximum_context
)
{
	MBASE_SESSION_CONTROL;
	MBASE_DESC_MODIF_CONTROL;

	if(in_maximum_context < 64)
	{
		return maip_err_code::INF_INVALID_PARAMS;
	}

	InfMaipModelDescription& currentDescription = mModelDescriptionMap[in_model_target];
	currentDescription.set_maximum_context_length(in_maximum_context);
	currentDescription.update_state_file(mDescriptionDirectory);

	// TODO: UPDATE LOADED MODELS CONTEXT LENGTH

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

	InfProcessorBase* tmpProc = It->second;
    InfMaipTunedClient* mtClient = NULL;

    if(tmpProc->get_processor_type() == InfProcessorBase::processor_type::TEXT_TO_TEXT)
    {
        InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(tmpProc);
        mtClient = static_cast<InfMaipTunedClient*>(t2tProcessor->get_assigned_client()); // 100% success;
    }

    else if(tmpProc->get_processor_type() == InfProcessorBase::processor_type::EMBEDDER)
    {
        InfEmbedderProcessor* embedProcessor = static_cast<InfEmbedderProcessor*>(tmpProc);
        mtClient = static_cast<InfMaipTunedClient*>(embedProcessor->get_assigned_client());
    }

    U32 outMsg = 0;
    if(mtClient->add_message(in_input, in_role, outMsg) == InfClientTextToText::flags::INF_CLIENT_ERR_MISSING_INPUT)
    {
        return maip_err_code::INF_INVALID_PARAMS;
    }
    
    out_msgid = outMsg;

	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::exec_set_input(const mbase::string& in_session_token, const U64& in_ctxId, mbase::context_role in_role, CBYTEBUFFER in_input, const size_type& in_length, U32& out_msgid)
{
	MBASE_SESSION_CONTROL;
	InfClientSession::chat_session_map::iterator It = clientSession.mChatSessions.find(in_ctxId);
	if(It == clientSession.mChatSessions.end())
	{
		// Couldn't find the chat session
		return maip_err_code::INF_CONTEXT_ID_MISMATCH;
	}
	InfProcessorBase* tmpProc = It->second;
    InfMaipTunedClient* mtClient = NULL;

    if(tmpProc->get_processor_type() == InfProcessorBase::processor_type::TEXT_TO_TEXT)
    {
        InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(tmpProc);
        mtClient = static_cast<InfMaipTunedClient*>(t2tProcessor->get_assigned_client()); // 100% success;
    }

    else if(tmpProc->get_processor_type() == InfProcessorBase::processor_type::EMBEDDER)
    {
        InfEmbedderProcessor* embedProcessor = static_cast<InfEmbedderProcessor*>(tmpProc);
        mtClient = static_cast<InfMaipTunedClient*>(embedProcessor->get_assigned_client()); // 100% success;
    }

    U32 outMsg = 0;
    if(mtClient->add_message(in_input, in_length, in_role, outMsg) == InfClientTextToText::flags::INF_CLIENT_ERR_MISSING_INPUT)
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

	InfProcessorBase* tmpProc = It->second;
    InfMaipTunedClient* mtClient = NULL;
    inf_text_token_vector tokenVector;
    mbase::vector<context_line> outMessages;

	if(tmpProc->get_processor_type() == InfProcessorBase::processor_type::TEXT_TO_TEXT)
    {
        InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(tmpProc);
        mtClient = static_cast<InfMaipTunedClient*>(t2tProcessor->get_assigned_client()); // 100% success;
        if(mtClient->get_message_array(in_msgid.data(), in_msgid.size(), outMessages) == InfClientTextToText::flags::INF_CLIENT_ERR_MSG_ID_MISMATCH)
        {
            return maip_err_code::EXEC_MESSAGE_ID_MISMATCH;
        }

        if(t2tProcessor->tokenize_input(outMessages.data(), outMessages.size(), tokenVector) != InfProcessorBase::flags::INF_PROC_SUCCESS)
        {
            return maip_err_code::EXEC_TOKENIZATION_FAILED;
        }

        InfProcessorBase::flags execErr = t2tProcessor->execute_input(tokenVector, true);
        if(execErr == InfProcessorBase::flags::INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT)
        {
            return maip_err_code::EXEC_TOKEN_LIMIT_EXCEEDED;
        }

        if(execErr == InfProcessorBase::flags::INF_PROC_INFO_HALTED)
        {
            return maip_err_code::EXEC_PROCESS_HALTED;
        }
    }

    else if(tmpProc->get_processor_type() == InfProcessorBase::processor_type::EMBEDDER)
    {
        InfEmbedderProcessor* embedProcessor = static_cast<InfEmbedderProcessor*>(tmpProc);
        mtClient = static_cast<InfMaipTunedClient*>(embedProcessor->get_assigned_client());
        for(mbase::vector<U32>::iterator It = in_msgid.begin(); It != in_msgid.end(); ++It)
        {
            if(!mtClient->has_message(*It))
            {
                return maip_err_code::EXEC_MESSAGE_ID_MISMATCH;
            }
        }
        mtClient->set_embedder_message_queue(in_msgid);
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

	clientSession.mPeer = in_peer;
	InfProcessorBase* tmpProc = It->second;
    InfMaipTunedClient* mtClient = NULL;
	
    if(tmpProc->get_processor_type() == InfProcessorBase::processor_type::TEXT_TO_TEXT)
    {
        InfTextToTextProcessor* t2tProc = static_cast<InfTextToTextProcessor*>(tmpProc);
        //InfProcessorBase::flags outRes = t2tProc->next();  
        // if(outRes != InfTextToTextProcessor::flags::INF_PROC_SUCCESS)
        // {
        //     if(outRes == InfTextToTextProcessor::flags::INF_PROC_INFO_HALTED)
        //     {
        //         return maip_err_code::INF_CONTEXT_HALTED;
        //     }
        //     else if(outRes == InfTextToTextProcessor::flags::INF_PROC_ERR_INPUT_IS_EMPTY)
        //     {
        //         return maip_err_code::EXEC_MISSING_MESSAGE;
        //     }
        // }
    }

    else if(tmpProc->get_processor_type() == InfProcessorBase::processor_type::EMBEDDER)
    {
        InfEmbedderProcessor* embedProcessor = static_cast<InfEmbedderProcessor*>(tmpProc);
        mtClient = static_cast<InfMaipTunedClient*>(embedProcessor->get_assigned_client());

        if(!embedProcessor->is_running())
        {
            return maip_err_code::INF_CONTEXT_HALTED;
        }

        if(!mtClient->proc_next_embedding())
        {
            return maip_err_code::EXEC_MISSING_MESSAGE;
        }
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
	mDescriptionDirectory = mInferenceConfigurator.get_data_path() + L"states/descriptions/";

	mbase::create_directory(mInferenceConfigurator.get_data_path() + L"states/");
	mbase::create_directory(mClientStateDirectory);
	mbase::create_directory(mDescriptionDirectory);

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
			maipUser.lock_user();
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
	_reload_model_descriptions();
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

InfProgram::flags InfProgram::create_user(
		const mbase::string& in_username,
		const mbase::string& in_access_token,
		const mbase::string& in_system_prompt,
		const U32& in_authority_flags,
		const U32& in_model_access_limit,
		const U32& in_maximum_context_length,
		const U32& in_batch_length,
		const U32& in_max_proc_threads,
		const U32& in_proc_threads,
		const bool& in_superuser,
		const bool& in_is_static,
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

	mbase::string accessToken = in_access_token;

	if(accessToken.size() > 768 || !accessToken.size())
	{
		accessToken = mbase::string::generate_uuid();
	}

	U32 batchSize = in_batch_length;
	U32 procMaxThreads = in_max_proc_threads;
	U32 procThreads = in_proc_threads;

	if(batchSize > in_maximum_context_length)
	{
		batchSize = in_maximum_context_length;
	}

	if(!procMaxThreads)
	{
		procMaxThreads = 1;
	}

	if(procThreads > procMaxThreads)
	{
		procThreads = procMaxThreads;
	}

	InfMaipUser newUser;
	newUser.set_access_key(accessToken);
	newUser.set_username(in_username);
	newUser.set_system_prompt(in_system_prompt);
	newUser.add_authority_flags(in_authority_flags);
	newUser.set_distinct_model_access_limit(in_model_access_limit);
	newUser.set_maximum_context_length(in_maximum_context_length);
	newUser.set_batch_size(batchSize);
	newUser.set_processor_max_thread_count(in_max_proc_threads);
	newUser.set_processor_thread_count(procThreads);

	if(in_superuser)
	{
		newUser.make_superuser();
	}

	if(in_is_static)
	{
		newUser.lock_user();
	}

	mUserMap[in_username] = newUser;
	newUser.update_state_file(mClientStateDirectory);

	for(auto& n : mUserMap)
	{
		std::cout << "Username: " << n.first << std::endl;

		InfMaipUser& maipUser = n.second;
		std::cout << "- Context length: " << maipUser.get_maximum_context_length() << std::endl;
		std::cout << "- Model access limit: " << maipUser.get_model_access_limit() << std::endl;
		std::cout << "- Username: " << maipUser.get_username() << std::endl;
		std::cout << "- Access key: " << maipUser.get_access_key() << std::endl;
		std::cout << "- Is superuser: " << maipUser.is_superuser() << std::endl;
		std::cout << "- Is static: " << maipUser.is_static() << std::endl;
	}

	out_access_token = accessToken;

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

InfProgram::maip_err_code InfProgram::common_modification_control(InfClientSession& in_session, const mbase::string& in_username, const U32& in_flags)
{
	// 1- Check if the flags are set. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Check if the specified user is static. If so and if the current session is not superuser, INF_AUTHORIZATION_FAILED
	// 3- Check if the specified user is superuser. If so and if the current session is not a superuser INF_AUTHORIZATION_FAILED

	if(!in_session.mMaipUser.is_flags_set(in_flags))
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	inference_user_map::iterator It = mUserMap.find(in_username);

	if(It == mUserMap.end())
	{
		return maip_err_code::INF_USER_NOT_FOUND;
	}

	InfMaipUser& maipUser = It->second;
	if(maipUser.is_static() && !in_session.mMaipUser.is_superuser())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	if(maipUser.is_superuser() && !in_session.mMaipUser.is_superuser())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	return maip_err_code::INF_SUCCESS;
}

GENERIC InfProgram::_reload_model_descriptions()
{
	mbase::vector<FS_FILE_INFORMATION> fileInfo;
	mbase::get_directory(mClientStateDirectory, fileInfo);

	for(mbase::vector<FS_FILE_INFORMATION>::iterator It = fileInfo.begin(); It != fileInfo.end(); ++It)
	{
		// Querying model descriptions

		mbase::string stateObjectName = mbase::to_utf8(It->fileName);
		InfMaipModelDescription modelDescriptionObject;
		
		modelDescriptionObject.load_from_state_file(stateObjectName, mClientStateDirectory);
		if(modelDescriptionObject.get_original_name().size())
		{
			mModelDescriptionMap[modelDescriptionObject.get_original_name()] = modelDescriptionObject;
		}
	}
}

InfProgram::maip_err_code InfProgram::common_description_modification_control(InfClientSession& in_session, const mbase::string& in_model_target)
{
	maip_err_code result = common_modification_control(in_session, in_session.mMaipUser.get_username(), MAIP_MODEL_LOAD_UNLOAD);
	if(result != maip_err_code::INF_SUCCESS)
	{
		return result;
	}

	model_description_map::iterator It = mModelDescriptionMap.find(in_model_target);
	if(It == mModelDescriptionMap.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}

	return maip_err_code::INF_SUCCESS;
}

GENERIC InfProgram::update_maip_user_sessions(InfMaipUser& in_maip_user)
{
	inference_user_map::iterator It = mUserMap.find(in_maip_user.get_username());
	in_maip_user.update_state_file(mClientStateDirectory);

	if(It != mUserMap.end())
	{
		for(accepted_client_map::iterator It = mSessionMap.begin(); It != mSessionMap.end(); ++It)
		{
			if(It->second.mMaipUser.get_username() == in_maip_user.get_username())
			{
				It->second.mMaipUser = in_maip_user;
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
		t2tModel->update();
		if(!t2tModel->is_initialized())
		{
			// If the model is not initialized,
			// and it is not even initializing now
			// remove it
			if(t2tModel->signal_destroying())
			{
				
			}
			else if(!t2tModel->signal_initializing())
			{
				delete t2tModel; // Since the model object's memory managed by the program
				It = mRegisteredModels.erase(It);
				continue;
			}
		}
		++It;
	}
}

MBASE_END
