#ifndef MBASE_TIMERS_H
#define MBASE_TIMERS_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/app/handler_base.h>

MBASE_BEGIN

class timer_base : public handler_base {
public:
	enum class timer_type : U8 {
		TIMER_TIMEOUT = 0,
		TIMER_INTERVAL = 1
	};

	enum class timer_exec_policy : U8 {
		TIMER_POLICY_IMMEDIATE = 0,
		TIMER_POLICY_ASYNC = 1
	};

	using user_data = PTRGENERIC;

	timer_base() noexcept :  handler_base(), mCurrentTime(0), mTargetTime(0), mPolicy(timer_exec_policy::TIMER_POLICY_IMMEDIATE) {}

	MBASE_EXPLICIT timer_base(user_data in_data) noexcept : mCurrentTime(0), mTargetTime(0), mPolicy(timer_exec_policy::TIMER_POLICY_IMMEDIATE) {
		suppliedData = in_data;
	}

	MBASE_ND("timer observation ignored") U32 GetTimerId() const noexcept {
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

	MBASE_ND("timer observation ignored") timer_type GetTimerType() const noexcept {
		return tt;
	}

	MBASE_ND("timer observation ignored") timer_exec_policy GetExecutionPolicy() const noexcept {
		return mPolicy;
	}

	MBASE_ND("timer observation ignored") bool IsActive() const noexcept {
		return mIsActive;
	}

	GENERIC SetTargetTime(U32 in_time_inms, timer_exec_policy in_policy = timer_exec_policy::TIMER_POLICY_IMMEDIATE) noexcept {
		mCurrentTime = 0;
		mTargetTime = in_time_inms;
		mPolicy = in_policy;
	}

	GENERIC SetExecutionPolicy(timer_exec_policy in_policy) noexcept {
		mPolicy = in_policy;
	}

	GENERIC ResetTime() noexcept {
		mCurrentTime = 0;
	}

	friend class timer_loop;

	virtual GENERIC on_call(user_data in_data) {/* do nothing */ };

protected:
	bool mIsActive;
	timer_type tt;
	U32 mTimerId;
	F64 mCurrentTime;
	F64 mTargetTime;
	timer_exec_policy mPolicy;
};

class timeout : public timer_base {
public:
	timeout() noexcept : timer_base(nullptr) {
		tt = timer_type::TIMER_TIMEOUT;
	}

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }
};

class time_interval : public timer_base {
public:
	time_interval() noexcept : timer_base(nullptr) {
		tt = timer_type::TIMER_INTERVAL;
	}

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }
};

MBASE_END

#endif // !MBASE_TIMERS_H
