#ifndef MBASE_SYNCHRONIZATION_H
#define MBASE_SYNCHRONIZATION_H

#include <mbase/common.h>

#ifdef MBASE_PLATFORM_WINDOWS
#include <Windows.h> // CreateMutexA, CloseHandle, WaitForSingleObject, ReleaseMutex
#endif

#ifdef MBASE_PLATFORM_UNIX
#include <pthread.h>
#endif

MBASE_STD_BEGIN

class mutex {
public:
#ifdef MBASE_PLATFORM_WINDOWS
	using raw_handle = HANDLE;
#endif
#ifdef MBASE_PLATFORM_UNIX
	using raw_handle = pthread_mutex_t;
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
#ifdef MBASE_PLATFORM_UNIX
		pthread_mutex_init(&mHandle, NULL);
#endif
	}

	~mutex() noexcept 
	{
        this->release();
#ifdef MBASE_PLATFORM_WINDOWS
		CloseHandle(mHandle);
#endif
#ifdef MBASE_PLATFORM_UNIX
		pthread_mutex_destroy(&mHandle);
#endif
	}

	MBASE_INLINE GENERIC acquire() noexcept 
	{
#ifdef MBASE_PLATFORM_WINDOWS
		WaitForSingleObject(mHandle, INFINITE);
#endif
#ifdef MBASE_PLATFORM_UNIX
		pthread_mutex_lock(&mHandle);
#endif
	}

	MBASE_INLINE GENERIC release() noexcept 
	{
#ifdef MBASE_PLATFORM_WINDOWS
		ReleaseMutex(mHandle);
#endif
#ifdef MBASE_PLATFORM_UNIX
		pthread_mutex_unlock(&mHandle);
#endif
	}

private:
	raw_handle mHandle;
};

class lock_guard {
public:
	MBASE_INLINE lock_guard(mbase::mutex& in_mutex) noexcept
	{
		mMutex = &in_mutex;
		mMutex->acquire();
	}

	~lock_guard() noexcept
	{
		mMutex->release();
	}
private:
	mbase::mutex* mMutex;
};

MBASE_STD_END

#endif // MBASE_SYNCHRONIZATION_H
