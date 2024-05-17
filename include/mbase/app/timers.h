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
		loopId(-1), 
		teSelf(nullptr)
	{
	}

	MBASE_EXPLICIT timer_base(user_data in_data) noexcept : mCurrentTime(0), mTargetTime(0), mPolicy(flags::TIMER_POLICY_IMMEDIATE), mIsRegistered(false), loopId(-1), teSelf(nullptr) {
		suppliedData = in_data;
	}

	MBASE_ND("timer observation ignored") I32 GetLoopId() const noexcept {
		return loopId;
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

	MBASE_ND("timer observation ignored") flags GetTimerType() const noexcept {
		return tt;
	}

	MBASE_ND("timer observation ignored") flags GetExecutionPolicy() const noexcept {
		return mPolicy;
	}

	MBASE_ND("timer observation ignored") bool IsRegistered() const noexcept {
		return mIsRegistered;
	}

	GENERIC SetTargetTime(U32 in_time_inms, flags in_policy = flags::TIMER_POLICY_IMMEDIATE) noexcept {
		mCurrentTime = 0;
		mTargetTime = in_time_inms;
		mPolicy = in_policy;
	}

	GENERIC SetExecutionPolicy(flags in_policy) noexcept {
		mPolicy = in_policy;
	}

	GENERIC ResetTime() noexcept {
		mCurrentTime = 0;
	}

	friend class timer_loop;

	virtual GENERIC on_register() { /* DEFAULT IMPL */ }
	virtual GENERIC on_unregister() { /* DEFAULT IMPL */ }

protected:
	bool mIsRegistered;
	flags tt;
	flags mPolicy;
	I32 loopId;
	F64 mCurrentTime;
	F64 mTargetTime;
private:
	using timer_element = mbase::list<timer_base*>::iterator;
	timer_element teSelf;
};

class timeout : public timer_base {
public:
	timeout() noexcept : timer_base(nullptr) {
		tt = flags::TIMER_TYPE_TIMEOUT;
	}

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }

	friend class timer_loop;

};

class time_interval : public timer_base {
public:
	time_interval() noexcept : timer_base(nullptr), tickCount(0), tickLimit(0) {
		tt = flags::TIMER_TYPE_INTERVAL;
	}

	GENERIC SetTickLimit(U32 in_tick_limit)
	{
		tickLimit = in_tick_limit;
	}

	GENERIC ResetTickCounter() 
	{
		tickCount = 0;
	}

	U32 GetTickCount()
	{
		return tickCount;
	}

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }

	friend class timer_loop;

private:
	U32 tickCount;
	U32 tickLimit;
};

MBASE_END

#endif // !MBASE_TIMERS_H
