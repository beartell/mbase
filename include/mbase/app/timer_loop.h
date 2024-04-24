#ifndef MBASE_TIMER_LOOP_H
#define MBASE_TIMER_LOOP_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <mbase/app/timers.h>
#include <mbase/app/thread_pool.h>
#include <Windows.h>

#define MBASE_DEFAULT_TIMER_LIMIT 1024

MBASE_BEGIN

class timer_loop : public non_copymovable {
public:
	using timer_element_iterator = mbase::list<timer_base*>::iterator;

	enum class timer_err : U32 {
		TIMER_SUCCESS = 0,
		TIMER_ERR_LIMIT_REACHED = 1,
		TIMER_ERR_INVALID_DATA = 2,
		TIMER_WARN_TIMER_WILL_EXECUTE_IMM = 3
	};

	timer_loop() : timerLimit(MBASE_DEFAULT_TIMER_LIMIT), timerIdCounter(0), isRunning(false) {
		LARGE_INTEGER performanceFrequency = {};
		QueryPerformanceFrequency(&performanceFrequency);
		frequency = 1000 / (F64)performanceFrequency.QuadPart; // MS ACCURACY
		deltaTime = frequency;
	}

	~timer_loop() {}

	timer_err RegisterTimer(timer_base& in_timer, timer_element_iterator* out_timer) noexcept {
		if(registeredTimers.size() > timerLimit)
		{
			return timer_err::TIMER_ERR_LIMIT_REACHED;
		}

		in_timer.mTimerId = ++timerIdCounter; // Timer ids will start from 1
		timer_err terr = timer_err::TIMER_SUCCESS;

		if(in_timer.GetTargetTime() <= 0)
		{
			// WARN THE USER THAT THE TIMER WILL BE EXECUTED IMMEDIATELY
			terr = timer_err::TIMER_WARN_TIMER_WILL_EXECUTE_IMM;
		}
		in_timer.mIsActive = true;
		registeredTimers.push_back(&in_timer);
		out_timer = &registeredTimers.end();

		return terr;
	}

	timer_err RegisterTimer(timer_base& in_timer, PTRGENERIC in_usr_data, timer_element_iterator* out_timer) noexcept {
		if (registeredTimers.size() > timerLimit)
		{
			return timer_err::TIMER_ERR_LIMIT_REACHED;
		}

		in_timer.mTimerId = ++timerIdCounter; // Timer ids will start from 1
		timer_err terr = timer_err::TIMER_SUCCESS;

		if (in_timer.GetTargetTime() <= 0)
		{
			// WARN THE USER THAT THE TIMER WILL BE EXECUTED IMMEDIATELY
			terr = timer_err::TIMER_WARN_TIMER_WILL_EXECUTE_IMM;
		}
		in_timer.mIsActive = true;
		in_timer.suppliedData = in_usr_data;
		registeredTimers.push_back(&in_timer);
		out_timer = &registeredTimers.end();

		return terr;
	}

	timer_err UnregisterTimer(timer_element_iterator* out_timer) noexcept {
		MBASE_NULL_CHECK_RETURN_VAL(out_timer, timer_err::TIMER_ERR_INVALID_DATA);
		timer_base* tb = **out_timer;
		tb->mIsActive = false;
		registeredTimers.erase(*out_timer);
		return timer_err::TIMER_SUCCESS;
	}

	MBASE_ND("timer loop observation being ignored") U32 GetDeltaSeconds() const noexcept {
		return deltaTime;
	}

	MBASE_ND("timer loop observation being ignored") mbase::tpool* GetThreadPool() noexcept {
		return &threadPool;
	}

	MBASE_ND("timer loop observation being ignored") mbase::list<timer_base*>* GetTimerList() noexcept {
		return &registeredTimers;
	}

	GENERIC RunTimerOnly() noexcept {
		if(isRunning)
		{
			return;
		}
		isRunning = true;
		
		LARGE_INTEGER queryTime;
		QueryPerformanceCounter(&queryTime);

		U64 currentTime = queryTime.QuadPart;
		U64 prevTime = currentTime;

		while (isRunning)
		{	
			QueryPerformanceCounter(&queryTime);
			currentTime = queryTime.QuadPart;
			deltaTime = (currentTime - prevTime) * frequency;
			prevTime = currentTime;

			mbase::list<timer_base*>::iterator It = registeredTimers.begin();
			while(It != registeredTimers.end())
			{
				timer_base* tmpTimerBase = *It;
				tmpTimerBase->mCurrentTime += deltaTime;
				tmpTimerBase->mIsActive = true;
				if (tmpTimerBase->mCurrentTime >= tmpTimerBase->mTargetTime)
				{
					if(tmpTimerBase->GetExecutionPolicy() == mbase::timer_base::timer_exec_policy::TIMER_POLICY_ASYNC)
					{
						threadPool.ExecuteJob(tmpTimerBase);
					}
					else
					{
						tmpTimerBase->on_call(tmpTimerBase->GetUserData());
					}
					
					if (tmpTimerBase->GetTimerType() == mbase::timer_base::timer_type::TIMER_TIMEOUT)
					{
						It = registeredTimers.erase(It);
					}
					else
					{
						tmpTimerBase->ResetTime();
					}
				}
				It++;
			}
		}
	}

	// RETURNS ELAPSED TIME
	GENERIC ManualRunTimers(PTRF64 in_ms) noexcept {
		mbase::list<timer_base*>::iterator It = registeredTimers.begin();
		while (It != registeredTimers.end())
		{
			timer_base* tmpTimerBase = *It;
			tmpTimerBase->mCurrentTime += *in_ms * 1000;
			if (tmpTimerBase->mCurrentTime >= tmpTimerBase->mTargetTime)
			{
				tmpTimerBase->on_call(tmpTimerBase->GetUserData());
				if (tmpTimerBase->GetTimerType() == mbase::timer_base::timer_type::TIMER_TIMEOUT)
				{
					It = registeredTimers.erase(It);
				}
				else
				{
					tmpTimerBase->ResetTime();
				}
			}
			It++;
		}
	}

	MBASE_ND("timer loop observation being ignored") U32 GetActiveTimerCount() const noexcept {
		return registeredTimers.size();
	}

	GENERIC SetTimerLimit(U32 in_limit) noexcept {
		timerLimit = in_limit;
	}

	GENERIC Halt() noexcept {
		isRunning = false;
	}

protected:
	F64 deltaTime;
	F64 frequency;
	U32 timerLimit;
	U32 timerIdCounter;
	bool isRunning;
	mbase::list<timer_base*> registeredTimers;
	mbase::tpool threadPool;
};

MBASE_END

#endif // !MBASE_TIMER_LOOP_H
