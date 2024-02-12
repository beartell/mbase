#ifndef MBASE_TIMER_LOOP_H
#define MBASE_TIMER_LOOP_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <mbase/app/timers.h>
#include <mbase/app/thread_pool.h>
#include <Windows.h>

MBASE_BEGIN

// THIS WILL BE EXTENDED INTO NEW timer_loop IF THE APPLICATION HAS GUI

/*
	Public Methods:
	- register_timer
	- unregister_timer
	- register_event
	- listen_event
	- dispatch_event
	- run
	- halt

*/

class timer_loop {
public:
	enum class timer_err : U32 {
		TIMER_SUCCESS = 0,
		TIMER_ERR_LIMIT_REACHED = 1,
		TIMER_ERR_INVALID_DATA = 2
	};

	timer_loop() : timerLimit(16), timerIdCounter(0), isRunning(false) {
		LARGE_INTEGER performanceFrequency = {};
		QueryPerformanceFrequency(&performanceFrequency);
		frequency = 1000 / (F64)performanceFrequency.QuadPart; // MS ACCURACY
		deltaTime = frequency;
	}

	~timer_loop() {
		
	}

	timer_err RegisterTimer(timer_base* in_timer) {
		if(registeredTimers.size() > timerLimit)
		{
			return timer_err::TIMER_ERR_LIMIT_REACHED;
		}

		in_timer->mTimerId = ++timerIdCounter; // Timer ids will start from 1
		registeredTimers.push_back(in_timer);

		return timer_err::TIMER_SUCCESS;
	}

	timer_err RegisterTimer(timer_base* in_timer, PTRGENERIC in_usr_data) noexcept {
		if (registeredTimers.size() > timerLimit)
		{
			return timer_err::TIMER_ERR_LIMIT_REACHED;
		}

		in_timer->mTimerId = timerIdCounter++; // Timer ids will start from 1
		in_timer->suppliedData = in_usr_data;
		registeredTimers.push_back(in_timer);

		return timer_err::TIMER_SUCCESS;
	}

	GENERIC UnregisterTimer(timer_base* in_timer) {

		// find will be implemented in list
	}

	U32 GetDeltaSeconds() {
		return deltaTime;
	}

	mbase::list<timer_base*>* GetTimerList() {
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

	GENERIC Halt() noexcept {
		isRunning = false;
	}

protected:
	F64 deltaTime; // DO NOT MODIFY
	F64 frequency; // DO NOT MODIFY
	U32 timerLimit;
	U32 timerIdCounter; // DO NOT MODIFY
	bool isRunning;
	mbase::list<timer_base*> registeredTimers;
	mbase::tpool threadPool;
};

MBASE_END

#endif // !MBASE_TIMER_LOOP_H
