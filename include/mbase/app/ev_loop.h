#ifndef MBASE_EV_LOOP_H
#define MBASE_EV_LOOP_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <mbase/app/timers.h>
#include <Windows.h>

MBASE_BEGIN

// THIS WILL BE EXTENDED INTO NEW EV_LOOP IF THE APPLICATION HAS GUI

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

class ev_loop {
public:
	enum class timer_err : U32 {
		TIMER_SUCCESS = 0,
		TIMER_ERR_LIMIT_REACHED = 1,
		TIMER_ERR_INVALID_DATA = 2
	};

	ev_loop() : timerLimit(0), timerIdCounter(0), isRunning(false) {
		LARGE_INTEGER performanceFrequency = {};
		QueryPerformanceFrequency(&performanceFrequency);
		frequency = performanceFrequency.QuadPart;
	}

	timer_err RegisterTimer(timer_base* in_timer) {
		if(timerIdCounter > timerLimit)
		{
			return timer_err::TIMER_ERR_LIMIT_REACHED;
		}

		in_timer->mTimerId = ++timerIdCounter; // Timer ids will start from 1
		registeredTimers.push_back(in_timer);

		return timer_err::TIMER_SUCCESS;
	}

	timer_err RegisterTimer(timer_base* in_timer, PTRGENERIC in_usr_data) noexcept {
		if (timerIdCounter > timerLimit)
		{
			return timer_err::TIMER_ERR_LIMIT_REACHED;
		}

		in_timer->mTimerId = timerIdCounter++; // Timer ids will start from 1
		in_timer->mUsrData = in_usr_data;
		registeredTimers.push_back(in_timer);

		return timer_err::TIMER_SUCCESS;
	}

	GENERIC UnregisterTimer(timer_base* in_timer) {

		// find will be implemented in list
	}

	U32 GetTimerResolutionInMs() {
		LARGE_INTEGER queryTime;
		QueryPerformanceCounter(&queryTime);

		U32 startTime = queryTime.QuadPart;
		Sleep(15);
		QueryPerformanceCounter(&queryTime);
		U32 endTime = queryTime.QuadPart;
		U32 elapsedTime = ((endTime - startTime) * 1000) / frequency;

		return elapsedTime;
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
		U32 startTime = 0;
		U32 endTime = 0;
		U32 elapsedTime = GetTimerResolutionInMs();
		
		while (isRunning)
		{
			mbase::list<timer_base*>::iterator It = registeredTimers.begin();
			QueryPerformanceCounter(&queryTime);
			Sleep(15);
			startTime = queryTime.QuadPart;
			while(It != registeredTimers.end())
			{
				timer_base* tmpTimerBase  = *It;
				tmpTimerBase->mCurrentTime += elapsedTime;
				
				if(tmpTimerBase->GetRemainingTime() <= 0)
				{
					tmpTimerBase->on_time(tmpTimerBase->GetUserData());
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
			QueryPerformanceCounter(&queryTime);
			endTime = queryTime.QuadPart;
			elapsedTime = ((endTime - startTime) * 1000) / frequency;
		}
	}

	// RETURNS ELAPSED TIME
	U32 ManualRunTimers(U32 in_ms) noexcept {
		LARGE_INTEGER queryTime;
		QueryPerformanceCounter(&queryTime);
		U32 startTime = queryTime.QuadPart;
		U32 endTime = 0;
		U32 elapsedTime = in_ms;

		mbase::list<timer_base*>::iterator It = registeredTimers.begin();
		while (It != registeredTimers.end())
		{
			timer_base* tmpTimerBase = *It;
			tmpTimerBase->mCurrentTime += elapsedTime;
			if (tmpTimerBase->GetRemainingTime() <= 0)
			{
				tmpTimerBase->on_time(tmpTimerBase->GetUserData());
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
		
		QueryPerformanceCounter(&queryTime);
		endTime = queryTime.QuadPart;

		elapsedTime += ((endTime - startTime) * 1000) / frequency;

		return elapsedTime;
	}

	GENERIC Halt() noexcept {
		isRunning = false;
	}

protected:
	U32 frequency; // DO NOT MODIFY
	U32 timerLimit;
	U32 timerIdCounter; // DO NOT MODIFY
	bool isRunning;
	mbase::list<timer_base*> registeredTimers;
};

MBASE_END

#endif // !MBASE_EV_LOOP_H
