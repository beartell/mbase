#include <mbase/inference/inf_maip_security.h>

MBASE_BEGIN

GENERIC InfMaipUser::set_distinct_model_access_limit(const U32& in_access_limit)
{
	mDistinctModelAccessLimit = in_access_limit;
}

GENERIC InfMaipUser::set_maximum_context_length(const U32& in_context_length)
{
	mMaximumContextLength = in_context_length;
}

InfMaipUser::flags InfMaipUser::add_accessible_model(const mbase::string& in_modelname)
{
	if (!is_flags_set(MAIP_USER_ACCESS_MODIFICATION))
	{
		return flags::INF_MAIP_USER_UNAUTHORIZED_OPERATION;
	}
	model_name_vector::iterator accessibleModels = std::find(mAccessibleModels.begin(), mAccessibleModels.end(), in_modelname);
	if (accessibleModels == mAccessibleModels.end())
	{
		mAccessibleModels.push_back(in_modelname);
	}
	return flags::INF_MAIP_USER_SUCCESS;
}

InfMaipUser::flags InfMaipUser::remove_accessible_model(const mbase::string& in_modelname)
{
	if(!is_flags_set(MAIP_USER_ACCESS_MODIFICATION))
	{
		return flags::INF_MAIP_USER_UNAUTHORIZED_OPERATION;
	}

	model_name_vector::iterator accessibleModels = std::find(mAccessibleModels.begin(), mAccessibleModels.end(), in_modelname);
	if(accessibleModels != mAccessibleModels.end())
	{
		mAccessibleModels.erase(accessibleModels);
	}

	return flags::INF_MAIP_USER_SUCCESS;
}

GENERIC InfMaipUser::set_username(const mbase::string& in_username)
{
	mUsername = in_username;
}

GENERIC InfMaipUser::add_authority_flags(U32 in_flags)
{
	mAuthorityFlags | in_flags;
}

GENERIC InfMaipUser::remove_authority_flags(U32 in_flags)
{
	mAuthorityFlags &= ~in_flags;
}

GENERIC InfMaipUser::make_superuser()
{
	
}

bool InfMaipUser::is_superuser()
{
	return mIsSuperUser;
}

bool InfMaipUser::is_authorization_locked()
{
	return mIsAuthorizationLocked;
}

bool InfMaipUser::is_flags_set(U32 in_flags)
{
	return mAuthorityFlags & in_flags;
}

MBASE_END