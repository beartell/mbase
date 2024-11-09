#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_sampling.h>
#include <common/common.h>

MBASE_BEGIN

#define MBASE_INF_PROC_RETURN_UNREGISTERED \
if(this->signal_state_initializing())\
{\
	return flags::INF_PROC_INFO_INITIALIZING;\
}\
if(!this->is_registered())\
{\
	return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;\
}\
if (this->signal_state_destroying())\
{\
	return flags::INF_PROC_INFO_DESTROYING;\
}

#define MBASE_INF_PROC_RETURN_HALTED \
if(!this->is_registered())\
{\
	return flags::INF_PROC_ERR_UNREGISTERED_PROCESSOR;\
}\
if(!this->is_running())\
{\
	return flags::INF_PROC_ERR_HALTED;\
}

static U32 gInfProcMaxTokenLength = 128;

InfProcessorBase::InfProcessorBase() :
	mTargetModel_md_model(NULL),
	mIsRunning(false),
	mIsRegistered(false),
	mContextLength(0),
	mInactivityThreshold(0)
{
}

bool InfProcessorBase::is_registered() const
{
	return mIsRegistered;
}

bool InfProcessorBase::is_running()
{
	return mIsRunning;
}

bool InfProcessorBase::signal_state_initializing() const
{
	return mInitializeSignal.get_signal_state();
}

bool InfProcessorBase::signal_state_destroying() const
{
	return mDestroySignal.get_signal_state();
}

bool InfProcessorBase::signal_initializing() const
{
	return mInitializeSignal.get_signal();
}

bool InfProcessorBase::signal_destroying() const
{
	return mDestroySignal.get_signal();
}

U32 InfProcessorBase::get_context_size()
{
	return mContextLength;
}

InfModelBase* InfProcessorBase::get_processed_model()
{
	return mTargetModel_md_model;
}

U32 InfProcessorBase::get_inactivity_threshold()
{
	return mInactivityThreshold;
}

processor_signal& InfProcessorBase::get_initialize_signal()
{
	return mInitializeSignal;
}

processor_signal& InfProcessorBase::get_destroy_signal()
{
	return mDestroySignal;
}

const mbase::string& InfProcessorBase::get_context_identifier()
{
	return mContextIdentifier;
}

InfProcessorBase::processor_type InfProcessorBase::get_processor_type()
{
	return mProcessorType;
}

GENERIC InfProcessorBase::acquire_object_watcher(mbase::inf_processor_watcher<InfProcessorBase>* in_watcher)
{
	if(in_watcher)
	{
		mTargetWatcher = in_watcher;
		mTargetWatcher->mSubject = this;
		mTargetWatcher->mContextLength = mContextLength;
	}
}

GENERIC InfProcessorBase::release_object_watcher()
{
	if(mTargetWatcher)
	{
		mTargetWatcher->mSubject = NULL;
	}
}

GENERIC InfProcessorBase::halt()
{
	mIsRunning = false;
}

GENERIC InfProcessorBase::resume()
{
	mIsRunning = true;
}

GENERIC InfProcessorBase::reset_base_signals()
{
	mInitializeSignal.reset_signal_with_state();
	mDestroySignal.reset_signal_with_state();
}

InfProcessorBase::flags InfProcessorBase::destroy()
{
	return flags::INF_PROC_INFO_NOT_IMPLEMENTED;
}

InfProcessorBase::flags InfProcessorBase::destroy_sync()
{
	return flags::INF_PROC_INFO_NOT_IMPLEMENTED;
}

MBASE_END
