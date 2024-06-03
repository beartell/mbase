#ifndef MBASE_THREAD_H
#define MBASE_THREAD_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <tuple>
#include <Windows.h>

MBASE_STD_BEGIN

enum class thread_error : U32 {
	THREAD_SUCCESS = 0,
	THREAD_INVALID_PARAMS = ERROR_INVALID_PARAMETER,
	THREAD_NO_AVAILABLE = ERROR_ALREADY_THREAD,
	THREAD_MISSING_THREAD = 3,
	THREAD_UNKNOWN_ERROR = 4,
	THREAD_INVALID_CALL = 5,
	THREAD_CANT_JOIN_LAST_OPERATION = 6
};

template<typename Func, typename ... Args>
class thread {
public:
	using raw_handle = HANDLE;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE thread(Func&& in_fptr, Args&&... in_args) noexcept;
	MBASE_INLINE thread(thread&& in_rhs) noexcept;
	MBASE_INLINE ~thread() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 get_id() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) static I32 get_current_thread_id() noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE thread_error run() noexcept;
	MBASE_INLINE thread_error run(Func&& in_fptr, Args&&... in_args) noexcept;
	MBASE_INLINE thread_error run_with_args(Args&... in_args) noexcept;
	MBASE_INLINE thread_error join() noexcept;
	MBASE_INLINE thread_error halt() noexcept;
	MBASE_INLINE thread_error resume() noexcept;
	MBASE_INLINE thread_error exit(I32 in_exit_code) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

private:
	struct thread_param {
		thread_param(){}
		std::decay_t<Func> fPtr;
		std::tuple<Args...> fParams;
	};

	static DWORD THREAD_ROUTINE(LPVOID lParam) 
	{
		thread_param* targetParams = (thread_param*)lParam;
		std::apply(targetParams->fPtr, targetParams->fParams);
		return 0;
	}

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

		raw_handle _mThreadHandle = CreateThread(nullptr,
			0,
			THREAD_ROUTINE,
			(LPVOID)&mThreadParams,
			0,
			(LPDWORD)&mThreadId);
		
		mThreadHandle = _mThreadHandle;
		if (!mThreadHandle)
		{
			return (thread_error)GetLastError();
		}

		return thread_error::THREAD_SUCCESS;
	}
	thread_param mThreadParams;
	I32 mThreadId;
	raw_handle mThreadHandle;
};

template<typename Func, typename ...Args>
MBASE_INLINE thread<Func, Args...>::thread(Func&& in_fptr, Args && ...in_args) noexcept {
	mThreadParams.fPtr = std::forward<Func>(in_fptr);
	mThreadParams.fParams = std::make_tuple(std::forward<Args>(in_args)...);

	mThreadHandle = nullptr;
	mThreadId = 0;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread<Func, Args...>::thread(thread&& in_rhs) noexcept {
	mThreadParams.fPtr = in_rhs.mThreadParams.fPtr;
	mThreadParams.fParams = in_rhs.mThreadParams.fParams;
	mThreadHandle = in_rhs.mThreadHandle;
	mThreadId = in_rhs.mThreadId;

	in_rhs.mThreadHandle = nullptr;
	in_rhs.mThreadId = 0;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread<Func, Args...>::~thread() noexcept {
	join();
}

template<typename Func, typename ...Args>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 thread<Func, Args...>::get_id() noexcept {
	if (!mThreadId)
	{
		return GetCurrentThreadId();
	}

	return mThreadId;
}

template<typename Func, typename ...Args>
MBASE_ND(MBASE_OBS_IGNORE) I32 thread<Func, Args...>::get_current_thread_id() noexcept {
	return GetCurrentThreadId();
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
		if (WaitForSingleObject(mThreadHandle, INFINITE) == WAIT_FAILED)
		{
			t_err = thread_error::THREAD_INVALID_CALL;
		}
		mThreadHandle = nullptr;
		mThreadId = 0;
		return t_err;
	}

	return thread_error::THREAD_MISSING_THREAD;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::halt() noexcept {
	if (mThreadHandle)
	{
		if (SuspendThread(mThreadHandle) == -1)
		{
			return thread_error::THREAD_UNKNOWN_ERROR;
		}
		return thread_error::THREAD_SUCCESS;
	}

	return thread_error::THREAD_MISSING_THREAD;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::resume() noexcept {
	if (mThreadHandle)
	{
		if (ResumeThread(mThreadHandle) == -1)
		{
			return thread_error::THREAD_UNKNOWN_ERROR;
		}
		return thread_error::THREAD_SUCCESS;
	}
	return thread_error::THREAD_MISSING_THREAD;
}

template<typename Func, typename ...Args>
MBASE_INLINE thread_error thread<Func, Args...>::exit(I32 in_exit_code) noexcept {
	if (mThreadHandle)
	{
		if (TerminateThread(mThreadHandle, in_exit_code) == -1)
		{
			mThreadHandle = nullptr;
			mThreadId = 0;
			return thread_error::THREAD_UNKNOWN_ERROR;
		}
		mThreadHandle = nullptr;
		mThreadId = 0;
		return thread_error::THREAD_SUCCESS;
	}
	return thread_error::THREAD_MISSING_THREAD;
}

MBASE_STD_END

#endif // MBASE_THREAD_H