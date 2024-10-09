#ifndef MBASE_INF_PROGRAM_H
#define MBASE_INF_PROGRAM_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/unordered_map.h>
#include <mbase/traits.h>
#include <mbase/pc/pc_net_manager.h>
#include <mbase/pc/pc_state.h>
#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_client.h>

MBASE_BEGIN

#define MBASE_MAIP_CL_AUTH \
const U64& in_csid, const mbase::string& in_clid\

struct InfAcceptedClient;
class InfMaipTunedClient;
class InfMaipTunedT2TProcessor;
class InfProgram;
class ClientContextCleaner;

class MBASE_API InfMaipTunedClient : public mbase::InfClientTextToText {
public:
	InfMaipTunedClient();
	InfMaipTunedClient(InfAcceptedClient& in_client);

	GENERIC on_register(InfTextToTextProcessor* out_processor) override;
	GENERIC on_write(CBYTEBUFFER out_data, size_type out_size, InfTextToTextProcessor::inf_token out_token, bool out_is_special, bool out_is_finish) override;
	GENERIC on_finish(size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override;
	GENERIC on_unregister() override;

	mbase::string lastToken;
	InfAcceptedClient* mManagerClient;
	bool mIsDeadClient;
};

class MBASE_API InfMaipTunedT2TProcessor : public mbase::InfTextToTextProcessor {
public:
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;

	GENERIC set_nominee_client(InfMaipTunedClient* in_nominee);

private:
	InfMaipTunedClient* mNomineeClient = NULL;
};

struct MBASE_API InfAcceptedClient {
	using chat_session_map = mbase::unordered_map<U64, InfMaipTunedClient*>;
	using size_type = SIZE_T;

	std::shared_ptr<mbase::PcNetPeerClient> mPeer;
	mbase::vector<mbase::string> mAcceptedModels; // TODO: WRITE TO FILE
	chat_session_map mChatSessions;
	mbase::string mClid = ""; // TODO: WRITE TO FILE
	U64 mCsId = 0; // TODO: WRITE TO FILE
	U64 mChatSessionIdCounter = 0;
	bool mTemporaryClient = true;

	size_type get_serialized_size() const noexcept;
	GENERIC serialize(char_stream& out_stream) const;
	static InfAcceptedClient deserialize(IBYTEBUFFER in_src, size_type in_length, SIZE_T& bytes_processed);
};

class MBASE_API InfProgram {
public:
	using accepted_client_map = mbase::unordered_map<U64, InfAcceptedClient>;
	using registered_model_map = mbase::unordered_map<mbase::string, InfModelTextToText*>;

	enum class flags : U8 {
		INF_PROGRAM_SUCCESS,
		INF_PROGRAM_ERR_MODEL_ALREADY_BEING_HOSTED,
		INF_PROGRAM_ERR_MODEL_IS_NOT_INITIALIZED,
		INF_PROGRAM_ERR_MODEL_NAME_MISMATCH,
		INF_PROGRAM_ERR_MODEL_MISSING
	};

	enum class maip_err_code : U16 {
		INF_SUCCESS = 2000,
		INF_MAXIMUM_CLIENTS = 2001,
		INF_CLIENT_ID_MISMATCH = 2002,
		INF_UNAUTHORIZED_ACCESS = 2003,
		INF_MODEL_NAME_MISMATCH = 2004,
		INF_CONTEXT_ID_MISMATCH = 2005,
		INF_INVALID_TOKEN_LIMIT = 2006,
		INF_FAILED_TO_CREATE_CONTEXT = 2007,
		INF_UNKNOWN_STATUS = 2008,
		INF_CLIENT_UNREGISTERING = 2009,
		INF_CLIENT_NOT_REGISTERED = 2010,
		INF_CONTEXT_PROCESSING = 2011,
		INF_CONTEXT_ACTIVE = 2012,
		INF_CONTEXT_INACTIVE = 2013,
		INF_CONTEXT_HOST_MODEL_SYSTEM_ERROR = 2014,
		INF_CONTEXT_INPUT_IS_EMPTY = 2015,
		INF_CONTEXT_INITIALIZING = 2016,
		INF_CONTEXT_DESTROYING = 2017,
		INF_CONTEXT_HALTED = 2018,
		EXEC_SUCCESS = 3000,
		EXEC_ALREADY_PROCESSING = 3001,
		EXEC_MESSAGE_ID_MISMATCH = 3002,
		EXEC_MISSING_MESSAGE = 3003,
		EXEC_TOKENIZATION_FAILED = 3004,
		EXEC_TOKEN_LIMIT_EXCEEDED = 3005,
		EXEC_MESSAGE_CONTINUE = 3006,
		EXEC_MESSAGE_FINISH = 3007,
		EXEC_ABANDONED = 3008,
		EXEC_CONTEXT_INACTIVE = 3009,
		EXEC_UNKNOWN_STATUS = 3010
	};

	bool is_session_match(MBASE_MAIP_CL_AUTH);
	#ifdef MBASE_INTERNAL_API
	accepted_client_map& get_accepted_clients();
	#endif
	
	maip_err_code inf_create_session(const mbase::string& in_clid, U64& out_csid, mbase::string& out_clid, bool in_is_temporary = true);
	maip_err_code inf_authorize(MBASE_MAIP_CL_AUTH);
	maip_err_code inf_destroy_client(MBASE_MAIP_CL_AUTH);
	maip_err_code inf_get_acquired_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models);
	maip_err_code inf_get_created_context_ids(MBASE_MAIP_CL_AUTH, mbase::vector<U64>& out_contexts);
	maip_err_code inf_create_context(MBASE_MAIP_CL_AUTH, const mbase::string& in_model, const U32& in_ctsize, U64& out_ctxId, const mbase::vector<InfSamplingInput>& in_samplers = mbase::vector<InfSamplingInput>()); // TODO: CHANGE CONTENT
	maip_err_code inf_clear_short_term_history(MBASE_MAIP_CL_AUTH, const U64& in_ctxId);
	maip_err_code inf_get_context_status(MBASE_MAIP_CL_AUTH, const U64& in_ctxId); // CHANGE CONTENT
	maip_err_code inf_destroy_context(MBASE_MAIP_CL_AUTH, const U64& in_ctxId); // CHANGE CONTENT
	maip_err_code inf_acquire_model(MBASE_MAIP_CL_AUTH, const mbase::string& in_model);
	maip_err_code inf_release_model(MBASE_MAIP_CL_AUTH, const mbase::string& in_model);
	maip_err_code inf_get_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models);
	maip_err_code inf_get_model_params(MBASE_MAIP_CL_AUTH, const mbase::string& in_model);
	maip_err_code inf_get_program_models(MBASE_MAIP_CL_AUTH, mbase::vector<mbase::string>& out_models);

	maip_err_code exec_set_input(MBASE_MAIP_CL_AUTH, const U64& in_ctxId, mbase::context_role in_role, const mbase::string& in_input, U32& out_msgid);
	maip_err_code exec_execute_input(MBASE_MAIP_CL_AUTH, const U64& in_ctxId, mbase::vector<U32>& in_msgid); // TODO: CHANGE CONTENT
	maip_err_code exec_next(MBASE_MAIP_CL_AUTH, std::shared_ptr<mbase::PcNetPeerClient> in_peer, const U64& in_ctxId);

	GENERIC initialize(const mbase::string& in_state_directory = gDefaultStateDirectory);
	flags host_model(InfModelTextToText* in_model);
	flags release_model(const mbase::string& in_model_name);
	#ifdef MBASE_INTERNAL_API
	GENERIC destroy_all_context(InfAcceptedClient* in_client);
	GENERIC destroy_context(InfAcceptedClient* in_client, const U64& in_ctxId);
	#endif
	static maip_err_code inf_proc_err_to_maip(InfProcessorBase::flags in_flag);
	static maip_err_code inf_exec_err_to_maip(InfProcessorBase::flags in_flag);

	GENERIC update();

private:
	accepted_client_map mAcceptedClients;
	registered_model_map mRegisteredModels;
	U64 mClientSessionIdCounter = 0;
	mbase::string mClientStateDirectory;
};

MBASE_END

#endif // !MBASE_INF_PROGRAM_H