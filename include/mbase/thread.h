#ifndef MBASE_THREAD_H
#define MBASE_THREAD_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <Windows.h>

MBASE_STD_BEGIN

template<typename Func, typename ... Args>
class thread {
public:
	enum class thread_error : U32 {
		THREAD_SUCCESS = 0,
		THREAD_INVALID_PARAMS = ERROR_INVALID_PARAMETER,
		THREAD_NO_AVAILABLE = ERROR_ALREADY_THREAD,
		THREAD_MISSING_THREAD = 3,
		THREAD_UNKNOWN_ERROR = 4,
		THREAD_INVALID_CALL = 5
	};

	using raw_handle = HANDLE;


	thread(Func&& in_fptr, Args&&... in_args) noexcept {
		tp.fPtr = std::forward<Func>(in_fptr);
		tp.fParams = std::make_tuple(std::forward<Args>(in_args)...);
	}

	thread(thread&& in_rhs) noexcept {
		tp.fPtr = in_rhs.tp.fPtr;
		tp.fParams = in_rhs.tp.fParams;
		threadHandle = in_rhs.threadHandle;
		threadId = in_rhs.threadId;

		in_rhs.threadHandle = nullptr;
		in_rhs.threadId = 0;
	}

	~thread() noexcept {
		join();
	}

	MBASE_INLINE thread_error run() noexcept {
		return _run();
	}

	MBASE_INLINE thread_error run(Func&& in_fptr, Args&&... in_args) noexcept {
		tp.fPtr = std::forward<Func>(in_fptr);
		tp.fParams = std::make_tuple(std::forward<Args>(in_args)...);
		return _run();
	}

	MBASE_INLINE thread_error run_with_args(Args&... in_args) noexcept {
		tp.fParams = std::make_tuple(std::forward<Args>(in_args)...);
		return _run();
	}

	MBASE_INLINE thread_error join() noexcept {
		if(threadHandle)
		{
			thread_error t_err = thread_error::THREAD_SUCCESS;
			if(WaitForSingleObject(threadHandle, INFINITE) == WAIT_FAILED)
			{
				t_err = thread_error::THREAD_INVALID_CALL;
			}
			threadHandle = nullptr;
			threadId = 0;
			return t_err;
		}

		return thread_error::THREAD_MISSING_THREAD;
	}

	MBASE_INLINE thread_error halt() noexcept {
		if(threadHandle)
		{
			if(SuspendThread(threadHandle) == -1)
			{
				return thread_error::THREAD_UNKNOWN_ERROR;
			}
			return thread_error::THREAD_SUCCESS;
		}

		return thread_error::THREAD_MISSING_THREAD;
	}

	MBASE_INLINE thread_error resume() noexcept {
		if(threadHandle)
		{
			if(ResumeThread(threadHandle) == -1)
			{
				return thread_error::THREAD_UNKNOWN_ERROR;
			}
			return thread_error::THREAD_SUCCESS;
		}
		return thread_error::THREAD_MISSING_THREAD;
	}

	MBASE_INLINE thread_error exit(I32 in_exit_code) noexcept {
		if(threadHandle)
		{
			if(TerminateThread(threadHandle, in_exit_code) == -1)
			{
				return thread_error::THREAD_UNKNOWN_ERROR;
			}
			return thread_error::THREAD_SUCCESS;
		}
		return thread_error::THREAD_MISSING_THREAD;
	}

	USED_RETURN MBASE_INLINE I32 get_id() noexcept {
		if(!threadId)
		{
			return GetCurrentThreadId();
		}

		return threadId;
	}

private:
	struct thread_param {
		thread_param(){}
		std::decay_t<Func> fPtr;
		std::tuple<Args...> fParams;
	};

	static DWORD THREAD_ROUTINE(LPVOID lParam) {
		thread_param* targetParams = (thread_param*)lParam;
		std::apply(targetParams->fPtr, targetParams->fParams);
		return 0;
	}

	MBASE_INLINE thread_error _run() noexcept {
		join();

		raw_handle _threadHandle = CreateThread(nullptr,
			0,
			THREAD_ROUTINE,
			(LPVOID)&tp,
			0,
			(LPDWORD)&threadId);
		
		threadHandle = _threadHandle;
		if (!threadHandle)
		{
			return (thread_error)GetLastError();
		}

		return thread_error::THREAD_SUCCESS;
	}
	thread_param tp;
	I32 threadId;
	raw_handle threadHandle;
};

MBASE_STD_END

#endif // MBASE_THREAD_H