#ifndef MBASE_LOGICAL_PROCESSING_H
#define MBASE_LOGICAL_PROCESSING_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/thread.h>
#include <mbase/synchronization.h>
#include <mbase/behaviors.h>
#include <atomic>

MBASE_BEGIN

class MBASE_API processor_signal {
public:
	processor_signal() : mSignalState(false), mSignal(false) {}

	bool get_signal_state() const { return mSignalState; }
	bool get_signal() const { return mSignal; }

	GENERIC set_signal_finished() 
	{
		mSignalState = true;
		mSignal = false;
	}

	GENERIC set_signal()
	{
		mSignal = true;
	}

	GENERIC set_signal_state()
	{
		mSignalState = true;
	}

	GENERIC set_signal_with_state()
	{
		mSignalState = true;
		mSignal = true;
	}

	GENERIC reset_signal()
	{
		mSignal = false;
	}

	GENERIC reset_signal_state()
	{
		mSignalState = false;
	}

	GENERIC reset_signal_with_state()
	{
		mSignalState = false;
		mSignal = false;
	}

private:
	volatile bool mSignalState;
	volatile bool mSignal;
};

class MBASE_API logical_processor : public mbase::non_copymovable {
public:
	logical_processor() : mProcessorThread(_update_t_static, this), mIsProcessorRunning(false) {}
	~logical_processor() 
	{ 
		stop_processor();
	}

	bool is_processor_running() { return mIsProcessorRunning; }

	static GENERIC _update_t_static(logical_processor* in_self) 
	{
		in_self->update_t();
	}

	GENERIC start_processor() 
	{
		if(mIsProcessorRunning)
		{
			// if it is running, do nothing
			return;
		}
		mIsProcessorRunning = true;
		mProcessorThread.run();
	}

	GENERIC stop_processor()
	{
		if(mIsProcessorRunning)
		{
			mIsProcessorRunning = false;
			mProcessorThread.join();
		}
	}
	GENERIC acquire_synchronizer() { mLogicSynchronizer.acquire(); }
	GENERIC release_synchronizer() { mLogicSynchronizer.release(); }
	virtual GENERIC update() = 0; // Program logic goes here ...
	virtual GENERIC update_t() = 0; // Program processing goes here ...
	
protected:
	mbase::thread<decltype(_update_t_static), logical_processor*> mProcessorThread;
	mbase::mutex mLogicSynchronizer;
	bool mIsProcessorRunning;
};

MBASE_END

#endif // !MBASE_LOGICAL_PROCESSING_H
