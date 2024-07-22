#ifndef MBASE_INF_CLIENT_H
#define MBASE_INF_CLIENT_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/inference/inf_processor.h>

MBASE_BEGIN

class InfClient : public mbase::non_copyable {
public:
	using inf_proc_iter = mbase::list<InfClient*>::iterator;
	using size_type = SIZE_T;

	friend class InfProcessor;

	enum class flags : U8 {
		INF_CLIENT_SUCCESS,
		INF_CLIENT_ERR_NOT_REGISTERED,
		INF_CLIENT_ERR_PROCESSING,
		INF_CLIENT_ERR_UNKNOWN
	};

	InfClient();
	~InfClient();

	bool is_processing() const;
	bool is_registered() const;
	bool is_data_set() const;
	bool is_logic_processed() const;
	flags get_generated_token_count(size_type& out_token_count);

	virtual GENERIC on_register();
	virtual GENERIC on_write(CBYTEBUFFER out_data, size_type out_size);
	virtual GENERIC on_finish(size_type out_total_token_size);
	virtual GENERIC on_abandon();
	virtual GENERIC on_unregister();

	flags set_input(CBYTEBUFFER in_data, size_type in_size);
	GENERIC abandon();

private:
	InfProcessor* mHostProcessor;
	inf_proc_iter mSelfIter;
	bool mIsProcessing;
	bool mIsLogicProcessed;
	bool mIsDataSet;
	mbase::vector<InfProcessor::inf_token> mParsedTokens;
	I32 mSequenceId; // batch seq id
	U32 mBatchCursor;
	size_type mGeneratedTokenCount;
	llama_batch mBatch;
	mbase::deep_char_stream mGeneratedToken;
};

MBASE_END

#endif // !MBASE_INF_CLIENT_H
