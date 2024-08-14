#ifndef MBASE_INF_CLIENT_H
#define MBASE_INF_CLIENT_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/unordered_map.h>

MBASE_BEGIN

class MBASE_API InfClient : public mbase::non_movable {
public:
	enum class flags : U8 {
		INF_CLIENT_SUCCESS,
		INF_CLIENT_ERR_NOT_REGISTERED,
		INF_CLIENT_ERR_PROCESSING,
		INF_CLIENT_ERR_MSG_ID_MISMATCH,
		INF_CLIENT_ERR_MISSING_CHAT,
		INF_CLIENT_ERR_TOKENIZATION_FAILED,
		INF_CLIENT_ERR_TOKEN_LIMIT_EXCEEDED,
		INF_CLIENT_ERR_UNKNOWN,
		INF_CLIENT_ERR_UNREGISTERATION_IN_PROGRESS,
		INF_CLIENT_ERR_MISSING_PROCESSOR
	};

	enum class finish_state : U8 {
		INF_FINISH_STATE_SUCCESS,
		INF_FINISH_STATE_CONTINUE,
		INF_FINISH_STATE_TOKEN_LIMIT_REACHED,
		INF_FINISH_STATE_ABANDONED
	};

	enum class input_role : U8 {
		INF_ROLE_SYSTEM,
		INF_ROLE_USR,
		INF_ROLE_NONE
	};

	struct context_line {
		input_role mRole = input_role::INF_ROLE_NONE;
		mbase::string mInput = "";
		U32 mMessageIndex = 0;
	};

	using inf_proc_iter = mbase::list<InfClient*>::iterator;
	using chat_history_map = mbase::unordered_map<U32, context_line>;
	using token_vector = mbase::vector<InfProcessor::inf_token>;
	using size_type = SIZE_T;

	friend class InfProcessor;

	InfClient();
	InfClient(const InfClient& in_rhs);
	~InfClient();

	InfClient& operator=(const InfClient& in_rhs);

	bool is_processing() const;
	bool is_registered() const;
	bool is_unregistering() const;
	bool is_data_set() const;
	bool is_logic_processed() const;
	flags get_generated_token_count(size_type& out_token_count);
	flags get_context_id(U32& out_context_id);
	flags get_message_context(U32 in_msg_id, context_line& out_context_line);
	flags get_host_processor(InfProcessor*& out_processor);

	virtual GENERIC on_register() = 0;
	virtual GENERIC on_write(CBYTEBUFFER out_data, size_type out_size) = 0;
	virtual GENERIC on_finish(size_type out_total_token_size) = 0;
	virtual GENERIC on_unregister() = 0;

	flags set_input(CBYTEBUFFER in_data, size_type in_size, input_role in_role, U32& out_message_id);
	flags set_input(const mbase::string& in_data, input_role in_role, U32& out_message_id);
	flags execute_prompt(const mbase::vector<U32>& in_msg_ids);
	flags remove_messages(const mbase::vector<U32>& in_msg_ids = mbase::vector<U32>());
	GENERIC next();
	GENERIC abandon();
	GENERIC clear_chat_history(); // clears the chat map

protected:
	GENERIC _reset_client();

	InfProcessor* mfrHostProcessor;
	inf_proc_iter mfrSelfIter;
	bool mIsProcessing;
	bool mIsUnregistering;
	bool mIsLogicProcessed;
	bool mIsDataSet;
	mbase::vector<InfProcessor::inf_token> mParsedTokens;
	mbase::unordered_map<U32, context_line> mChatHistory;
	U32 mSequenceId;
	U32 mfrBatchCursor;
	U32 mMessageIndexer;
	U32 mfrMaxTokenCount;
	llama_batch mfrBatch;
	mbase::deep_char_stream mfrGeneratedToken;
	finish_state mFs;
};

MBASE_END

#endif // !MBASE_INF_CLIENT_H
