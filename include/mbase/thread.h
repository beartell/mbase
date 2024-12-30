#ifndef MBASE_THREAD_H
#define MBASE_THREAD_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <tuple>

#ifdef MBASE_PLATFORM_WINDOWS
#include <Windows.h>
#include <timeapi.h>

#pragma comment(lib, "Winmm.lib")

#endif

#ifdef MBASE_PLATFORM_UNIX
#include <pthread.h>
#endif

MBASE_STD_BEGIN

#ifdef MBASE_PLATFORM_WINDOWS

struct StaticTimeCapsQuery {
	StaticTimeCapsQuery() : mTimeCaps({0})
	{
		timeGetDevCaps(&mTimeCaps, sizeof(mTimeCaps));
		timeBeginPeriod(mTimeCaps.wPeriodMin);
	}
	~StaticTimeCapsQuery()
	{
		timeEndPeriod(mTimeCaps.wPeriodMin);
	}

	TIMECAPS mTimeCaps;
};

static StaticTimeCapsQuery __time_caps_init;

#endif

enum class thread_error : U32 {
	THREAD_SUCCESS = 0,
	THREAD_INVALID_PARAMS = 1,
	THREAD_NO_AVAILABLE = 2,
	THREAD_MISSING_THREAD = 3,
	THREAD_UNKNOWN_ERROR = 4,
	THREAD_INVALID_CALL = 5,
	THREAD_CANT_JOIN_LAST_OPERATION = 6
};

template<typename Func, typename ... Args>
class thread {
public:
#ifdef MBASE_PLATFORM_WINDOWS
	using raw_handle = HANDLE;
#endif
#ifdef MBASE_PLATFORM_UNIX
	using raw_handle = pthread_t;
#endif

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE thread(Func&& in_fptr, Args&&... in_args) noexcept;
	MBASE_INLINE thread(thread&& in_rhs) noexcept;
	MBASE_INLINE ~thread() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 get_id() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE static I32 get_current_thread_id() noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE thread_error run() noexcept;
	MBASE_INLINE thread_error run(Func&& in_fptr, Args&&... in_args) noexcept;
	MBASE_INLINE thread_error run_with_args(Args&... in_args) noexcept;
	MBASE_INLINE thread_error join() noexcept;
	MBASE_INLINE thread_error halt() noexcept; // undefined in linux environment
	MBASE_INLINE thread_error resume() noexcept; // undefined in linux environment
	MBASE_INLINE thread_error exit(I32 in_exit_code) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

private:
	struct thread_param {
		thread_param(){}
		std::decay_t<Func> fPtr;
		std::tuple<Args...> fParams;
	};
#ifdef MBASE_PLATFORM_WINDOWS
	static DWORD THREAD_ROUTINE(LPVOID in_param) 
	{
		thread_param* targetParams = (thread_param*)in_param;
		std::apply(targetParams->fPtr, targetParams->fParams);
		return 0;
	}
#endif
#ifdef MBASE_PLATFORM_UNIX
	static void* THREAD_ROUTINE(PTRGENERIC in_param)
	{
		thread_param* targetParams = (thread_param*)in_param;
		std::apply(targetParams->fPtr, targetParams->fParams);
		return nullptr;
	}
#endif
	MBASE_INLINE thread_error _run() noexcept 
	{
		thread_error _te;
		_te = join();
		if(_te == thread_error::THREAD_INVALID_CALL)
		{
			// THREAD DO NOT RESPOND
			// REPORT ON DIAGNOSTICS
			return thread_error::THREAD_CANT_JOIN_LAST_OPERATION;
		}
#ifdef MBASE_PLATFORM_WINDOWS

		raw_handle _mThreadHandle = CreateThread(nullptr,
			0,
			(LPTHREAD_START_ROUTINE)THREAD_ROUTINE,
			(LPVOID)&mThreadParams,
			0,
			(LPDWORD)&mThreadId);
		
		mThreadHandle = _mThreadHandle;
		if (!mThreadHandle)
		{
			return (thread_error)GetLastError();
		}
#endif
#ifdef MBASE_PLATFORM_UNIX
		I32 threadCreationRes = pthread_create(&mThreadHandle, nullptr, THREAD_ROUTINE, &mThreadParams);
		if(threadCreationRes)
		{
			return thread_error::THREAD_NO_AVAILABLE;
		}
		mThreadId = mThreadHandle;
#endif
		return thread_error::THREAD_SUCCESS;
	}
	thread_param mThreadParams;
	I32 mThreadId;
	raw_handle mThreadHandle;
	bool mIsHalted;
};

template<typename Func, typename ...Args>
MBASE_INLINE thread<Func, Args...>::thread(Func&& in_fptr, Args && ...in_args) noexcept {
	mThreadParams.fPtr = std::forward<Func>(in_fptr);
	mThreadParams.fParams = std::make_tuple(std::forward<Args>(in_args)...);

	mThreadHandle = 0;
	mThreadId = 0;
	mIsHalted = false;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread<Func, Args...>::thread(thread&& in_rhs) noexcept {
	mThreadParams.fPtr = in_rhs.mThreadParams.fPtr;
	mThreadParams.fParams = in_rhs.mThreadParams.fParams;
	mThreadHandle = in_rhs.mThreadHandle;
	mThreadId = in_rhs.mThreadId;
	mIsHalted = in_rhs.mIsHalted;

	in_rhs.mThreadHandle = 0;
	in_rhs.mThreadId = 0;
	in_rhs.mIsHalted = false;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread<Func, Args...>::~thread() noexcept {
	join();
}

template<typename Func, typename ...Args>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 thread<Func, Args...>::get_id() const noexcept {
	if (!mThreadId)
	{
#ifdef MBASE_PLATFORM_WINDOWS
		return GetCurrentThreadId();
#endif
#ifdef MBASE_PLATFORM_UNIX
		return pthread_self();
#endif
	}

	return mThreadId;
}

template<typename Func, typename ...Args>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 thread<Func, Args...>::get_current_thread_id() noexcept {
#ifdef MBASE_PLATFORM_WINDOWS
	return GetCurrentThreadId();
#endif
#ifdef MBASE_PLATFORM_UNIX
	return pthread_self();
#endif
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::run() noexcept {
	return _run();
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::run(Func&& in_fptr, Args&&... in_args) noexcept {
	mThreadParams.fPtr = std::forward<Func>(in_fptr);
	mThreadParams.fParams = std::make_tuple(std::forward<Args>(in_args)...);
	return _run();
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::run_with_args(Args&... in_args) noexcept {
	mThreadParams.fParams = std::make_tuple(std::forward<Args>(in_args)...);
	return _run();
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::join() noexcept {
	if (mThreadHandle)
	{
		thread_error t_err = thread_error::THREAD_SUCCESS;
#ifdef MBASE_PLATFORM_WINDOWS
		if (WaitForSingleObject(mThreadHandle, INFINITE) == WAIT_FAILED)
		{
			t_err = thread_error::THREAD_INVALID_CALL;
		}
#endif
#ifdef MBASE_PLATFORM_UNIX
		if(pthread_join(mThreadHandle, nullptr))
		{
			t_err = thread_error::THREAD_INVALID_CALL;
		}
#endif
		mThreadHandle = 0;
		mThreadId = 0;
		return t_err;
	}

	return thread_error::THREAD_MISSING_THREAD;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::halt() noexcept {
	if (mThreadHandle)
	{
#ifdef MBASE_PLATFORM_WINDOWS
		if (SuspendThread(mThreadHandle) == -1)
		{
			return thread_error::THREAD_UNKNOWN_ERROR;
		}
		return thread_error::THREAD_SUCCESS;
#endif
	}

	return thread_error::THREAD_MISSING_THREAD;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::resume() noexcept {
	if (mThreadHandle)
	{
#ifdef MBASE_PLATFORM_WINDOWS
		if (ResumeThread(mThreadHandle) == -1)
		{
			return thread_error::THREAD_UNKNOWN_ERROR;
		}
		return thread_error::THREAD_SUCCESS;
#endif
	}
	return thread_error::THREAD_MISSING_THREAD;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::exit([[maybe_unused]] I32 in_exit_code) noexcept {
	if (mThreadHandle)
	{
#ifdef MBASE_PLATFORM_WINDOWS
		if (TerminateThread(mThreadHandle, in_exit_code) == -1)
		{
			mThreadHandle = nullptr;
			mThreadId = 0;
			return thread_error::THREAD_UNKNOWN_ERROR;
		}
#endif
#ifdef MBASE_PLATFORM_UNIX
		if(pthread_cancel(mThreadHandle))
		{
			mThreadHandle = 0;
			mThreadId = 0;
			return thread_error::THREAD_MISSING_THREAD;
		}
#endif
		mThreadHandle = 0;
		mThreadId = 0;
		return thread_error::THREAD_SUCCESS;
	}
	return thread_error::THREAD_MISSING_THREAD;
}

MBASE_INLINE GENERIC sleep(I32 in_ms) noexcept {
	#ifdef MBASE_PLATFORM_WINDOWS
	Sleep(in_ms);
	#endif
	
	#ifdef MBASE_PLATFORM_UNIX
	struct timespec ts;
    ts.tv_sec = in_ms / 1000;
    ts.tv_nsec = (in_ms % 1000) * 1000000;
    nanosleep(&ts, nullptr);
	#endif
}

MBASE_STD_END

#endif // MBASE_THREAD_H