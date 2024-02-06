#ifndef MBASE_TIMERS_H
#define MBASE_TIMERS_H

#include <mbase/common.h>
#include <mbase/behaviors.h>

MBASE_BEGIN

class timer_base {
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

	timer_base() noexcept : mUsrData(nullptr), mCurrentTime(0), mTargetTime(0), mPolicy(timer_exec_policy::TIMER_POLICY_IMMEDIATE) {}

	MBASE_EXPLICIT timer_base(user_data in_data) noexcept : mCurrentTime(0), mTargetTime(0), mPolicy(timer_exec_policy::TIMER_POLICY_IMMEDIATE) {
		mUsrData = in_data;
	}

	USED_RETURN U32 GetTimerId() const noexcept {
		return mTimerId;
	}

	USED_RETURN I32 GetTargetTime() const noexcept {
		return mTargetTime;
	}

	USED_RETURN I32 GetCurrTime() const noexcept {
		return mCurrentTime;
	}

	USED_RETURN I32 GetRemainingTime() noexcept {
		return mTargetTime - mCurrentTime;
	}

	USED_RETURN user_data GetUserData() const noexcept {
		return mUsrData;
	}

	USED_RETURN timer_type GetTimerType() const noexcept {
		return tt;
	}

	USED_RETURN timer_exec_policy GetExecutionPolicy() const noexcept {
		return mPolicy;
	}

	USED_RETURN bool IsActive() const noexcept {
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

	virtual GENERIC on_call(user_data in_data) = 0;

protected:
	bool mIsActive;
	timer_type tt;
	U32 mTimerId;
	F64 mCurrentTime;
	F64 mTargetTime;
	user_data mUsrData;
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
