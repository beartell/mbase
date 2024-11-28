#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_client.h>

MBASE_BEGIN

InfProcessorBase::InfProcessorBase() :
	mAssignedClient(NULL),
	mTargetModel_md_model(NULL),
	mIsRunning(false),
	mIsRegistered(false),
	mModelCategory(inf_model_category::UNDEFINED),
	mContextLength(0),
	mInactivityThreshold(0),
	mTargetWatcher(NULL)
{
}

InfProcessorBase::~InfProcessorBase()
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

InfClientBase* InfProcessorBase::get_assigned_client()
{
	return mAssignedClient;
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

inf_model_category InfProcessorBase::get_model_category()
{
	return mModelCategory;
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

InfProcessorBase::flags InfProcessorBase::set_inference_client(InfClientBase* in_client)
{
	release_inference_client();

	mAssignedClient = in_client;
	mAssignedClient->on_register(this);

	return flags::INF_PROC_SUCCESS;
}

GENERIC InfProcessorBase::release_inference_client()
{
	if(mAssignedClient)
	{
		mAssignedClient->on_unregister(this);
		mAssignedClient = NULL;
	}
}

GENERIC InfProcessorBase::release_inference_client_stacked()
{
	mAssignedClient = NULL;
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
