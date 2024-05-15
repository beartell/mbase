#ifndef MBASE_TIMER_LOOP_H
#define MBASE_TIMER_LOOP_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <mbase/index_assigner.h>
#include <mbase/app/timers.h>
#include <mbase/app/thread_pool.h>
#include <Windows.h>

#define MBASE_DEFAULT_TIMER_LIMIT 2048

MBASE_BEGIN

class timer_loop : public non_copymovable {
public:
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

	timer_err RegisterTimer(timer_base& in_timer) noexcept {
		if(registeredTimers.size() > timerLimit)
		{
			return timer_err::TIMER_ERR_LIMIT_REACHED;
		}

		in_timer.mTimerId = ++timerIdCounter;
		timer_err terr = timer_err::TIMER_SUCCESS;

		if(in_timer.GetTargetTime() <= 0)
		{
			terr = timer_err::TIMER_WARN_TIMER_WILL_EXECUTE_IMM;
		}

		in_timer.mIsRegistered = true;
		registeredTimers.push_back(&in_timer);
		in_timer.teSelf = registeredTimers.insert(registeredTimers.cend(), &in_timer); // WE WILL FIX IT LATER

		return terr;
	}

	timer_err RegisterTimer(timer_base& in_timer, PTRGENERIC in_usr_data) noexcept {
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

		in_timer.mIsRegistered = true;
		in_timer.suppliedData = in_usr_data;
		//registeredTimers.push_back(&in_timer);
		in_timer.teSelf = registeredTimers.insert(registeredTimers.cend(), &in_timer); // WE WILL FIX IT LATER

		return terr;
	}

	timer_err UnregisterTimer(timer_base& in_timer) noexcept {
		MBASE_NULL_CHECK_RETURN_VAL(in_timer.teSelf.get(), timer_err::TIMER_ERR_INVALID_DATA);
		timer_base* tb = *in_timer.teSelf;
		if(tb->mIsRegistered)
		{
			registeredTimers.erase(in_timer.teSelf);
			tb->mIsRegistered = false;
		}
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

	GENERIC RunTimerLoop() noexcept {
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
				++It;
				if (tmpTimerBase->mCurrentTime >= tmpTimerBase->mTargetTime)
				{
					if(tmpTimerBase->GetExecutionPolicy() == mbase::timer_base::timer_flag::TIMER_POLICY_ASYNC)
					{
						threadPool.ExecuteJob(tmpTimerBase);
					}
					else
					{
						tmpTimerBase->on_call(tmpTimerBase->GetUserData());
					}

					if (!tmpTimerBase->mIsRegistered)
					{
						// which means that the handler in on_call method unregistered itself
						continue;
					}

					if (tmpTimerBase->GetTimerType() == mbase::timer_base::timer_flag::TIMER_TYPE_TIMEOUT)
					{
						tmpTimerBase->mIsRegistered = false;
						tmpTimerBase->mStatus = mbase::timer_base::timer_flag::TIMER_STATE_FINISHED;
						It = registeredTimers.erase(tmpTimerBase->teSelf);
					}
					else
					{
						tmpTimerBase->ResetTime();
					}
				}
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
				if (tmpTimerBase->GetTimerType() == mbase::timer_base::timer_flag::TIMER_TYPE_TIMEOUT)
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
