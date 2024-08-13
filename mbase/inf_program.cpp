#include <mbase/inference/inf_program.h>

MBASE_BEGIN

#define MBASE_SESSION_CONTROL \
if(!this->is_session_match(in_csid, in_clid))\
{\
	return maip_err_code::INF_UNAUTHORIZED_ACCESS;\
}\

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

InfProgram::maip_err_code InfProgram::inf_create_session(const mbase::string& in_clid = mbase::string(), U64& out_csid)
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
}

InfProgram::maip_err_code InfProgram::inf_accept_client(const U64& in_csid, const mbase::vector<mbase::string>& in_models, mbase::string& out_clid)
{
	return 0;
}

InfProgram::maip_err_code InfProgram::inf_destroy_client(MBASE_MAIP_CL_AUTH)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_get_acquired_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_get_created_context_ids(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_contexts)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_create_context(MBASE_MAIP_CL_AUTH, const mbase::string& in_model, const U32& in_ctsize)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_destroy_context(MBASE_MAIP_CL_AUTH, const mbase::string& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_acquire_model(MBASE_MAIP_CL_AUTH, const mbase::string& in_model)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_release_model(MBASE_MAIP_CL_AUTH, const mbase::string& in_model)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_get_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::inf_get_model_params(MBASE_MAIP_CL_AUTH, const mbase::string& in_model)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::exec_set_input(MBASE_MAIP_CL_AUTH, const mbase::string& in_ctxId, InfClient::input_role in_role, const mbase::string& in_input, U32& out_msgid)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::exec_execute_input(MBASE_MAIP_CL_AUTH, const mbase::string& in_ctxId, const U32& in_msgid)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::exec_next(MBASE_MAIP_CL_AUTH, const mbase::string& in_ctxId, mbase::string& out_message)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

InfProgram::maip_err_code InfProgram::exec_terminate_generation(MBASE_MAIP_CL_AUTH, const mbase::string& in_ctxId)
{
	MBASE_SESSION_CONTROL;

	return 0;
}

GENERIC InfProgram::host_model(InfModel& in_model)
{

}

MBASE_END
