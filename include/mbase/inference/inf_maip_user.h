#ifndef MBASE_INF_MAIP_USER_H
#define MBASE_INF_MAIP_USER_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>
#include <mbase/inference/inf_sampling_set.h>
#include <set>

MBASE_BEGIN

#define MAIP_MODEL_LOAD_UNLOAD (1 << 0) // Allows user group to load and unload models on server
#define MAIP_ADAPTER_LOAD_UNLOAD (1 << 1) // Allows user group to load and unload adapters on server
#define MAIP_USER_CREATE_DELETE (1 << 2) // Allows user group to create and delete new users
#define MAIP_USER_STATIC (1 << 3) // If this is set, the properties of a user group is not modifiable unless the request has been made by a superuser.
#define MAIP_USER_MODIFICATION (1 << 4) // Allows user group to change the username and acces token of other user groups
#define MAIP_USER_CONTEXT_LENGTH_MODIFICATION (1 << 5) // Allows user group to modify context length of other user groups
#define MAIP_USER_ACCESS_MODIFICATION (1 << 6) // Allows user group to modify access list of other user groups
#define MAIP_USER_SYS_PROMPT_MODIFICATION (1 << 7) // Allows user group to modify group system prompt of another user group.
#define MAIP_USER_SAMPLER_MODIFICATION (1 << 8) // Allows user to modify sampler order of other user groups.
#define MAIP_USER_MAX_PROCESSOR_THREAD_COUNT_MODIFICATION (1 << 9) // Allows user to modify maximum amount of thread usage of another user group
#define MAIP_USER_PROCESSOR_THREAD_COUNT_MODIFICATION (1 << 10) // Allows user to modify amount of thread used by user group
#define MAIP_USER_BATCH_LENGTH_MODIFICATION (1 << 11) // Allows user to modify the batch length of other user groups.

class MBASE_API InfMaipUser { // Useless unless it is associated with MAIP Program
public:
	using model_name_set = std::set<mbase::string>;
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

	bool is_superuser() const noexcept;
	bool is_static() const noexcept;
	bool is_flags_set(U32 in_flags) const noexcept;
	bool is_model_accessible(const mbase::string& in_modelname);
	const U32& get_model_access_limit() const noexcept;
	const U32& get_maximum_context_length() const noexcept;
	const U32& get_batch_size() const noexcept;
	const U32& get_processor_max_thread_count() const noexcept;
	const U32& get_processor_thread_count() const noexcept;
	const mbase::string& get_access_key() const noexcept;
	const mbase::string& get_username() const noexcept;
	const mbase::string& get_system_prompt() const noexcept;
	const mbase::string& get_assistant_prefix() const noexcept;
	const inf_sampling_set& get_sampling_set() const noexcept;
	const U32& get_authority_flags() const noexcept;
	const model_name_set& get_accessible_models() const noexcept;

	GENERIC set_distinct_model_access_limit(const U32& in_access_limit);
	GENERIC set_maximum_context_length(const U32& in_context_length);
	GENERIC set_batch_size(const U32& in_batch_size);
	GENERIC set_processor_max_thread_count(const U32& in_thread_count);
	GENERIC set_processor_thread_count(const U32& in_thread_count);
	GENERIC set_sampling_set(const inf_sampling_set& in_sampling_set);
	GENERIC set_system_prompt(const mbase::string& in_system_prompt);
	GENERIC set_assistant_prefix(const mbase::string& in_prefix);
	flags add_accessible_model(const mbase::string& in_modelname);
	flags remove_accessible_model(const mbase::string& in_modelname);
	GENERIC set_username(const mbase::string& in_username);
	GENERIC set_access_key(const mbase::string& in_key);
	GENERIC add_authority_flags(U32 in_flags);
	GENERIC remove_authority_flags(U32 in_flags);
	GENERIC make_superuser();
	GENERIC unmake_superuser();
	GENERIC lock_user();
	GENERIC unlock_user();

	GENERIC load_from_state_file(const mbase::string& in_object_name, const mbase::wstring& in_state_path);
	GENERIC update_state_file(const mbase::wstring& in_state_path);

private:
	U32 mAuthorityFlags = 0;
	U32 mDistinctModelAccessLimit = 0;
	U32 mMaximumContextLength = 0;
	U32 mBatchSize = 0;
	U32 mMaxProcessorThreadCount = 0;
	U32 mProcessorThreadCount = 0;
	model_name_set mAccessibleModels;
	mbase::string mUsername = "";
	mbase::string mAccessKey = "";
	mbase::string mSystemPrompt = "";
	mbase::string mAssistantPrefix = "";
	inf_sampling_set mSamplingSet;
	bool mIsSuperUser = false;
	bool mIsStatic = false;
};

MBASE_END

#endif // !MBASE_INF_MAIP_USER_H
