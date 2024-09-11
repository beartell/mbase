#ifndef MBASE_TIMERS_H
#define MBASE_TIMERS_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/list.h>
#include <mbase/framework/handler_base.h>

MBASE_BEGIN

class MBASE_API timer_base : public handler_base {
public:
	enum class flags : U8 {
		TIMER_TYPE_TIMEOUT = 2,
		TIMER_TYPE_INTERVAL = 3,
		TIMER_POLICY_IMMEDIATE = 4,
		TIMER_POLICY_ASYNC = 5,
		TIMER_STATUS_REGISTERED = 6,
		TIMER_STATUS_UNREGISTERED = 7,
		TIMER_STATUS_ABANDONED = 8
	};

	using user_data = PTRGENERIC;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE timer_base() noexcept;
	MBASE_INLINE MBASE_EXPLICIT timer_base(user_data in_data) noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_loop_id() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 get_target_time() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 get_current_time() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 get_remaining_time() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE flags get_timer_type() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE flags get_execution_policy() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE flags get_timer_status() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_registered() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE GENERIC set_target_time(U32 in_time_inms, flags in_policy = flags::TIMER_POLICY_IMMEDIATE) noexcept;
	MBASE_INLINE GENERIC set_execution_policy(flags in_policy) noexcept;
	MBASE_INLINE GENERIC reset_time() noexcept;
	virtual GENERIC on_register() { /* DEFAULT IMPL */ }
	virtual GENERIC on_unregister() { /* DEFAULT IMPL */ }
	/* ===== STATE-MODIFIER METHODS END ===== */

	friend class timer_loop;

protected:
	flags mTimerType;
	flags mPolicy;
	flags mStatus;
	U32 mLoopId;
	F64 mCurrentTime;
	F64 mTargetTime;
private:
	using timer_element = mbase::list<timer_base*>::iterator;
	timer_element mSelfIter;
};

class timeout : public timer_base {
public:
	MBASE_INLINE timeout() noexcept;
	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }

	friend class timer_loop;
};

class MBASE_API time_interval : public timer_base {
public:
	MBASE_INLINE time_interval() noexcept;

	MBASE_INLINE GENERIC set_tick_limit(U32 in_tick_limit) noexcept;
	MBASE_INLINE GENERIC reset_tick_counter() noexcept;
	MBASE_INLINE U32 get_tick_count() const noexcept;

	virtual GENERIC on_call(user_data in_data) override { /* Do nothing literally */ }
	friend class timer_loop;

private:
	U32 mTickCount;
	U32 mTickLimit;
};

MBASE_INLINE timer_base::timer_base() noexcept : handler_base(), mCurrentTime(0), mTargetTime(0), mPolicy(flags::TIMER_POLICY_IMMEDIATE), mLoopId(0), mSelfIter(nullptr), mStatus(flags::TIMER_STATUS_UNREGISTERED)
{
}

MBASE_INLINE timer_base::timer_base(user_data in_data) noexcept : mCurrentTime(0), mTargetTime(0), mPolicy(flags::TIMER_POLICY_IMMEDIATE), mLoopId(0), mSelfIter(nullptr), mStatus(flags::TIMER_STATUS_UNREGISTERED)
{
	mSuppliedData = in_data;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 timer_base::get_loop_id() const noexcept 
{
	return mLoopId;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 timer_base::get_target_time() const noexcept 
{
	return mTargetTime;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 timer_base::get_current_time() const noexcept 
{
	return mCurrentTime;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 timer_base::get_remaining_time() const noexcept 
{
	return mTargetTime - mCurrentTime;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE timer_base::flags timer_base::get_timer_type() const noexcept 
{
	return mTimerType;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE timer_base::flags timer_base::get_execution_policy() const noexcept 
{
	return mPolicy;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE timer_base::flags timer_base::get_timer_status() const noexcept
{
	return mStatus;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool timer_base::is_registered() const noexcept 
{
	return mStatus == flags::TIMER_STATUS_REGISTERED;
}

MBASE_INLINE GENERIC timer_base::set_target_time(U32 in_time_inms, flags in_policy) noexcept 
{
	mCurrentTime = 0;
	mTargetTime = in_time_inms;
	mPolicy = in_policy;
}

MBASE_INLINE GENERIC timer_base::set_execution_policy(flags in_policy) noexcept 
{
	mPolicy = in_policy;
}

MBASE_INLINE GENERIC timer_base::reset_time() noexcept 
{
	mCurrentTime = 0;
}

MBASE_INLINE timeout::timeout() noexcept : timer_base(nullptr)
{
	mTimerType = flags::TIMER_TYPE_TIMEOUT;
}

MBASE_INLINE time_interval::time_interval() noexcept : timer_base(nullptr), mTickCount(0), mTickLimit(0) {
	mTimerType = flags::TIMER_TYPE_INTERVAL;
}

MBASE_INLINE GENERIC time_interval::set_tick_limit(U32 in_tick_limit) noexcept
{
	mTickLimit = in_tick_limit;
}

MBASE_INLINE GENERIC time_interval::reset_tick_counter() noexcept
{
	mTickCount = 0;
}

MBASE_INLINE U32 time_interval::get_tick_count() const noexcept
{
	return mTickCount;
}

MBASE_END

#endif // !MBASE_TIMERS_H
