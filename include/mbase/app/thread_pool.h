#ifndef MBASE_THREAD_POOL_H
#define MBASE_THREAD_POOL_H

#include <mbase/common.h>
#include <mbase/stack.h>
#include <mbase/synchronization.h>
#include <mbase/app/handler_base.h>
#include <mbase/thread.h>
#include <mbase/atomic.h>

#define MBASE_TPOOL_MAX_THREADS 1024 // ARBITRARY NUMBER. FIND A WAY TO CALCULATE MAX THREAD COUNT
#define MBASE_TPOOL_DEFAULT_THREADS 32

MBASE_BEGIN

class tpool : public non_copymovable {
public:
	struct thread_pool_routine_args {
		static I32 _PoolRoutine(thread_pool_routine_args* in_args) {
			while (in_args->selfClass->isRunning)
			{
				if (in_args->tHandler)
				{
					in_args->tHandler->on_call(in_args->tHandler->GetUserData());
					in_args->tHandler = nullptr;
					in_args->selfClass->_UpdateIndex(in_args->tIndex);
				}

				in_args->selfThread.halt();
			}
			return 0;
		}

		thread_pool_routine_args() : selfClass(nullptr), tHandler(nullptr), selfThread(_PoolRoutine, nullptr), tIndex(0) {}
		tpool* selfClass;
		I32 tIndex;
		handler_base* tHandler;
		mbase::thread<decltype(_PoolRoutine), thread_pool_routine_args*> selfThread;
	};

	tpool() noexcept : isRunning(true){
		threadCount = MBASE_TPOOL_DEFAULT_THREADS;

		threadPool = new thread_pool_routine_args[threadCount];

		for (I32 i = threadCount - 1; i != -1; --i)
		{
			thread_pool_routine_args* tpra = threadPool + i;
			tpra->selfClass = this;
			tpra->tIndex = i;
			tpra->selfThread.run_with_args(tpra);
			freeThreadIndex.push(tpra->tIndex);
		}
	}

	MBASE_EXPLICIT tpool(U32 in_thread_count) noexcept : isRunning(true) {
		threadCount = in_thread_count;
		if(!threadCount || threadCount > MBASE_TPOOL_MAX_THREADS)
		{
			// notify the user about it
			threadCount = MBASE_TPOOL_DEFAULT_THREADS;
		}

		threadPool = new thread_pool_routine_args[threadCount];

		for(I32 i = threadCount - 1; i != -1; --i)
		{
			thread_pool_routine_args* tpra = threadPool + i;
			tpra->selfClass = this;
			tpra->tIndex = i;
			tpra->selfThread.run_with_args(tpra);
			freeThreadIndex.push(tpra->tIndex);
		}
	}

	~tpool() noexcept {
		thread_pool_routine_args* tpra = threadPool;
		isRunning = false;
		for(I32 i = 0; i < threadCount; i++)
		{
			// finish all execution
			tpra->selfThread.resume();
			++tpra;
		}

		delete[]threadPool;
	}

	GENERIC ExecuteJob(handler_base* in_handler) {
		MBASE_NULL_CHECK_RETURN(in_handler);

		mtx.acquire();
		if(!freeThreadIndex.size())
		{
			mtx.release();
			return;
		}

		I32 freeIndex = freeThreadIndex.top();
		freeThreadIndex.pop();
		++activeThreadCounter;

		threadPool[freeIndex].tHandler = in_handler;
		threadPool[freeIndex].tHandler->_SetThreadIndex(freeIndex);
		threadPool[freeIndex].selfThread.resume();
		mtx.release();
	}

	MBASE_ND("thread pool observation being ignored") U32 GetThreadCount() const noexcept {
		return threadCount;
	}

	MBASE_ND("thread pool routine info being ignored") thread_pool_routine_args* GetRoutineInfo(I32 in_index) noexcept {
		return threadPool + in_index;
	}

	// INTERNAL CALL
	// MUST NOT BE CALLED MANUALLY BY USER
	GENERIC _UpdateIndex(I32 in_index) noexcept {
		mtx.acquire();

		freeThreadIndex.push(in_index);
		++activeThreadCounter;

		mtx.release();
	}

private:

	bool isRunning;
	U32 threadCount;
	mbase::mutex mtx;
	mbase::atomic_i32 activeThreadCounter;
	mbase::stack<I32> freeThreadIndex;
	thread_pool_routine_args* threadPool;
};

MBASE_END

#endif // MBASE_THREAD_POOL_H