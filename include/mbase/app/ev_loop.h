#ifndef MBASE_EV_LOOP_H
#define MBASE_EV_LOOP_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <mbase/app/timers.h>

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
	ev_loop() : mTimerLimit(0), mTimerIdCounter(0) {
		LARGE_INTEGER performanceFrequency = {};
		QueryPerformanceFrequency(&performanceFrequency);
		mFrequency = performanceFrequency.QuadPart;
	}

	GENERIC register_timer(timer_base* in_timer) {
		in_timer->mTimerId = mTimerIdCounter++;
		mRegisteredTimers.push_back(in_timer);
	}

	GENERIC unregister_timer(timer_base* in_timer) {
		// find will be implemented in list
	}

	GENERIC run() {

		LARGE_INTEGER queryTime;
		QueryPerformanceCounter(&queryTime);

		U32 startTime = queryTime.QuadPart;
		Sleep(1);
		QueryPerformanceCounter(&queryTime);
		U32 endTime = queryTime.QuadPart;
		U32 elapsedTime = ((endTime - startTime) * 1000) / mFrequency;

		while (1)
		{
			mbase::list<timer_base*>::iterator It = mRegisteredTimers.begin();
			QueryPerformanceCounter(&queryTime);
			Sleep(1);
			startTime = queryTime.QuadPart;
			while(It != mRegisteredTimers.end())
			{
				timer_base* tmpTimerBase  = *It;
				tmpTimerBase->mCurrentTime += elapsedTime;
				if(tmpTimerBase->get_remaining_time() <= 0)
				{
					tmpTimerBase->on_time(tmpTimerBase->get_user_data());
					if (tmpTimerBase->get_timer_type() == mbase::timer_base::timer_type::TIMER_TIMEOUT)
					{
						mbase::list<timer_base*>::iterator removedIt = It;
						It++;
						mRegisteredTimers.erase(removedIt);
					}
					else
					{
						tmpTimerBase->reset_time();
						It++;
					}
				}
				else
				{
					It++;
				}
				
			}
			QueryPerformanceCounter(&queryTime);
			endTime = queryTime.QuadPart;
			elapsedTime = ((endTime - startTime) * 1000) / mFrequency;
		}
	}

protected:
	U32 mFrequency;
	U32 mTimerLimit;
	U32 mTimerIdCounter;
	mbase::list<timer_base*> mRegisteredTimers;
};

MBASE_END

#endif // !MBASE_EV_LOOP_H
