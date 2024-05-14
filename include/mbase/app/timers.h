#ifndef MBASE_TIMERS_H
#define MBASE_TIMERS_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/app/handler_base.h>

MBASE_BEGIN

class timer_base : public handler_base {
public:
	enum class timer_flag : U8 {
		TIMER_STATE_FINISHED = 0,
		TIMER_STATE_INACTIVE = 1,
		TIMER_TYPE_TIMEOUT = 2,
		TIMER_TYPE_INTERVAL = 3,
		TIMER_POLICY_IMMEDIATE = 4,
		TIMER_POLICY_ASYNC = 5
	};

	using user_data = PTRGENERIC;

	timer_base() noexcept :  handler_base(), mCurrentTime(0), mTargetTime(0), mPolicy(timer_flag::TIMER_POLICY_IMMEDIATE), mStatus(timer_flag::TIMER_STATE_INACTIVE), mIsRegistered(false), mTimerId(-1), teSelf(nullptr) {}

	MBASE_EXPLICIT timer_base(user_data in_data) noexcept : mCurrentTime(0), mTargetTime(0), mPolicy(timer_flag::TIMER_POLICY_IMMEDIATE), mStatus(timer_flag::TIMER_STATE_INACTIVE), mIsRegistered(false), mTimerId(-1), teSelf(nullptr) {
		suppliedData = in_data;
	}

	MBASE_ND("timer observation ignored") I32 GetTimerId() const noexcept {
		return mTimerId;
	}

	MBASE_ND("timer observation ignored") I32 GetTargetTime() const noexcept {
		return mTargetTime;
	}

	MBASE_ND("timer observation ignored") I32 GetCurrTime() const noexcept {
		return mCurrentTime;
	}

	MBASE_ND("timer observation ignored") I32 GetRemainingTime() noexcept {
		return mTargetTime - mCurrentTime;
	}

	MBASE_ND("timer observation ignored") timer_flag GetTimerStatus() const noexcept 
	{
		return mStatus;
	}

	MBASE_ND("timer observation ignored") timer_flag GetTimerType() const noexcept {
		return tt;
	}

	MBASE_ND("timer observation ignored") timer_flag GetExecutionPolicy() const noexcept {
		return mPolicy;
	}

	MBASE_ND("timer observation ignored") bool IsRegistered() const noexcept {
		return mIsRegistered;
	}

	GENERIC SetTargetTime(U32 in_time_inms, timer_flag in_policy = timer_flag::TIMER_POLICY_IMMEDIATE) noexcept {
		mCurrentTime = 0;
		mTargetTime = in_time_inms;
		mPolicy = in_policy;
	}

	GENERIC SetExecutionPolicy(timer_flag in_policy) noexcept {
		mPolicy = in_policy;
	}

	GENERIC ResetTime() noexcept {
		mCurrentTime = 0;
	}

	friend class timer_loop;

	virtual GENERIC on_call(user_data in_data) {/* do nothing */ };

protected:
	bool mIsRegistered;
	timer_flag mStatus;
	timer_flag tt;
	timer_flag mPolicy;
	I32 mTimerId;
	F64 mCurrentTime;
	F64 mTargetTime;
private:
	using timer_element = mbase::list<timer_base*>::iterator;
	timer_element teSelf;
};

class timeout : public timer_base {
public:
	timeout() noexcept : timer_base(nullptr) {
		tt = timer_flag::TIMER_TYPE_TIMEOUT;
	}

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }
};

class time_interval : public timer_base {
public:
	time_interval() noexcept : timer_base(nullptr) {
		tt = timer_flag::TIMER_TYPE_INTERVAL;
	}

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }
};

MBASE_END

#endif // !MBASE_TIMERS_H
