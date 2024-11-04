#ifndef MBASE_INF_MAIP_SECURITY_H
#define MBASE_INF_MAIP_SECURITY_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>
#include <mbase/inference/inf_sampling_set.h>

MBASE_BEGIN

static U32 gMaipSecurityDefaultModelAccessLimit = 3;
static U32 gMaipSecurityDefaultMaximumContextLength = 4096;
static U32 gMaipSecurityDefaultAuthorityFlags = 0;

#define MAIP_MODEL_LOAD_UNLOAD (1 << 0) // Allows user group to load and unload models on server
#define MAIP_ADAPTER_LOAD_UNLOAD (1 << 1) // Allows user group to load and unload adapters on server
#define MAIP_USER_CREATE_DELETE (1 << 2) // Allows user group to create and delete new users
#define MAIP_USER_STATIC (1 << 3) // If this is set, the properties of a user group is not modifiable unless the request has been made by a superuser.
#define MAIP_USER_MODIFICATION (1 << 4) // Allows user group to change the username and acces token of other user groups
#define MAIP_USER_CONTEXT_LENGTH_MODIFICATION (1 << 5) // Allows user group to modify context length of other user groups
#define MAIP_USER_ACCESS_MODIFICATION (1 << 6) // Allows user group to modify access list of other user groups
#define MAIP_USER_SYS_PROMPT_MODIFICATION (1 << 7) // Allows user group to modify group system prompt of another user group.
#define MAIP_USER_SAMPLER_MODIFICATION (1 << 8) // Allows user to modify sampler order of other user groups.
#define MAIP_USER_PROCESSOR_MODIFICATION (1 << 9) // Allows user to modify processor resource usage of other user groups.
#define MAIP_USER_BATCH_LENGTH_MODIFICATION (1 << 10) // Allows user to modify the batch length of other user groups.

class InfMaipUser { // Useless unless it is associated with MAIP Program
public:
	using model_name_vector = mbase::vector<mbase::string>;
	using size_type = SIZE_T;

	enum class flags : U8 {
		INF_MAIP_USER_SUCCESS,
		INF_MAIP_USER_AUTHORIZATION_LOCKED,
		INF_MAIP_USER_UNAUTHORIZED_OPERATION
	};

	InfMaipUser() = default;
	InfMaipUser(const InfMaipUser& in_rhs) = default;
	InfMaipUser(InfMaipUser&& in_rhs) = default;
	~InfMaipUser() = default;

	InfMaipUser& operator=(const InfMaipUser& in_rhs) = default;
	InfMaipUser& operator=(InfMaipUser&& in_rhs) = default;

	bool is_superuser();
	bool is_authorization_locked();
	bool is_flags_set(U32 in_flags);
	bool is_model_accessible(const mbase::string& in_modelname);
	U32 get_model_access_limit();
	U32 get_maximum_context_length();
	U32 get_batch_size();
	U32 get_processor_thread_count();
	U32 get_batch_thread_count();
	const mbase::string& get_access_key();
	const mbase::string& get_username();
	const mbase::string& get_system_prompt();
	const inf_sampling_set& get_sampling_set();
	U32 get_authority_flags();
	const model_name_vector& get_accessible_models();

	GENERIC set_distinct_model_access_limit(const U32& in_access_limit);
	GENERIC set_maximum_context_length(const U32& in_context_length);
	GENERIC set_batch_size(const U32& in_batch_size);
	GENERIC set_processor_thread_count(const U32& in_thread_count);
	GENERIC set_batch_thread_count(const U32& in_thread_count);
	GENERIC set_sampling_set(const inf_sampling_set& in_sampling_set);
	GENERIC set_system_prompt(const mbase::string& in_system_prompt);
	flags add_accessible_model(const mbase::string& in_modelname);
	flags remove_accessible_model(const mbase::string& in_modelname);
	GENERIC set_username(const mbase::string& in_username);
	GENERIC set_access_key(const mbase::string& in_key);
	GENERIC add_authority_flags(U32 in_flags);
	GENERIC remove_authority_flags(U32 in_flags);
	GENERIC make_superuser();
	GENERIC lock_authorization();
	GENERIC unlock_authorization();

	GENERIC update_state_file(const mbase::wstring& in_state_path, bool in_overwrite = false);
private:
	U32 mAuthorityFlags = 0;
	U32 mDistinctModelAccessLimit = 0;
	U32 mMaximumContextLength = 0;
	U32 mBatchSize = 0;
	U32 mProcessorThreadCount = 0;
	U32 mBatchThreadCount = 0;
	model_name_vector mAccessibleModels;
	mbase::string mUsername = "";
	mbase::string mAccessKey = "";
	mbase::string mSystemPrompt = "";
	inf_sampling_set mSamplingSet;
	bool mIsSuperUser = false;
	bool mIsAuthorizationLocked = false; // If this is true, new clients won't be able to associate themselves with this user.
};

MBASE_END

#endif // !MBASE_INF_MAIP_SECURITY_H
