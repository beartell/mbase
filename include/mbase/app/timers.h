#ifndef MBASE_TIMERS_H
#define MBASE_TIMERS_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/app/handler_base.h>

MBASE_BEGIN

class timer_base : public handler_base {
public:
	enum class flags : U8 {
		TIMER_TYPE_TIMEOUT = 2,
		TIMER_TYPE_INTERVAL = 3,
		TIMER_POLICY_IMMEDIATE = 4,
		TIMER_POLICY_ASYNC = 5
	};

	using user_data = PTRGENERIC;

	timer_base() noexcept : 
		handler_base(), 
		mCurrentTime(0), 
		mTargetTime(0), 
		mPolicy(flags::TIMER_POLICY_IMMEDIATE),
		mIsRegistered(false), 
		mLoopId(-1), 
		mSelfIter(nullptr)
	{
	}

	MBASE_EXPLICIT timer_base(user_data in_data) noexcept : mCurrentTime(0), mTargetTime(0), mPolicy(flags::TIMER_POLICY_IMMEDIATE), mIsRegistered(false), mLoopId(-1), mSelfIter(nullptr) {
		mSuppliedData = in_data;
	}

	MBASE_ND("timer observation ignored") I32 get_loop_id() const noexcept {
		return mLoopId;
	}

	MBASE_ND("timer observation ignored") I32 get_target_time() const noexcept {
		return mTargetTime;
	}

	MBASE_ND("timer observation ignored") I32 get_current_time() const noexcept {
		return mCurrentTime;
	}

	MBASE_ND("timer observation ignored") I32 get_remaining_time() const noexcept {
		return mTargetTime - mCurrentTime;
	}

	MBASE_ND("timer observation ignored") flags get_timer_type() const noexcept {
		return mTimerType;
	}

	MBASE_ND("timer observation ignored") flags get_execution_policy() const noexcept {
		return mPolicy;
	}

	MBASE_ND("timer observation ignored") bool is_registered() const noexcept {
		return mIsRegistered;
	}

	GENERIC set_target_time(U32 in_time_inms, flags in_policy = flags::TIMER_POLICY_IMMEDIATE) noexcept {
		mCurrentTime = 0;
		mTargetTime = in_time_inms;
		mPolicy = in_policy;
	}

	GENERIC set_execution_policy(flags in_policy) noexcept {
		mPolicy = in_policy;
	}

	GENERIC reset_time() noexcept {
		mCurrentTime = 0;
	}

	friend class timer_loop;

	virtual GENERIC on_register() { /* DEFAULT IMPL */ }
	virtual GENERIC on_unregister() { /* DEFAULT IMPL */ }

protected:
	bool mIsRegistered;
	flags mTimerType;
	flags mPolicy;
	I32 mLoopId;
	F64 mCurrentTime;
	F64 mTargetTime;
private:
	using timer_element = mbase::list<timer_base*>::iterator;
	timer_element mSelfIter;
};

class timeout : public timer_base {
public:
	timeout() noexcept : timer_base(nullptr) {
		mTimerType = flags::TIMER_TYPE_TIMEOUT;
	}

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }

	friend class timer_loop;

};

class time_interval : public timer_base {
public:
	time_interval() noexcept : timer_base(nullptr), mTickCount(0), mTickLimit(0) {
		mTimerType = flags::TIMER_TYPE_INTERVAL;
	}

	GENERIC set_tick_limit(U32 in_tick_limit) noexcept
	{
		mTickLimit = in_tick_limit;
	}

	GENERIC reset_tick_counter() noexcept
	{
		mTickCount = 0;
	}

	U32 get_tick_count() const noexcept
	{
		return mTickCount;
	}

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }

	friend class timer_loop;

private:
	U32 mTickCount;
	U32 mTickLimit;
};

MBASE_END

#endif // !MBASE_TIMERS_H
