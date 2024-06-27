#ifndef MBASE_SYNCHRONIZATION_H
#define MBASE_SYNCHRONIZATION_H

#include <mbase/common.h>

#ifdef MBASE_PLATFORM_WINDOWS
#include <Windows.h> // CreateMutexA, CloseHandle, WaitForSingleObject, ReleaseMutex
#endif

MBASE_STD_BEGIN

class mutex {
public:
#ifdef MBASE_PLATFORM_WINDOWS
	using raw_handle = HANDLE;
#endif

	mutex() noexcept 
	{ // ASSUME 100% SUCCESS
#ifdef MBASE_PLATFORM_WINDOWS
		mHandle = CreateMutexA(
			nullptr,
			false,
			nullptr
		);
#endif
	}

	~mutex() noexcept 
	{
#ifdef MBASE_PLATFORM_WINDOWS
		CloseHandle(mHandle);
#endif
	}

	MBASE_INLINE GENERIC acquire() noexcept 
	{
#ifdef MBASE_PLATFORM_WINDOWS
		WaitForSingleObject(mHandle, INFINITE);
#endif
	}

	MBASE_INLINE GENERIC release() noexcept 
	{
#ifdef MBASE_PLATFORM_WINDOWS
		ReleaseMutex(mHandle);
#endif
	}

private:
	raw_handle mHandle;
};

MBASE_STD_END

#endif // MBASE_SYNCHRONIZATION_H