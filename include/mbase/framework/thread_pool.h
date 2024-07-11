#ifndef MBASE_THREAD_POOL_H
#define MBASE_THREAD_POOL_H

#include <mbase/common.h>
#include <mbase/stack.h>
#include <mbase/synchronization.h>
#include <mbase/framework/handler_base.h>
#include <mbase/thread.h>

MBASE_BEGIN

static const U32 gThreadPoolMaxThreads = 1024; // ARBITRARY NUMBER. FIND A WAY TO CALCULATE MAX THREAD COUNT
static const U32 gThreadPoolDefaultThread = 32;

class tpool : public non_copymovable {
public:
	struct thread_pool_routine_args {
		static I32 _pool_routine(thread_pool_routine_args* in_args) {
			while (in_args->selfClass->mIsRunning)
			{
				if (in_args->tHandler)
				{
					in_args->tHandler->on_call(in_args->tHandler->get_user_data());
					in_args->tHandler = nullptr;
					in_args->selfClass->_update_index(in_args->tIndex);
				}

				in_args->selfThread.halt();
			}
			return 0;
		}

		MBASE_INLINE thread_pool_routine_args() : selfClass(nullptr), tHandler(nullptr), selfThread(_pool_routine, nullptr), tIndex(0) {}
		tpool* selfClass;
		I32 tIndex;
		handler_base* tHandler;
		mbase::thread<decltype(_pool_routine), thread_pool_routine_args*> selfThread;
	};

	MBASE_INLINE tpool() noexcept;
	MBASE_INLINE MBASE_EXPLICIT tpool(U32 in_thread_count) noexcept;
	MBASE_INLINE ~tpool() noexcept;

	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_thread_count() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE thread_pool_routine_args* get_routine_info(I32 in_index) noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const thread_pool_routine_args* get_routine_info(I32 in_index) const noexcept;

	MBASE_INLINE GENERIC execute_job(handler_base& in_handler);
	MBASE_INLINE GENERIC _update_index(I32 in_index) noexcept;

private:
	bool mIsRunning;
	U32 mThreadCount;
	mbase::mutex mtx;
	mbase::stack<I32> mFreeThreadIndex;
	thread_pool_routine_args* mThreadPool;
};

MBASE_INLINE tpool::tpool() noexcept : mIsRunning(true)
{
	mThreadCount = gThreadPoolDefaultThread;
	mThreadPool = new thread_pool_routine_args[mThreadCount];

	for (I32 i = mThreadCount - 1; i != -1; --i)
	{
		thread_pool_routine_args* tpra = mThreadPool + i;
		tpra->selfClass = this;
		tpra->tIndex = i;
		tpra->selfThread.run_with_args(tpra);
		mFreeThreadIndex.push(tpra->tIndex);
	}
}

MBASE_INLINE tpool::tpool(U32 in_thread_count) noexcept : mIsRunning(true)
{
	mThreadCount = in_thread_count;
	if (!mThreadCount || mThreadCount > gThreadPoolMaxThreads)
	{
		// notify the user about it
		mThreadCount = gThreadPoolDefaultThread;
	}

	mThreadPool = new thread_pool_routine_args[mThreadCount];

	for (I32 i = mThreadCount - 1; i != -1; --i)
	{
		thread_pool_routine_args* tpra = mThreadPool + i;
		tpra->selfClass = this;
		tpra->tIndex = i;
		tpra->selfThread.run_with_args(tpra);
		mFreeThreadIndex.push(tpra->tIndex);
	}
}

MBASE_INLINE tpool::~tpool() noexcept
{
	thread_pool_routine_args* tpra = mThreadPool;
	mIsRunning = false;
	for (I32 i = 0; i < mThreadCount; i++)
	{
		// finish all execution
		tpra->selfThread.resume();
		++tpra;
	}

	delete[]mThreadPool;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 tpool::get_thread_count() const noexcept
{
	return mThreadCount;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE tpool::thread_pool_routine_args* tpool::get_routine_info(I32 in_index) noexcept
{
	return mThreadPool + in_index;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const tpool::thread_pool_routine_args* tpool::get_routine_info(I32 in_index) const noexcept
{
	return mThreadPool + in_index;
}

MBASE_INLINE GENERIC tpool::execute_job(handler_base& in_handler)
{
	mtx.acquire();
	if (!mFreeThreadIndex.size())
	{
		mtx.release();
		return;
	}

	I32 freeIndex = mFreeThreadIndex.top();
	mFreeThreadIndex.pop();

	mThreadPool[freeIndex].tHandler = &in_handler;
	mThreadPool[freeIndex].tHandler->_set_thread_index(freeIndex);
	mThreadPool[freeIndex].selfThread.resume();
	mtx.release();
}

MBASE_INLINE GENERIC tpool::_update_index(I32 in_index) noexcept
{
	mtx.acquire();

	mFreeThreadIndex.push(in_index);

	mtx.release();
}

MBASE_END

#endif // MBASE_THREAD_POOL_H