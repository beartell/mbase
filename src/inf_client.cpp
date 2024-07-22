#include <mbase/inference/inf_client.h>

MBASE_BEGIN

#define MBASE_INF_CLIENT_USUAL_CHECK() \
if(!is_registered())\
{\
	return flags::INF_CLIENT_ERR_NOT_REGISTERED;\
}\
if(is_processing())\
{\
	return flags::INF_CLIENT_ERR_PROCESSING;\
}

InfClient::InfClient() :
	mHostProcessor(NULL),
	mSelfIter(NULL),
	mIsProcessing(false),
	mIsLogicProcessed(false),
	mIsDataSet(false),
	mSequenceId(0)
{

}

InfClient::~InfClient()
{
	// release self from the processor
}

bool InfClient::is_processing() const
{
	return mIsProcessing;
}

bool InfClient::is_registered() const
{
	return (mHostProcessor != NULL);
}

bool InfClient::is_data_set() const
{
	return mIsDataSet;
}

bool InfClient::is_logic_processed() const
{
	return mIsLogicProcessed;
}

InfClient::flags InfClient::get_generated_token_count(size_type& out_token_count)
{
	return flags::INF_CLIENT_SUCCESS;
}

GENERIC InfClient::on_register()
{
}

GENERIC InfClient::on_write(CBYTEBUFFER out_data, size_type out_size)
{
}

GENERIC InfClient::on_finish(size_type out_total_token_size)
{
}

GENERIC InfClient::on_abandon()
{
}

GENERIC InfClient::on_unregister()
{
}

InfClient::flags InfClient::set_input(CBYTEBUFFER in_data, size_type in_size)
{
	MBASE_INF_CLIENT_USUAL_CHECK();

	if(mHostProcessor->tokenize_input(in_data, in_size, mParsedTokens) == InfProcessor::flags::INF_PROC_SUCCESS)
	{
		mIsDataSet = true;
		return flags::INF_CLIENT_SUCCESS;
	}
	
	mIsDataSet = false;
	return flags::INF_CLIENT_ERR_UNKNOWN;
}

GENERIC InfClient::abandon()
{
	mIsDataSet = false;
}

MBASE_END