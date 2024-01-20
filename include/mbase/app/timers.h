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

	using user_data = PTRGENERIC;

	timer_base() noexcept : mUsrData(nullptr), mCurrentTime(0), mTargetTime(0) {}

	MBASE_EXPLICIT timer_base(user_data in_data) noexcept : mCurrentTime(0), mTargetTime(0) {
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
		return GetTargetTime() - mCurrentTime;
	}

	USED_RETURN user_data GetUserData() const noexcept {
		return mUsrData;
	}

	USED_RETURN timer_type GetTimerType() const noexcept {
		return tt;
	}

	USED_RETURN bool IsActive() const noexcept {
		return mIsActive;
	}

	GENERIC SetTargetTime(U32 in_time_inms) noexcept {
		mCurrentTime = 0;
		mTargetTime = in_time_inms;
	}

	GENERIC ResetTime() noexcept {
		mCurrentTime = 0;
	}

	friend class ev_loop;

	virtual GENERIC on_time(user_data in_data) = 0;

protected:
	bool mIsActive;
	timer_type tt;
	U32 mTimerId;
	I32 mCurrentTime;
	I32 mTargetTime;
	user_data mUsrData;
};

class timeout : public timer_base {
public:
	timeout() noexcept : timer_base(nullptr) {
		tt = timer_type::TIMER_TIMEOUT;
	}

	virtual GENERIC on_time(user_data in_data) override { /* Do nothing literally */ }
};

class time_interval : public timer_base {
public:
	time_interval() noexcept : timer_base(nullptr) {
		tt = timer_type::TIMER_INTERVAL;
	}

	virtual GENERIC on_time(user_data in_data) override { /* Do nothing literally */ }
};

MBASE_END

#endif // !MBASE_TIMERS_H
