#ifndef MBASE_INF_PROGRAM_H
#define MBASE_INF_PROGRAM_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/unordered_map.h>
#include <mbase/traits.h>
#include <mbase/pc/pc_net_manager.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_program.h>
#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_maip_security.h>
#include <unordered_map>

MBASE_BEGIN

struct InfClientSession;
class InfMaipTunedClient;
class InfMaipTunedT2TProcessor;
class InfProgram;

class MBASE_API InfMaipTunedClient : public mbase::InfClientTextToText {
public:
	InfMaipTunedClient();
	InfMaipTunedClient(InfClientSession& in_client);

	GENERIC on_register(InfTextToTextProcessor* out_processor) override;
	GENERIC on_write(CBYTEBUFFER out_data, size_type out_size, InfTextToTextProcessor::inf_token out_token, bool out_is_special, bool out_is_finish) override;
	GENERIC on_finish(size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override;
	GENERIC on_unregister() override;

	mbase::string lastToken;
	InfClientSession* mManagerClient;
	U64 mCurrentContextIndex;
	bool mIsSessionAlive;
};

class MBASE_API InfMaipTunedT2TProcessor : public mbase::InfTextToTextProcessor {
public:
	GENERIC on_initialize_fail(init_fail_code out_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;

	GENERIC set_nominee_client(InfMaipTunedClient* in_nominee);

private:
	InfMaipTunedClient* mNomineeClient = NULL;
};

struct MBASE_API InfClientSession {
	using chat_session_map = std::unordered_map<U64, InfMaipTunedClient*>;
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
	mbase::wstring mExecutionPath;
	mbase::wstring mTempPath;
};

struct MBASE_API InfRegisteredModelInformation {
	mbase::wstring mModelPath;
	mbase::string mModelName;
	mbase::string mModelArchitecture;
	mbase::string mSystemPrompt;
	F32 mQuantizationCoefficient = 0.0f;
	U32 mBlockCount = 0;
	U32 mHeadCount = 0;
	U32 mEmdeddingLength = 0;
	U64 mModelSize = 0;
};

class MBASE_API InfProgram : public mbase::PcProgramBase {
public:
	using accepted_client_map = std::unordered_map<mbase::string, InfClientSession>;
	using registered_model_map = std::unordered_map<mbase::string, InfModelTextToText*>;
	using model_information_map = std::unordered_map<mbase::string, InfRegisteredModelInformation>;
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
	maip_err_code inf_create_context(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_model, const U32& in_ctsize, const mbase::vector<InfSamplingInput>& in_samplers = mbase::vector<InfSamplingInput>());
	maip_err_code inf_clear_context_history(const mbase::string& in_session_token, const U64& in_ctxId);
	maip_err_code inf_get_context_status(const mbase::string& in_session_token, const U64& in_ctxId);
	maip_err_code inf_destroy_context(const mbase::string& in_session_token, const U64& in_ctxId);
	maip_err_code inf_get_program_models(const mbase::string& in_session_token, mbase::vector<mbase::string>& out_models);
	maip_err_code inf_load_model(const mbase::string& in_session_token, const mbase::string& in_modelname, const U32& in_total_context_size);
	maip_err_code inf_unload_model(const mbase::string& in_session_token, const mbase::string& in_modelname);
	//maip_err_code inf_load_adapter(const mbase::string& in_session_token, const mbase::string& in_adapter_name);
	//maip_err_code inf_unload_adapter(const mbase::string& in_session_token, const mbase::string& in_adapter_name);
	maip_err_code inf_create_new_user(
		const mbase::string& in_session_token,
		const mbase::string& in_username,
		const U32& in_model_access_limit,
		const U32& in_maximum_context_length,
		const bool& in_superuser,
		const bool& in_authorization_locked,
		const mbase::string& in_access_token,
		const mbase::vector<mbase::string>& in_authority_flags,
		mbase::string& out_access_token
	);
	maip_err_code inf_delete_user(const mbase::string& in_session_token, const mbase::string& in_username);
	maip_err_code inf_modify_user_model_access_limit(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_new_access_limit);
	maip_err_code inf_modify_user_maximum_context_length(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_maximum_context_length);
	
	maip_err_code inf_modify_user_batch_size(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_batch_size); // Implement
	maip_err_code inf_modify_user_processor_thread_count(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_thread_count); // Implement
	maip_err_code inf_modify_user_batch_thread_count(const mbase::string& in_session_token, const mbase::string& in_username, const U32& in_thread_count); // Implement
	// maip_err_code inf_modify_user_sampling_set(const mbase::string& in_session_token, const mbase::string& in_username, const inf_sampling_set& in_sampling_set) /* Implement */
	maip_err_code inf_modify_user_system_prompt(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_system_prompt); // Implement

	maip_err_code inf_modify_user_make_superuser(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::string& in_access_token);
	maip_err_code inf_modify_user_unmake_superuser(const mbase::string& in_session_token, const mbase::string& in_username);
	maip_err_code inf_modify_user_accept_models(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::vector<mbase::string>& in_models);
	maip_err_code inf_modify_user_set_authority_flags(const mbase::string& in_session_token, const mbase::string& in_username, const mbase::vector<mbase::string>& in_authority_flags);
	
	maip_err_code exec_set_input(const mbase::string& in_session_token, const U64& in_ctxId, mbase::context_role in_role, const mbase::string& in_input, U32& out_msgid);
	maip_err_code exec_execute_input(const mbase::string& in_session_token, const U64& in_ctxId, mbase::vector<U32>& in_msgid); // TODO: CHANGE CONTENT
	maip_err_code exec_next(const mbase::string& in_session_token, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const U64& in_ctxId);

	GENERIC initialize(InfProgramInformation in_program_information);
	flags host_model(InfModelTextToText* in_model);
	flags release_model(const mbase::string& in_model_name);
	flags create_user(const mbase::string& in_username, // TODO: Check if the username contains non-printable characters. Check if the username contains '/', '.', "..", '*' to avoid filesystem alterations
		const U32& in_model_access_limit,
		const U32& in_maximum_context_length,
		const bool& in_superuser,
		const bool& in_authorization_locked,
		const mbase::string& in_access_token,
		const U32& in_authority_flags,
		const bool& in_is_permanent,
		mbase::string& out_access_token
	);
	flags update_users_model_access_limit(const mbase::string& in_username, const U32& in_new_access_limit);
	flags update_users_maximum_context(const mbase::string& in_username, const U32& in_new_context_length);
	flags authorize_user_on_model(const mbase::string& in_username, const mbase::string& in_model);
	//flags delete_user(const mbase::string& in_username);

	GENERIC update_maip_user_sessions(const InfMaipUser& in_maip_user);
	GENERIC update() override;

private:
	PcDiagnostics mInferenceDiagnostics;
	PcConfig mInferenceConfigurator;
	PcNetManager mInferenceNetManager;
	PcState mMainProgramState;

	accepted_client_map mSessionMap;
	registered_model_map mRegisteredModels;
	model_information_map mModelInformationMap;
	inference_user_map mUserMap;
	U64 mClientSessionIdCounter = 0;
	U32 mModelHostingLimit;
	U32 mDefaultModelAccessLimit;
	U32 mDefaultContextLimit;
	mbase::wstring mClientStateDirectory;
	mbase::wstring mModelsDirectory;
};

MBASE_END

#endif // !MBASE_INF_PROGRAM_H