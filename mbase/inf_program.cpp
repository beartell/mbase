#include <mbase/inference/inf_program.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_maip_peer_t2t.h>
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
InfMaipPeerBase* clientSession = mSessionMap[in_session_token];

#define MBASE_DESC_MODIF_CONTROL \
maip_err_code result = common_description_modification_control(clientSession, in_model_target);\
if(result != maip_err_code::INF_SUCCESS)\
{\
	return result;\
}

InfMaipModelBase::InfMaipModelBase(InfProgram* in_program, const mbase::string& in_model_name):
	mProgramInstance(in_program),
	mDefinedModelName(in_model_name)
{
}

InfMaipModelBase::~InfMaipModelBase(){}

class MBASE_API InfMaipModelTextToText : public InfMaipModelBase, public InfModelTextToText {
public:
	GENERIC on_initialize_fail(init_fail_code out_fail_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;
};

GENERIC InfMaipModelTextToText::on_initialize_fail(init_fail_code out_fail_code)
{
	mProgramInstance->remove_loading_model(mDefinedModelName);
}

GENERIC InfMaipModelTextToText::on_initialize()
{
	mProgramInstance->set_registered_model(this, mDefinedModelName);
}

GENERIC InfMaipModelTextToText::on_destroy()
{
	
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

InfProgram::maip_err_code InfProgram::inf_access_request(const mbase::string& in_username, const mbase::string& in_access_token, std::shared_ptr<PcNetPeerClient> in_client, const mbase::string& in_category, mbase::string& out_session_token)
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
		if(in_category == "T2T")
		{
			mSessionMap[out_session_token] = new InfMaipPeerTextToText(in_client, maipUser.get_username());
		}
		else
		{
			// undefined category
			return maip_err_code::INF_UNDEFINED_CATEGORY;
		}

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
		accepted_client_map::iterator It = mSessionMap.find(in_session_token);
		delete It->second;
		mSessionMap.erase(It);
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
	const InfMaipUser::model_name_set& accModels = mUserMap[clientSession->get_maip_username()].get_accessible_models();
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

	out_contexts = clientSession->get_processor_ids();
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_create_context(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_model, const U32& in_ctsize)
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
	InfMaipUser& sessionMaip = mUserMap[clientSession->get_maip_username()];
	if(!sessionMaip.is_model_accessible(in_model))
	{
		return maip_err_code::INF_MODEL_IS_NOT_ACCESSIBLE;
	}

	if (in_ctsize < 32)
	{
		return maip_err_code::INF_INVALID_TOKEN_LIMIT;
	}

	model_description_map::iterator It = mModelDescriptionMap.find(in_model);
	if(It == mModelDescriptionMap.end())
	{
		return maip_err_code::INF_MODEL_NAME_MISMATCH;
	}
	
	if(It->second.get_category_string() != clientSession->get_peer_category())
	{
		return maip_err_code::INF_CATEGORY_MISMATCH;
	}

	registered_model_map::iterator It2 = mRegisteredModels.find(in_model);

	if(It2 == mRegisteredModels.end())
	{
		return maip_err_code::INF_MODEL_NOT_LOADED;
	}

	if(in_ctsize > sessionMaip.get_maximum_context_length())
	{
		return maip_err_code::INF_USER_CONTEXT_LENGTH_EXCEEDED;
	}

	// InfModelTextToText* t2tModel = It->second;
	// if(!t2tModel->is_available(in_ctsize))
	// {
	// 	return maip_err_code::INF_MODEL_CONTEXT_FULL;
	// }

	// // means the model is available
	// // we can create the context
    // clientSession.mPeer = in_peer;

	// if(t2tModel->is_embedding_model())
	// {
	// 	InfMaipTunedEmbedderProcessor* maipEmbedderContext = new InfMaipTunedEmbedderProcessor(clientSession, this);
	// 	t2tModel->register_context_process(
	// 		maipEmbedderContext,
	// 		in_ctsize,
	// 		clientSession.mMaipUser.get_batch_size(),
	// 		clientSession.mMaipUser.get_processor_thread_count()
	// 	); // 100% SUCCESS
	// }
	// else
	// {
	// 	InfMaipTunedT2TProcessor* maipT2tContext = new InfMaipTunedT2TProcessor(clientSession, this);
	// 	t2tModel->register_context_process(
	// 		maipT2tContext,
	// 		in_ctsize,
	// 		clientSession.mMaipUser.get_batch_size(),
	// 		clientSession.mMaipUser.get_processor_thread_count(),
	// 		true,
	// 		clientSession.mMaipUser.get_sampling_set()
	// 	); // 100% SUCCESS
	// }
	return maip_err_code::INF_SUCCESS;
}

InfProgram::maip_err_code InfProgram::inf_clear_context_history(const mbase::string& in_session_token, const U64& in_ctxId)
{
	MBASE_SESSION_CONTROL;
	
	if(clientSession->get_peer_category() == "T2T")
	{
		InfMaipPeerTextToText* peerT2t = static_cast<InfMaipPeerTextToText*>(clientSession);
		peerT2t->clear_chat_history();
	}
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
	InfProcessorBase* procBase = clientSession->get_processor_by_id(in_ctxId);
	if(procBase)
	{
		procBase->destroy();
	}
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
	InfMaipUser& sessionMaip = mUserMap[clientSession->get_maip_username()];
	if(!sessionMaip.is_flags_set(MAIP_MODEL_LOAD_UNLOAD))
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

	for(auto& n : mLoadingModels)
	{
		if(n == in_modelname)
		{
			return maip_err_code::INF_LOADING_MODEL;
		}
	}

	for(auto& n : mModelDescriptionMap)
	{
		if(n.first == in_modelname)
		{
			if(!mbase::is_file_valid(mbase::from_utf8(n.first)))
			{
				return maip_err_code::INF_UNABLE_TO_OPEN_MODEL_FILE;
			}

			mbase::wstring modelFileTotal = mModelDirectory + mbase::from_utf8(n.second.get_model_file());

			if(n.second.get_category_value() == InfMaipModelDescription::CATEGORY::TEXT_TO_TEXT)
			{

			}

			InfMaipModel* newModel = new InfMaipModel(n.second.get_custom_name(), *this);
			newModel->initialize_model(modelFileTotal, in_total_context_size, 999);
			mRegisteredModels[n.second.get_custom_name()] = newModel;
			return maip_err_code::INF_LOADING_MODEL;
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
		{},
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
		in_program_information.mConfigPath + L"mbase_inference_config"
	);

	mbase::string tmpApplicationName;
	mbase::string tmpHostName;
	mbase::string productId;
	mbase::string defaultUsername;
	mbase::string defaultAccessToken;
	mbase::string versionString;
	I32 tmpHostPort = 0;

	//mInferenceConfigurator.get_config_param("models_directory", mModelDirectory);

	// UNIX
	// Config path is: /etc/mbase_inference_config
	// Data path is: /var/lib/mbase_inference/

	// Windows 
	// Config path is: C:/ProgramData/mbase_inference/mbase_inference_config
	// Data path is: C:/User/AppData/Roaming/

	// TODO: HANDLE PATH NOT FOUND

	mClientStateDirectory = in_program_information.mDataPath + L"states/users/";
	mDescriptionDirectory = in_program_information.mDataPath + L"states/descriptions/";

	mbase::create_directory(in_program_information.mDataPath + L"states/");
	mbase::create_directory(mClientStateDirectory);
	mbase::create_directory(mDescriptionDirectory);

	mbase::string mainStateName = in_program_information.mProgramInformation.mProductName;
	mMainProgramState.initialize(mainStateName, in_program_information.mDataPath + L"states/");

	this->initialize_system(
		in_program_information.mProgramInformation,
		&mInferenceConfigurator,
		&mInferenceDiagnostics,
		NULL,
		&mInferenceNetManager,
		&mMainProgramState
	);

	_load_user_states();
	_reload_model_descriptions();
}

bool InfProgram::is_maip_user_valid(InfMaipPeerBase* in_client)
{
	
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
		const inf_sampling_set& in_sampling_set,
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

	newUser.set_sampling_set(in_sampling_set);
	mUserMap[in_username] = newUser;
	newUser.update_state_file(mClientStateDirectory);

	out_access_token = accessToken;

	return flags::INF_PROGRAM_SUCCESS;
}

GENERIC InfProgram::remove_loading_model(const mbase::string& in_model_name)
{
	// 100% success, no need for further control
	actively_loading_models::iterator It = mbase::find(mLoadingModels.begin(), mLoadingModels.end(), in_model_name);
	mLoadingModels.erase(It);
}

GENERIC InfProgram::set_registered_model(InfMaipModelBase* in_model, const mbase::string& in_model_name)
{
	// No need to check anything, this place will succeed
	remove_loading_model(in_model_name);
	mRegisteredModels[in_model_name] = in_model;
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

InfProgram::maip_err_code InfProgram::common_modification_control(InfMaipPeerBase* in_session, const mbase::string& in_username, const U32& in_flags)
{
	// 1- Check if the flags are set. If not, INF_AUTHORIZATION_FAILED
	// 2- Check if the specified username exists. If not, INF_USER_NOT_FOUND
	// 3- Check if the specified user is static. If so and if the current session is not superuser, INF_AUTHORIZATION_FAILED
	// 3- Check if the specified user is superuser. If so and if the current session is not a superuser INF_AUTHORIZATION_FAILED

	InfMaipUser& sessionMaip = mUserMap[in_session->get_maip_username()];

	if(!sessionMaip.is_flags_set(in_flags))
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	inference_user_map::iterator It = mUserMap.find(in_username);

	if(It == mUserMap.end())
	{
		return maip_err_code::INF_USER_NOT_FOUND;
	}

	InfMaipUser& maipUser = It->second;
	if(maipUser.is_static() && !sessionMaip.is_superuser())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	if(maipUser.is_superuser() && !sessionMaip.is_superuser())
	{
		return maip_err_code::INF_AUTHORIZATION_FAILED;
	}

	return maip_err_code::INF_SUCCESS;
}

GENERIC InfProgram::_reload_model_descriptions()
{
	mbase::vector<FS_FILE_INFORMATION> fileInfo;
	mbase::get_directory(mDescriptionDirectory, fileInfo);

	for(mbase::vector<FS_FILE_INFORMATION>::iterator It = fileInfo.begin(); It != fileInfo.end(); ++It)
	{
		// Querying model descriptions

		mbase::string stateObjectName = mbase::to_utf8(It->fileName);
		std::cout << "Model description name: " << stateObjectName << std::endl;
		InfMaipModelDescription modelDescriptionObject;
		
		modelDescriptionObject.load_from_state_file(stateObjectName, mDescriptionDirectory);
		
			std::cout << "Original name: " << modelDescriptionObject.get_original_name() << std::endl;
			std::cout << "Custom name: " << modelDescriptionObject.get_custom_name() << std::endl;
			std::cout << "Description: " << modelDescriptionObject.get_description() << std::endl;
			std::cout << "System prompt: " << modelDescriptionObject.get_system_prompt() << std::endl;
			std::cout << "Model file: " << modelDescriptionObject.get_model_file() << std::endl;
			std::cout << "Maximum context length: " << modelDescriptionObject.get_maximum_context_length() << std::endl;
			
			std::cout << "Tags: ";
			for(auto& n : modelDescriptionObject.get_tags())
			{
				std::cout << n << ";";
			}
			std::cout << std::endl;
			std::cout << "Force prompt: " << modelDescriptionObject.get_forced_system_prompt() << std::endl;
			std::cout << "Is embedding: " << modelDescriptionObject.get_embedding() << std::endl;

			mModelDescriptionMap[modelDescriptionObject.get_custom_name()] = modelDescriptionObject;
		
	}
}

GENERIC InfProgram::_load_user_states()
{
	mbase::vector<FS_FILE_INFORMATION> fileInfo;
	mbase::get_directory(mClientStateDirectory, fileInfo);

	for(mbase::vector<FS_FILE_INFORMATION>::iterator It = fileInfo.begin(); It != fileInfo.end(); ++It)
	{
		// Querying model descriptions

		InfMaipUser maipUser;
		maipUser.load_from_state_file(mbase::to_utf8(It->fileName), mClientStateDirectory);

		if(maipUser.get_username().size() && maipUser.get_access_key().size())
		{	
			// If there is a username and access key
			// load it
			mUserMap[maipUser.get_username()] = maipUser;
		}
	}
}

InfProgram::maip_err_code InfProgram::common_description_modification_control(InfMaipModelDescription& in_description, const mbase::string& in_model_target)
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

GENERIC InfProgram::push_dead_model(InfModelBase& in_model)
{
	mDeadModelVector.push_back(&in_model);
}

GENERIC InfProgram::push_dead_processor(InfProcessorBase& in_processor)
{
	mDeadProcessorVector.push_back(&in_processor);
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

	for(dead_processor_vector::iterator It = mDeadProcessorVector.begin(); It != mDeadProcessorVector.end(); It++)
	{
		delete *It;
	}

	for(dead_model_vector::iterator It = mDeadModelVector.begin(); It != mDeadModelVector.end(); It++)
	{
		delete *It;
	}

	mDeadProcessorVector.clear();
	mDeadModelVector.clear();

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
