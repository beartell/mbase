#ifndef MBASE_CLIENT_T2T_H
#define MBASE_CLIENT_T2T_H

#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_context_line.h>
#include <mbase/inference/inf_t2t_processor.h>

MBASE_BEGIN

class MBASE_API InfClientTextToText : public InfClientBase {
public:
	using chat_history_map = mbase::unordered_map<U32, context_line>;

	enum flags : U8 {
		INF_CLIENT_SUCCESS,
		INF_CLIENT_ERR_MSG_ID_MISMATCH,
		INF_CLIENT_ERR_MISSING_INPUT,
		INF_CLIENT_ERR_NOT_REGISTERED
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	InfClientTextToText();
	InfClientTextToText(const InfClientTextToText& in_rhs);
	~InfClientTextToText();
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	InfClientTextToText& operator=(const InfClientTextToText& in_rhs);
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	bool has_message(const U32& in_msg_id) const;
	flags get_message(const U32& in_msg_id, context_line& out_message);
	flags get_message_array(PTRU32 in_msg_ids, size_type in_id_count, mbase::vector<context_line>& out_messages);
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	flags add_message(CBYTEBUFFER in_data, size_type in_size, context_role in_role, U32& out_message_id);
	flags add_message(const mbase::string& in_data, context_role in_role, U32& out_message_id);
	flags add_message(const mbase::wstring& in_data, context_role in_role, U32& out_message_id);
	flags remove_messages(const mbase::vector<U32>& in_msg_ids = mbase::vector<U32>());
	GENERIC clear_chat_history();
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== INTERFACE METHODS BEGIN ===== */
	virtual GENERIC on_batch_processed(InfProcessorTextToText* out_processor, const U32& out_proc_batch_length, const bool& out_is_kv_locked) = 0;
	virtual GENERIC on_write(InfProcessorTextToText* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) = 0;
	virtual GENERIC on_finish(InfProcessorTextToText* out_processor, size_type out_total_token_size, InfProcessorTextToText::finish_state out_finish_state) = 0;
	/* ===== INTERFACE METHODS END ===== */
protected:
	chat_history_map mChatHistory;
	U32 mMessageIndexer;
};

MBASE_END

#endif // !MBASE_CLIENT_T2T_H
