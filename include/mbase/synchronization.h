#ifndef MBASE_SYNCHRONIZATION_H
#define MBASE_SYNCHRONIZATION_H

#include <mbase/common.h>
#include <Windows.h> // CreateMutexA, CloseHandle, WaitForSingleObject, ReleaseMutex

MBASE_STD_BEGIN

class mutex {
public:
	using raw_handle = HANDLE;

	mutex() noexcept 
	{ // ASSUME 100% SUCCESS
		mHandle = CreateMutexA(
			nullptr,
			false,
			nullptr
		);
	}

	~mutex() noexcept 
	{
		CloseHandle(mHandle);
	}

	MBASE_INLINE GENERIC acquire() noexcept 
	{
		WaitForSingleObject(mHandle, INFINITE);
	}

	MBASE_INLINE GENERIC release() noexcept 
	{
		ReleaseMutex(mHandle);
	}

private:
	raw_handle mHandle;
};

MBASE_STD_END

#endif // MBASE_SYNCHRONIZATION_H