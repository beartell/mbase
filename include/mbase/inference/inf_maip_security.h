#ifndef MBASE_INF_MAIP_SECURITY_H
#define MBASE_INF_MAIP_SECURITY_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>

MBASE_BEGIN

static U32 gMaipSecurityDefaultModelAccessLimit = 3;
static U32 gMaipSecurityDefaultMaximumContextLength = 4096;
static U32 gMaipSecurityDefaultAuthorityFlags = 0;

#define MAIP_MODEL_LOAD_UNLOAD (1 << 0) // allow loading/unloading models
#define MAIP_ADAPTER_LOAD_UNLOAD (1 << 1) // allow loading/unloading adapters
#define MAIP_CONTEXT_LENGTH_MODIFICATION (1 << 2) // context length modification
#define MAIP_USER_ACCESS_MODIFICATION (1 << 3) // model access modification
#define MAIP_USER_CREATE_DELETE (1 << 4) // user deletion creation 
#define MAIP_USER_MODIFICATION (1 << 5) // username and access token modification
#define MAIP_USER_STATIC (1 << 6) // makes user group parameters unchangable besides super user access
#define MAIP_USER_SYS_PROMPT_MANIP (1 << 7) // allows user to overwrite embedded system prompt locally.

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
	const mbase::string& get_access_key();
	const mbase::string& get_username();
	U32 get_authority_flags();
	const model_name_vector& get_accessible_models();

	GENERIC set_distinct_model_access_limit(const U32& in_access_limit);
	GENERIC set_maximum_context_length(const U32& in_context_length);
	flags add_accessible_model(const mbase::string& in_modelname);
	flags remove_accessible_model(const mbase::string& in_modelname);
	GENERIC set_username(const mbase::string& in_username);
	GENERIC set_access_key(const mbase::string& in_key);
	GENERIC add_authority_flags(U32 in_flags);
	GENERIC remove_authority_flags(U32 in_flags);
	GENERIC make_superuser();
	GENERIC lock_authorization();
	GENERIC unlock_authorization();

	GENERIC update_state_file(const mbase::string& in_object_name, const mbase::wstring& in_state_path, bool in_overwrite = false);
private:
	U32 mAuthorityFlags = 0;
	U32 mDistinctModelAccessLimit = 0;
	U32 mMaximumContextLength = 0;
	model_name_vector mAccessibleModels;
	mbase::string mUsername = "";
	mbase::string mAccessKey = "";
	bool mIsSuperUser = false;
	bool mIsAuthorizationLocked = false; // If this is true, new clients won't be able to associate themselves with this user.
};

MBASE_END

#endif // !MBASE_INF_MAIP_SECURITY_H
