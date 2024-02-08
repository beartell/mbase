#ifndef MBASE_THREAD_POOL_H
#define MBASE_THREAD_POOL_H

#include <mbase/common.h>
#include <mbase/stack.h>
#include <mbase/queue.h>
#include <mbase/synchronization.h>
#include <mbase/thread.h>
#include <iostream>
#include <tuple>

#define MBASE_TPOOL_MAX_THREADS 512 // ARBITRARY NUMBER. FIND A WAY TO CALCULATE MAX THREAD COUNT
#define MBASE_TPOOL_DEFAULT_THREADS 16

MBASE_BEGIN

class thread_pool_handler {
public:
	using user_data = PTRGENERIC;

	thread_pool_handler() noexcept : suppliedData(nullptr) {}
	
	virtual GENERIC on_call(user_data in_data) { /* Do nothing literally */ };
	
	GENERIC SetUserData(user_data in_data) noexcept {
		suppliedData = in_data;
	}

	USED_RETURN user_data GetUserData() noexcept {
		return suppliedData;
	}

	USED_RETURN U32 GetHandlerId() noexcept {
		return handlerId;
	}

protected:
	U32 handlerId;
	user_data suppliedData;
};

// OnCallable is an object type that has an on_call method
template <typename OnCallable>
class tpool : public non_copymovable {
public:
	tpool() noexcept : isRunning(true){
		threadCount = MBASE_TPOOL_DEFAULT_THREADS;

		threadPool = new thread_pool_routine_args[threadCount];

		for (I32 i = 0; i < threadCount; i++)
		{
			thread_pool_routine_args* tpra = threadPool + i;
			tpra->selfClass = this;
			tpra->tIndex = i;
			tpra->selfThread.run_with_args(tpra);
		}
	}

	MBASE_EXPLICIT tpool(U32 in_thread_count) noexcept : isRunning(true) {
		threadCount = in_thread_count;
		if(!threadCount || threadCount > MBASE_TPOOL_MAX_THREADS)
		{
			threadCount = MBASE_TPOOL_DEFAULT_THREADS;
		}

		threadPool = new thread_pool_routine_args[threadCount];

		for(I32 i = 0; i < threadCount; i++)
		{
			thread_pool_routine_args* tpra = threadPool + i;
			tpra->selfClass = this;
			tpra->tIndex = i;
			tpra->selfThread.run_with_args(tpra);
		}
	}

	~tpool() noexcept {
		thread_pool_routine_args* tpra = threadPool;
		for(I32 i = 0; i < threadCount; i++)
		{
			// finish all execution
			tpra->selfThread.resume();
			tpra++;
		}

		delete[]threadPool;
	}

	GENERIC ExecuteJob(std::add_lvalue_reference_t<OnCallable> in_handler) noexcept {
		mtx.acquire();
		if(!freeThreadIndex.size())
		{
			mtx.release();
			return;
		}

		I32 freeIndex = freeThreadIndex.top();
		threadPool[freeIndex].tHandler = &in_handler;
		threadPool[freeIndex].selfThread.resume();
		freeThreadIndex.pop();
		mtx.release();
	}

	USED_RETURN U32 GetThreadCount() noexcept {
		return threadCount;
	}

	// INTERNAL CALL
	// MUST NOT BE CALLED MANUALLY BY USER
	GENERIC _UpdateIndex(I32 in_index) noexcept {
		mtx.acquire();

		freeThreadIndex.push(in_index);
		
		mtx.release();
	}

private:

	struct thread_pool_routine_args {
		static I32 _PoolRoutine(thread_pool_routine_args* in_args) {
			while (in_args->selfClass->isRunning)
			{
				if (in_args->tHandler)
				{
					in_args->tHandler->on_call(in_args->tHandler->GetUserData());
				}
				in_args->selfClass->_UpdateIndex(in_args->tIndex);
				in_args->selfThread.halt();
			}
			return 0;
		}

		thread_pool_routine_args() : selfClass(nullptr), tHandler(nullptr), selfThread(_PoolRoutine, nullptr) {}
		tpool* selfClass;
		I32 tIndex;
		OnCallable* tHandler;
		mbase::thread<decltype(_PoolRoutine), thread_pool_routine_args*> selfThread;
	};

	bool isRunning;
	U32 threadCount;
	mbase::mutex mtx;
	mbase::stack<I32> freeThreadIndex;
	thread_pool_routine_args* threadPool;
};

MBASE_END

#endif // MBASE_THREAD_POOL_H