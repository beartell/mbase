#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_processor.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <iostream>

MBASE_BEGIN

#define MBASE_INF_MODEL_RETURN_UNINITIALIZED \
if(this->signal_state_initializing())\
{\
	return flags::INF_MODEL_INFO_INITIALIZING_MODEL;\
}\
if(!this->is_initialized())\
{\
	return flags::INF_MODEL_ERR_NOT_INITIALIZED;\
}\
if(this->signal_state_destroying())\
{\
	return flags::INF_MODEL_INFO_DESTROYING_MODEL;\
}

InfModelBase::InfModelBase() noexcept:
	mModelCategory(inf_model_category::UNDEFINED),
	mIsInitialized(false),
	mInitFailCode(init_fail_code::UNDEFINED),
	mIsInitFailed(false)
{
}

InfModelBase::~InfModelBase() noexcept
{

}

typename InfModelBase::iterator InfModelBase::begin() noexcept
{
	return mRegisteredProcessors.begin();
}

typename InfModelBase::iterator InfModelBase::end() noexcept
{
	return mRegisteredProcessors.end();
}

typename InfModelBase::const_iterator InfModelBase::begin() const noexcept
{
	return mRegisteredProcessors.begin();
}

typename InfModelBase::const_iterator InfModelBase::end() const noexcept
{
	return mRegisteredProcessors.end();
}

typename InfModelBase::const_iterator InfModelBase::cbegin() const noexcept
{
	return mRegisteredProcessors.cbegin();
}

typename InfModelBase::const_iterator InfModelBase::cend() const noexcept
{
	return mRegisteredProcessors.cend();
}

typename InfModelBase::reverse_iterator InfModelBase::rbegin() noexcept
{
	return mRegisteredProcessors.rbegin();
}

typename InfModelBase::reverse_iterator InfModelBase::rend() noexcept
{
	return mRegisteredProcessors.rend();
}

typename InfModelBase::const_reverse_iterator InfModelBase::crbegin() const noexcept
{
	return mRegisteredProcessors.crbegin();
}

typename InfModelBase::const_reverse_iterator InfModelBase::crend() const noexcept
{
	return mRegisteredProcessors.crend();
}

typename InfModelBase::context_processor_list& InfModelBase::get_registered_processors()
{
	return mRegisteredProcessors;
}

inf_model_category InfModelBase::get_model_category() const
{
	return mModelCategory;
}

bool InfModelBase::is_initialize_failed() const
{
	return mIsInitFailed;
}

bool InfModelBase::is_initialized() const
{
	return mIsInitialized;
}

bool InfModelBase::signal_state_initializing() const
{
	return mInitializeSignal.get_signal_state();
}

bool InfModelBase::signal_state_destroying() const
{
	return mDestroySignal.get_signal_state();
}

bool InfModelBase::signal_initializing() const
{
	return mInitializeSignal.get_signal();
}

bool InfModelBase::signal_destroying() const
{
	return mDestroySignal.get_signal();
}

GENERIC InfModelBase::reset_base_signals()
{
	mInitializeSignal.reset_signal_with_state();
	mDestroySignal.reset_signal_with_state();
}

MBASE_END