#include <mbase/inference/inf_maip_user.h>
#include <mbase/pc/pc_state.h>
#include <algorithm>

MBASE_BEGIN

bool InfMaipUser::is_superuser() const noexcept
{
	return mIsSuperUser;
}

bool InfMaipUser::is_static() const noexcept
{
	return mIsStatic;
}

bool InfMaipUser::is_flags_set(U32 in_flags) const noexcept
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

const U32& InfMaipUser::get_model_access_limit() const noexcept
{
	return mDistinctModelAccessLimit;
}

const U32& InfMaipUser::get_maximum_context_length() const noexcept
{
	return mMaximumContextLength;
}

const U32& InfMaipUser::get_batch_size() const noexcept
{
	return mBatchSize;
}

const U32& InfMaipUser::get_processor_max_thread_count() const noexcept
{
	return mMaxProcessorThreadCount;
}

const U32& InfMaipUser::get_processor_thread_count() const noexcept
{
	return mProcessorThreadCount;
}

const mbase::string& InfMaipUser::get_access_key() const noexcept
{
	return mAccessKey;
}

const mbase::string& InfMaipUser::get_username() const noexcept
{
	return mUsername;
}

const mbase::string& InfMaipUser::get_system_prompt() const noexcept
{
	return mSystemPrompt;
}

const mbase::string& InfMaipUser::get_assistant_prefix() const noexcept
{
	return mAssistantPrefix;
}

const inf_sampling_set& InfMaipUser::get_sampling_set() const noexcept
{
	return mSamplingSet;
}

const U32& InfMaipUser::get_authority_flags() const noexcept
{
	return mAuthorityFlags;
}

const typename InfMaipUser::model_name_set& InfMaipUser::get_accessible_models() const noexcept
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

GENERIC InfMaipUser::set_batch_size(const U32& in_batch_size)
{
	mBatchSize = in_batch_size;
}

GENERIC InfMaipUser::set_processor_max_thread_count(const U32& in_thread_count)
{
	mMaxProcessorThreadCount = in_thread_count;
}

GENERIC InfMaipUser::set_processor_thread_count(const U32& in_thread_count)
{
	mProcessorThreadCount = in_thread_count;
}

GENERIC InfMaipUser::set_sampling_set(const inf_sampling_set& in_sampling_set)
{
	mSamplingSet = in_sampling_set;
}

GENERIC InfMaipUser::set_system_prompt(const mbase::string& in_system_prompt)
{
	mSystemPrompt = in_system_prompt;
}

GENERIC InfMaipUser::set_assistant_prefix(const mbase::string& in_prefix)
{
	mAssistantPrefix = in_prefix;
}

InfMaipUser::flags InfMaipUser::add_accessible_model(const mbase::string& in_modelname)
{
	mAccessibleModels.insert(in_modelname);
	return flags::INF_MAIP_USER_SUCCESS;
}

InfMaipUser::flags InfMaipUser::remove_accessible_model(const mbase::string& in_modelname)
{
	model_name_set::iterator accessibleModels = std::find(mAccessibleModels.begin(), mAccessibleModels.end(), in_modelname);
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

GENERIC InfMaipUser::unmake_superuser()
{
	mIsSuperUser = false;
}

GENERIC InfMaipUser::lock_user()
{
	mIsStatic = true;
}

GENERIC InfMaipUser::unlock_user()
{
	mIsStatic = false;
}

GENERIC InfMaipUser::load_from_state_file(const mbase::string& in_object_name, const mbase::wstring& in_state_path)
{
	PcState userState;

	userState.initialize(in_object_name, in_state_path);

	mbase::vector<mbase::string> tmpVectorizedSet;

	userState.get_state("authority_flags", mAuthorityFlags);
	userState.get_state("model_access_limit", mDistinctModelAccessLimit);
	userState.get_state("max_context_length", mMaximumContextLength);
	userState.get_state("batch_size", mBatchSize);
	userState.get_state("proc_max_thread_count", mMaxProcessorThreadCount);
	userState.get_state("proc_thread_count", mProcessorThreadCount);
	userState.get_state("accessible_models", tmpVectorizedSet);
	userState.get_state("username", mUsername);
	userState.get_state("access_key", mAccessKey);
	userState.get_state("system_prompt", mSystemPrompt);
	userState.get_state("is_super", mIsSuperUser);
	userState.get_state("is_static", mIsStatic);

	mAccessibleModels.insert(tmpVectorizedSet.begin(), tmpVectorizedSet.end());

	// TODO, DO FOR THE SAMPLING SET TOO
}

GENERIC InfMaipUser::update_state_file(const mbase::wstring& in_state_path)
{
	mbase::PcState userState;
	
	if(!get_username().size())
	{
		return;
	}

	userState.initialize_overwrite(get_username(), in_state_path);
	mbase::vector<mbase::string> tmpVectorizedSet(mAccessibleModels.begin(), mAccessibleModels.end()); // For file serialization

	userState.set_state("authority_flags", get_authority_flags());
	userState.set_state("model_access_limit", get_model_access_limit());
	userState.set_state("max_context_length", get_maximum_context_length());
	userState.set_state("batch_size", get_batch_size());
	userState.set_state("proc_max_thread_count", get_processor_max_thread_count());
	userState.set_state("proc_thread_count", get_processor_thread_count());
	userState.set_state("accessible_models", tmpVectorizedSet);
	userState.set_state("username", get_username());
	userState.set_state("access_key", get_access_key());
	userState.set_state("system_prompt", get_system_prompt());
	/* userState.set_state<inf_sampling_set>("sampling_set", get_sampling_set()); // Fix this thing */
	userState.set_state("is_super", is_superuser());
	userState.set_state("is_static", is_static());
	 
	userState.update();
}

MBASE_END