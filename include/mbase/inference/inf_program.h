#ifndef MBASE_INF_PROGRAM_H
#define MBASE_INF_PROGRAM_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/unordered_map.h>
#include <mbase/traits.h>
#include <mbase/queue.h>
#include <mbase/pc/pc_net_manager.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_program.h>
#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_maip_user.h>
#include <mbase/inference/inf_maip_model_description.h>
#include <unordered_map>
#include <set>

MBASE_BEGIN

struct InfClientSession;
class InfMaipTunedClient;
class InfMaipTunedT2TProcessor;
class InfProgram;

class MBASE_API InfMaipTunedClient : public mbase::InfClientTextToText {
public:
	InfMaipTunedClient();
	GENERIC set_session(InfClientSession& in_session);
	GENERIC set_embedder_message_queue(mbase::vector<U32>& in_msg_ids);
	bool proc_next_embedding();

	GENERIC on_register(InfProcessorBase* out_processor) override; // 100% called if the proc init is successful
	GENERIC on_embedding_data(const F32* out_data, size_type out_size) override; // For embedder models
	GENERIC on_write(const inf_text_token_vector& out_token, bool out_is_finish) override;
	GENERIC on_finish(size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override;
	GENERIC on_unregister() override; // 100% called if the proc destroy is called either in stack or heap
private:
	mbase::queue<U32> mEmbeddingMessageIndexes;
	inf_token_description lastToken;
	InfClientSession* mManagerClient;
	U64 mCurrentContextIndex;
};

class MBASE_API InfMaipTunedT2TProcessor : public mbase::InfTextToTextProcessor {
public:
	InfMaipTunedT2TProcessor(InfClientSession& in_client, InfProgram* in_program_instance);

	GENERIC on_initialize_fail(last_fail_code out_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;

private:
	InfProgram* mProgramInstance;
	InfClientSession* mManagerClient;
	InfMaipTunedClient mProcessorClient;
};

class MBASE_API InfMaipTunedEmbedderProcessor : public mbase::InfEmbedderProcessor {
public:
	InfMaipTunedEmbedderProcessor(InfClientSession& in_client, InfProgram* in_program_instance);

	GENERIC on_initialize_fail(init_fail_code out_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;

private:
	InfProgram* mProgramInstance;
	InfClientSession* mManagerClient;
	InfMaipTunedClient mProcessorClient;
};

struct MBASE_API InfClientSession {
	using chat_session_map = std::unordered_map<U64, InfProcessorBase*>;
	using size_type = SIZE_T;

	InfClientSession() = default;
	~InfClientSession();

	std::shared_ptr<mbase::PcNetPeerClient> mPeer;
	chat_session_map mChatSessions;
	InfMaipUser mMaipUser;
	U64 mContextCounter = 0;
};

struct MBASE_API InfProgramInformation {
	PcProgramInformation mProgramInformation;
	mbase::wstring mDataPath;
	mbase::wstring mConfigPath;
	mbase::wstring mTempPath;
};

class MBASE_API InfProgram : public mbase::PcProgramBase {
public:
	using dead_model_vector = mbase::vector<InfModelBase*>;
	using dead_processor_vector = mbase::vector<InfProcessorBase*>;
	using accepted_client_map = std::unordered_map<mbase::string, InfClientSession>;
	using registered_model_map = std::unordered_map<mbase::string, InfModelTextToText*>;
	using model_description_map = std::unordered_map<mbase::string, InfMaipModelDescription>;
	using inference_user_map = std::unordered_map<mbase::string, InfMaipUser>;

	enum class flags : U8 {
		INF_PROGRAM_SUCCESS,
		INF_PROGRAM_ERR_MODEL_ALREADY_BEING_HOSTED,
		INF_PROGRAM_ERR_MODEL_IS_NOT_INITIALIZED,
		INF_PROGRAM_ERR_MODEL_NAME_MISMATCH,
		INF_PROGRAM_ERR_MODEL_MISSING,
		INF_PROGRAM_ERR_USR_NAME_NOT_GIVEN,
		INF_PROGRAM_ERR_USR_NAME_TOO_LONG,
		INF_PROGRAM_ERR_USR_ALREADY_EXISTS,
		INF_PROGRAM_ERR_USR_NOT_FOUND
	};

	enum class maip_err_code : U16 {
		INF_SUCCESS = 2000,
		INF_MAXIMUM_CLIENTS = 2001,
		INF_SESSION_TOKEN_MISMATCH = 2002,
		INF_UNAUTHORIZED_ACCESS = 2003,
		INF_MODEL_NAME_MISMATCH = 2004,
		INF_CONTEXT_ID_MISMATCH = 2005,
		INF_INVALID_TOKEN_LIMIT = 2006,
		INF_FAILED_TO_CREATE_CONTEXT = 2007,
		INF_AUTHORIZATION_FAILED = 2008,
		INF_CONTEXT_ACTIVE = 2009,
		INF_CONTEXT_INACTIVE = 2010,
		INF_CONTEXT_INPUT_IS_EMPTY = 2011,
		INF_CONTEXT_INITIALIZING = 2012,
		INF_CONTEXT_HALTED = 2013,
		INF_MODEL_IS_NOT_ACCESSIBLE = 2014,
		INF_USER_ALREADY_EXISTS = 2015,
		INF_INVALID_PARAMS = 2016,
		INF_CANT_DELETE_SELF = 2017,
		INF_CONTEXT_LENGTH_EXCEEDED = 2018,
		INF_MODEL_CONTEXT_FULL = 2019,
		INF_USER_CONTEXT_LENGTH_EXCEEDED = 2020,
		INF_USER_NOT_FOUND = 2021,
		INF_TARGET_MODEL_ACCESS_PROHIBITED = 2022,
		INF_NOT_ENOUGH_MEMORY = 2023,
		INF_DESCRIPTION_ALREADY_EXISTS = 2024,
		INF_MODEL_ALREADY_LOADED = 2025,
		INF_UNABLE_TO_OPEN_MODEL_FILE = 2026,
		EXEC_SUCCESS = 3000,
		EXEC_ALREADY_PROCESSING = 3001,
		EXEC_MESSAGE_ID_MISMATCH = 3002,
		EXEC_MISSING_MESSAGE = 3003,
		EXEC_TOKENIZATION_FAILED = 3004,
		EXEC_TOKEN_LIMIT_EXCEEDED = 3005,
		EXEC_MESSAGE_CONTINUE = 3006,
		EXEC_MESSAGE_FINISH = 3007,
		EXEC_ABANDONED = 3008,
		EXEC_PROCESS_HALTED = 3009
	};

	bool is_session_token_valid(const mbase::string& in_session_token);
	#ifdef MBASE_INTERNAL_API
	accepted_client_map& get_accepted_clients();
	registered_model_map& get_registered_models();
	#endif
	
	maip_err_code inf_access_request(const mbase::string& in_username, const mbase::string& in_access_token, std::shared_ptr<PcNetPeerClient> in_client, mbase::string& out_session_token);
	maip_err_code inf_destroy_session(const mbase::string& in_session_token);
	maip_err_code inf_get_accessible_models(const mbase::string& in_session_token, mbase::vector<mbase::string>& out_models);
	maip_err_code inf_get_context_ids(const mbase::string& in_session_token, mbase::vector<U64>& out_contexts);
	maip_err_code inf_create_context(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_model, const U32& in_ctsize);
	maip_err_code inf_clear_context_history(const mbase::string& in_session_token, const U64& in_ctxId);
	maip_err_code inf_get_context_status(const mbase::string& in_session_token, const U64& in_ctxId);
	maip_err_code inf_destroy_context(const mbase::string& in_session_token, const U64& in_ctxId);
	maip_err_code inf_get_program_models(const mbase::string& in_session_token, mbase::vector<mbase::string>& out_models);
	maip_err_code inf_load_model(const mbase::string& in_session_token, const mbase::string& in_modelname, const U32& in_total_context_size);
	maip_err_code inf_unload_model(const mbase::string& in_session_token, const mbase::string& in_modelname);
	maip_err_code inf_create_new_user(
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
		mbase::string& out_access_token
	);
	maip_err_code inf_delete_user(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_access_token);
	maip_err_code inf_modify_user_model_access_limit(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_new_access_limit);
	maip_err_code inf_modify_user_maximum_context_length(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_maximum_context_length);
	maip_err_code inf_modify_user_batch_size(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_batch_size);
	maip_err_code inf_modify_user_processor_thread_count(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_thread_count);
	maip_err_code inf_modify_user_max_processor_thread_count(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_thread_count);
	// maip_err_code inf_modify_user_sampling_set(const mbase::string& in_session_token, const mbase::string& in_username, const inf_sampling_set& in_sampling_set) /* Implement */
	maip_err_code inf_modify_user_system_prompt(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_system_prompt);
	maip_err_code inf_modify_user_make_superuser(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_access_token);
	maip_err_code inf_modify_user_unmake_superuser(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_access_token);
	maip_err_code inf_modify_user_accept_models(const mbase::string& in_session_token, const mbase::string& in_username, const std::set<mbase::string>& in_models, mbase::vector<mbase::string>& out_missing_models);
	maip_err_code inf_modify_user_set_authority_flags(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::vector<mbase::string>& in_authority_flags);
	maip_err_code inf_create_model_description(
		const mbase::string& in_session_token,
		const mbase::string& in_original_name,
		const mbase::string& in_custom_name,
		const mbase::string& in_description,
		const mbase::string& in_system_prompt,
		const mbase::string& in_model_file,
		const mbase::vector<mbase::string>& in_tags,
		const mbase::string& in_category,
		const bool& in_is_embedding_model,
		const bool& in_force_system_prompt,
		const U32& in_total_context_length
	);
	maip_err_code inf_modify_original_model_name(
		const mbase::string& in_session_token,
		const mbase::string& in_model_target,
		const mbase::string& in_name
	);
	maip_err_code inf_modify_custom_model_name(
		const mbase::string& in_session_token,
		const mbase::string& in_model_target,
		const mbase::string& in_name
	);
	maip_err_code inf_modify_model_description(
		const mbase::string& in_session_token,
		const mbase::string& in_model_target,
		const mbase::string& in_description
	);
	maip_err_code inf_modify_model_system_prompt(
		const mbase::string& in_session_token,
		const mbase::string& in_model_target,
		const mbase::string& in_system_prompt
	);
	maip_err_code inf_modify_model_model_file(
		const mbase::string& in_session_token,
		const mbase::string& in_model_target,
		const mbase::string& in_model_file,
		const bool& in_is_embedding,
		const mbase::string& in_model_category
	);
	maip_err_code inf_modify_model_tags(
		const mbase::string& in_session_token,
		const mbase::string& in_model_target,
		const mbase::vector<mbase::string>& in_tags
	);
	maip_err_code inf_modify_model_context_length(
		const mbase::string& in_session_token,
		const mbase::string& in_model_target,
		const U32& in_maximum_context
	);

	maip_err_code exec_set_input(const mbase::string& in_session_token, const U64& in_ctxId, mbase::context_role in_role, const mbase::string& in_input, U32& out_msgid);
	maip_err_code exec_set_input(const mbase::string& in_session_token, const U64& in_ctxId, mbase::context_role in_role, CBYTEBUFFER in_input, const size_type& in_length, U32& out_msgid);
	maip_err_code exec_execute_input(const mbase::string& in_session_token, const U64& in_ctxId, mbase::vector<U32>& in_msgid); // TODO: CHANGE CONTENT
	maip_err_code exec_next(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const U64& in_ctxId);

	GENERIC push_dead_model(InfModelBase& in_model);
	GENERIC push_dead_processor(InfProcessorBase& in_processor);
	GENERIC initialize(InfProgramInformation in_program_information);
	GENERIC update() override;
	flags create_user(
		const mbase::string& in_username,
		const mbase::string& in_access_token,
		const mbase::string& in_system_prompt,
		const U32& in_authority_flags,
		const U32& in_model_access_limit,
		const U32& in_maximum_context_length,
		const U32& in_batch_length,
		const U32& in_max_proc_threads,
		const U32& in_proc_threads,
		const bool& in_superuser,
		const bool& in_is_static,
		const inf_sampling_set& in_sampling_set,
		mbase::string& out_access_token
	);
private:
	flags host_model(InfModelTextToText* in_model);
	flags release_model(const mbase::string& in_model_name);
	flags update_users_model_access_limit(const mbase::string& in_username, const U32& in_new_access_limit);
	flags update_users_maximum_context(const mbase::string& in_username, const U32& in_new_context_length);
	flags authorize_user_on_model(const mbase::string& in_username, const mbase::string& in_model);
	maip_err_code common_modification_control(InfClientSession& in_session, const mbase::string& in_username, const U32& in_flags);
	maip_err_code common_description_modification_control(InfClientSession& in_session, const mbase::string& in_model_target);
	GENERIC update_maip_user_sessions(InfMaipUser& in_maip_user);
	GENERIC _reload_model_descriptions();
	GENERIC _load_user_states();

	PcDiagnostics mInferenceDiagnostics;
	PcConfig mInferenceConfigurator;
	PcNetManager mInferenceNetManager;
	PcState mMainProgramState;

	dead_model_vector mDeadModelVector;
	dead_processor_vector mDeadProcessorVector;
	accepted_client_map mSessionMap;
	registered_model_map mRegisteredModels;
	model_description_map mModelDescriptionMap;
	inference_user_map mUserMap;
	U64 mClientSessionIdCounter = 0;
	U32 mModelHostingLimit;
	U32 mDefaultModelAccessLimit;
	U32 mDefaultContextLimit;
	mbase::wstring mClientStateDirectory;
	mbase::wstring mDescriptionDirectory;
	mbase::wstring mModelDirectory;
};

MBASE_END

#endif // !MBASE_INF_PROGRAM_H