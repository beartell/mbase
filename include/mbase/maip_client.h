#ifndef MBASE_MAIP_CLIENT_H
#define MBASE_MAIP_CLIENT_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/maip_parser.h>

MBASE_STD_BEGIN

#define MBASE_MAIP_CLIENT_CHECK_SESSION \
if(!this->mSessionToken.size())\
{\
	return false;\
}\
maip_packet_builder packetBuilder;\
packetBuilder.set_kval("STOK", this->mSessionToken);

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
	UNMAKE_SUPERUSER,
	SET_INPUT,
	EXECUTE_INPUT,
	NEXT
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

	bool access_request(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_payload);
	bool destroy_session(mbase::string& out_payload);
	bool get_accessible_models(mbase::string& out_payload);
	bool get_context_ids(mbase::string& out_payload);
	bool create_context(const mbase::string& in_model, const U32& in_ctx_size, mbase::string& out_payload);
	bool get_program_models(mbase::string& out_payload);
	bool load_model(const mbase::string& in_model, mbase::string& out_payload);
	bool unload_model(const mbase::string& in_model, mbase::string& out_payload);
	bool create_user(
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
	bool delete_user(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_payload);
	bool modify_user_model_access_limit(const mbase::string& in_username, const U32& in_access_limit, mbase::string& out_payload);
	bool modify_user_maximum_context_length(const mbase::string& in_username, const U32& in_context_length, mbase::string& out_payload);
	bool modify_user_batch_size(const mbase::string& in_username, const U32& in_batch_length, mbase::string& out_payload);
	bool modify_user_processor_thread_count(const mbase::string& in_username, const U32& in_thread_count, mbase::string& out_payload);
	bool modify_user_max_processor_thread_count(const mbase::string& in_username, const U32& in_thread_count, mbase::string& out_payload);
	bool modify_user_system_prompt(const mbase::string& in_username, const mbase::string& in_system_prompt, mbase::string& out_payload);
	bool modify_user_make_superuser(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_payload);
	bool modify_user_unmake_superuser(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_payload);

	GENERIC resolve_packet(CBYTEBUFFER in_data, size_type in_size);
	virtual GENERIC	on_resolve(const maip_operation& out_last_operation, const maip_peer_request& out_result);

private:
	maip_operation mLastOperation;
	mbase::string mSessionToken = "123456";
};

class maip_context {
public:
	using size_type = SIZE_T;

	maip_context(const mbase::string& in_session_token, const U64& in_context_id);

	bool set_input(maip_input_role in_role, const mbase::string& in_input, mbase::string& out_payload);
	bool execute_input(const mbase::vector<U32>& in_msg_ids, mbase::string& out_payload);
	bool next();

	GENERIC resolve_packet(CBYTEBUFFER in_data, size_type in_size);
	virtual GENERIC on_resolve(const maip_operation& out_last_operation, const maip_peer_request& out_result);

private:
	maip_operation mLastOperation;
	U64 mContextId;
	mbase::string mSessionToken;
};

bool maip_client::access_request(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_payload)
{
	if(!in_username.size() || !in_access_token.size())
	{
		return false;
	}
	maip_packet_builder packetBuilder;

	packetBuilder.set_request_message("INF", "inf_access_request");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("ACCTOK", in_access_token);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::ACCESS_REQUEST;
	return true;
}

bool maip_client::destroy_session(mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_destroy_session");
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::DESTROY_SESSION;
	return true;
}

bool maip_client::get_accessible_models(mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;
	
	packetBuilder.set_request_message("INF", "inf_get_accessible_models");
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::GET_ACCESSIBLE_MODELS;
	return true;
}

bool maip_client::get_context_ids(mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_get_context_ids");
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::GET_CONTEXT_IDS;
	return true;
}

bool maip_client::create_context(const mbase::string& in_model, const U32& in_ctx_size, mbase::string& out_payload)
{	
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	if(!in_model.size())
	{
		return false;
	}

	packetBuilder.set_request_message("INF", "inf_create_context");
	packetBuilder.set_kval("MODEL", in_model);
	packetBuilder.set_kval("CTXSIZE", in_ctx_size);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::CREATE_CONTEXT;
	return true;
}

bool maip_client::get_program_models(mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_get_program_models");
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::GET_PROGRAM_MODELS;
	return true;
}

bool maip_client::load_model(const mbase::string& in_model, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	if(!in_model.size())
	{
		return false;
	}

	packetBuilder.set_request_message("INF", "inf_load_model");
	packetBuilder.set_kval("MODEL", in_model);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::LOAD_MODEL;
	return true;
}

bool maip_client::unload_model(const mbase::string& in_model, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	if(!in_model.size())
	{
		return false;
	}

	packetBuilder.set_request_message("INF", "inf_unload_model");
	packetBuilder.set_kval("MODEL", in_model);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::UNLOAD_MODEL;
	return true;
}

bool maip_client::create_user(
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
)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_create_new_user");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("ACCTOK", in_access_token);
	packetBuilder.set_kval("ACCLIMIT", in_model_access_limit);
	packetBuilder.set_kval("CTXLENGTH", in_maximum_context_length);
	packetBuilder.set_kval("BATCH_SET", in_batch_length);
	packetBuilder.set_kval("MAX_PROC_THREADS", in_max_proc_threads);
	packetBuilder.set_kval("PROC_THREADS", in_proc_threads);
	for(auto& authFlag : in_authority_flags)
	{
		packetBuilder.set_kval("AUTHFLAGS", authFlag);
	}
	packetBuilder.set_kval("ISSUPER", in_superuser);
	packetBuilder.set_kval("ISSTATIC", in_is_static);
	packetBuilder.generate_payload(out_payload, in_system_prompt);

	mLastOperation = maip_operation::CREATE_NEW_USER;
	return true;
}

bool maip_client::delete_user(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	if(!in_username.size() || !in_access_token.size())
	{
		return false;
	}

	packetBuilder.set_request_message("INF", "inf_delete_user");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("ACCTOK", in_access_token);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::DELETE_USER;
	return true;
}

bool maip_client::modify_user_model_access_limit(const mbase::string& in_username, const U32& in_access_limit, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_modify_user_model_access_limit");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("ACCLIMIT", in_access_limit);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::MODIFY_USER_MODEL_ACCESS_LIMIT;
	return true;
}

bool maip_client::modify_user_maximum_context_length(const mbase::string& in_username, const U32& in_context_length, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_modify_user_maximum_context_length");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("CTXLENGTH", in_context_length);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::MODIFY_USER_MAX_CONTEXT_LENGTH;
	return true;
}

bool maip_client::modify_user_batch_size(const mbase::string& in_username, const U32& in_batch_length, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_modify_user_batch_size");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("BATCH_SET", in_batch_length);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::MODIFY_USER_BATCH_SIZE;
	return true;
}

bool maip_client::modify_user_processor_thread_count(const mbase::string& in_username, const U32& in_thread_count, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_modify_user_processor_thread_count");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("PROC_THREADS", in_thread_count);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::MODIFY_USER_PROC_THREAD_COUNT;
	return true;
}

bool maip_client::modify_user_max_processor_thread_count(const mbase::string& in_username, const U32& in_thread_count, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_modify_user_max_processor_thread_count");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("MAX_PROC_THREADS", in_thread_count);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::MODIFY_USER_PROC_MAX_THREAD_COUNT;
	return true;
}

bool maip_client::modify_user_system_prompt(const mbase::string& in_username, const mbase::string& in_system_prompt, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_modify_user_system_prompt");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.generate_payload(out_payload, in_system_prompt);

	mLastOperation = maip_operation::MODIFY_USER_SYSTEM_PROMPT;
	return true;
}

bool maip_client::modify_user_make_superuser(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_modify_user_make_superuser");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("ACCTOK", in_access_token);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::MAKE_SUPERUSER;
	return true;
}

bool maip_client::modify_user_unmake_superuser(const mbase::string& in_username, const mbase::string& in_access_token, mbase::string& out_payload)
{
	MBASE_MAIP_CLIENT_CHECK_SESSION;

	packetBuilder.set_request_message("INF", "inf_modify_user_unmake_superuser");
	packetBuilder.set_kval("USERNAME", in_username);
	packetBuilder.set_kval("ACCTOK", in_access_token);
	packetBuilder.generate_payload(out_payload);

	mLastOperation = maip_operation::UNMAKE_SUPERUSER;
	return true;
}

GENERIC maip_client::resolve_packet(CBYTEBUFFER in_data, size_type in_size)
{

}

GENERIC	maip_client::on_resolve(const maip_operation& out_last_operation, const maip_peer_request& out_result)
{

}

MBASE_STD_END

#endif // !MBASE_MAIP_CLIENT_H
