#ifndef MBASE_INF_MAIP_SECURITY_H
#define MBASE_INF_MAIP_SECURITY_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>

MBASE_BEGIN

#define MAIP_MODEL_LOAD_UNLOAD 0x1
#define MAIP_ADAPTER_LOAD_UNLOAD 0x2
#define MAIP_CONTEXT_LENGTH_MODIFICATION 0x4
#define MAIP_USER_ACCESS_MODIFICATION 0x8
#define MAIP_USER_CREATE_DELETE 0x10
#define MAIP_USER_MODIFICATION 0x20
#define MAIP_USER_STATIC 0x40

//enum class maip_authority_flags : U32 {
//	MODEL_LOAD_UNLOAD, // Allows user group to load and unload models
//	ADAPTER_LOAD_UNLOAD, // Allows user group to load and unload lora adapters
//	CONTEXT_LENGTH_MODIFICATION, // Allows user group to extend or decrease the total context length
//	USER_ACCESS_MODIFICATION, // Allows user to modify  
//	USER_CREATE_DELETE, // Allows user to create or delete users
//	USER_MODIFICATION, // Allows user to modify parameters of other users
//	USER_STATIC // Makes the user groups parameters to be unchangable besides super user access
//};

class InfMaipUser { // Useless unless it is associated with MAIP Program
public:
	using model_name_vector = mbase::vector<mbase::string>;

	enum class flags : U8 {
		INF_MAIP_USER_SUCCESS,
		INF_MAIP_USER_AUTHORIZATION_LOCKED,
		INF_MAIP_USER_UNAUTHORIZED_OPERATION
	};

	InfMaipUser() = default;
	InfMaipUser(const InfMaipUser& in_rhs) = default;
	InfMaipUser(InfMaipUser&& in_rhs) = default;
	~InfMaipUser() = default;

	GENERIC set_distinct_model_access_limit(const U32& in_access_limit);
	GENERIC set_maximum_context_length(const U32& in_context_length);
	flags add_accessible_model(const mbase::string& in_modelname);
	flags remove_accessible_model(const mbase::string& in_modelname);
	GENERIC set_username(const mbase::string& in_username);
	GENERIC add_authority_flags(U32 in_flags);
	GENERIC remove_authority_flags(U32 in_flags);
	GENERIC make_superuser();

	bool is_superuser();
	bool is_authorization_locked();
	bool is_flags_set(U32 in_flags);
	bool is_model_accessible(const mbase::string& in_username);

private:
	U32 mAuthorityFlags;
	U32 mDistinctModelAccessLimit;
	U32 mMaximumContextLength;
	model_name_vector mAccessibleModels;
	mbase::string mUsername;
	bool mIsSuperUser;
	bool mIsAuthorizationLocked; // If this is true, new clients won't be able to associate themselves with this user.
};

MBASE_END

#endif // !MBASE_INF_MAIP_SECURITY_H
