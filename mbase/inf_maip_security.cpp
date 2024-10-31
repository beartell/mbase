#include <mbase/inference/inf_maip_security.h>
#include <algorithm>

MBASE_BEGIN

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
	if(is_superuser())
	{
		// all flags are assumed to be set on super user
		return true;
	}
	return mAuthorityFlags & in_flags;
}

bool InfMaipUser::is_model_accessible(const mbase::string& in_modelname)
{
	if(is_superuser())
	{
		return true;
	}

	if(std::find(mAccessibleModels.begin(), mAccessibleModels.end(), in_modelname) != mAccessibleModels.end())
	{
		return true;
	}
	return false;
}

U32 InfMaipUser::get_model_access_limit()
{
	return mDistinctModelAccessLimit;
}

U32 InfMaipUser::get_maximum_context_length()
{
	return mMaximumContextLength;
}

const mbase::string& InfMaipUser::get_access_key()
{
	return mAccessKey;
}

const mbase::string& InfMaipUser::get_username() 
{
	return mUsername;
}

U32 InfMaipUser::get_authority_flags()
{
	return mAuthorityFlags;
}

const typename InfMaipUser::model_name_vector& InfMaipUser::get_accessible_models()
{
	return mAccessibleModels;
}

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
	model_name_vector::iterator accessibleModels = std::find(mAccessibleModels.begin(), mAccessibleModels.end(), in_modelname);

	if (accessibleModels == mAccessibleModels.end())
	{
		mAccessibleModels.push_back(in_modelname);
	}
	return flags::INF_MAIP_USER_SUCCESS;
}

InfMaipUser::flags InfMaipUser::remove_accessible_model(const mbase::string& in_modelname)
{
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

GENERIC InfMaipUser::set_access_key(const mbase::string& in_key)
{
	mAccessKey = in_key;
}

GENERIC InfMaipUser::add_authority_flags(U32 in_flags)
{
	mAuthorityFlags |= in_flags;
}

GENERIC InfMaipUser::remove_authority_flags(U32 in_flags)
{
	mAuthorityFlags &= ~in_flags;
}

GENERIC InfMaipUser::make_superuser()
{
	mIsSuperUser = true;
}

GENERIC InfMaipUser::lock_authorization()
{
	mIsAuthorizationLocked = true;
}

GENERIC InfMaipUser::unlock_authorization()
{
	mIsAuthorizationLocked = false;
}

MBASE_END