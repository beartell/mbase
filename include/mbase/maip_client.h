#ifndef MBASE_MAIP_CLIENT_H
#define MBASE_MAIP_CLIENT_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/maip_parser.h>

MBASE_STD_BEGIN

enum class maip_operation {
	ACCESS_REQUEST,
	DESTROY_SESSION,
	GET_ACCESSIBLE_MODELS,
	GET_CONTEXT_IDS,
	CREATE_CONTEXT,
	CLEAR_CONTEXT_HISTORY,
	GET_CONTEXT_STATUS,
	DESTROY_CONTEXT,
	GET_PROGRAM_MODELS,
	LOAD_MODEL,
	UNLOAD_MODEL,
	CREATE_NEW_USER,
	DELETE_USER,
	MODIFY_USER_MODEL_ACCESS_LIMIT,
	MODIFY_USER_MAX_CONTEXT_LENGTH,
	MODIFY_USER_BATCH_SIZE,
	MODIFY_USER_PROC_THREAD_COUNT,
	MODIFY_USER_PROC_MAX_THREAD_COUNT,
	MODIFY_USER_SYSTEM_PROMPT,
	MODIFY_USER_ACCEPT_MODELS,
	MODIFY_USER_AUTHORITY,
	MAKE_SUPERUSER,
	UNMAKE_SUPERUSER
};

enum class maip_input_role {
	SYSTEM,
	ASSISTANT,
	USER,
	NONE
};

class maip_client {
public:
	using size_type = SIZE_T;

	bool access_request(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_packet);
	bool destroy_session(mbase::string& out_payload);
	GENERIC get_accessible_models(mbase::string& out_payload);
	GENERIC get_context_ids(mbase::string& out_payload);
	bool create_context(const mbase::string& in_model, const U32& in_ctx_size, mbase::string& out_payload);
	GENERIC get_program_models(mbase::string& out_payload);
	bool load_model(const mbase::string& in_model, mbase::string& out_payload);
	bool unload_model(const mbase::string& in_model, mbase::string& out_payload);
	bool create_user(
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
		mbase::string& out_payload
	);
	bool delete_user(const mbase::string& in_username, const mbase::string& in_access_token);
	bool modify_user_model_access_limit(const mbase::string& in_username, const U32& in_access_limit);
	bool modify_user_maximum_context_length(const mbase::string& in_username, const U32& in_context_length);
	bool modify_user_batch_size(const mbase::string& in_username, const U32& in_batch_length);
	bool modify_user_processor_thread_count(const mbase::string& in_username, const U32& in_thread_count);
	bool modify_user_max_processor_thread_count(const mbase::string& in_username, const U32& in_thread_count);
	bool modify_user_system_prompt(const mbase::string& in_username, const mbase::string& in_system_prompt);
	bool modify_user_make_superuser(const mbase::string& in_username);
	bool modify_user_unmake_superuser(const mbase::string& in_username);

	GENERIC resolve_packet(CBYTEBUFFER in_data, size_type in_size);
	virtual GENERIC	on_resolve(const maip_operation& out_last_operation, const maip_peer_request& out_result);

private:
	maip_operation mLastOperation;
	mbase::string mSessionToken;
};

class maip_context {
public:
	using size_type = SIZE_T;

	maip_context(const mbase::string& in_session_token, const U64& in_context_id);

	bool set_input(maip_input_role in_role, const mbase::string& in_input, mbase::string& out_payload);
	bool execute_input(const mbase::vector<U32>& in_msg_ids, mbase::string& out_payload);
	GENERIC next();

	GENERIC resolve_packet(CBYTEBUFFER in_data, size_type in_size);
	virtual GENERIC on_resolve(const maip_operation& out_last_operation, const maip_peer_request& out_result);

private:
	maip_operation mLastOperation;
	U64 mContextId;
	mbase::string mSessionToken;
};

MBASE_STD_END

#endif // !MBASE_MAIP_CLIENT_H
