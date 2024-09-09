#ifndef MBASE_LOGICAL_PROCESSING_H
#define MBASE_LOGICAL_PROCESSING_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/thread.h>
#include <mbase/synchronization.h>
#include <mbase/behaviors.h>
#include <mbase/framework/handler_base.h>

MBASE_BEGIN

class processor_signal {
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
	bool mSignalState;
	bool mSignal;
};

class logical_processor : public mbase::non_copymovable {
public:
	using logic_handler = handler_base;
	using logic_handler_ref = handler_base&;

	logical_processor() : mProcessorThread(_update_t_static, this), mIsProcessorRunning(false) {}
	~logical_processor() 
	{ 
		if(mIsProcessorRunning)
		{
			mProcessorThread.join();
		}
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
			mProcessorThread.join();
		}
		mIsProcessorRunning = false;
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

class single_logical_processor : public logical_processor {
public:
	single_logical_processor() : mLogicHandler(NULL), logical_processor() {}
	single_logical_processor() { mLogicHandler = NULL; }

	GENERIC set_logic_handler(logic_handler_ref in_handler) { mLogicHandler = &in_handler; }
	GENERIC release_logic_handler() { mLogicHandler = NULL; }
	logic_handler_ref get_logic_handler() { return *mLogicHandler; }
protected:
	logic_handler* mLogicHandler;
};

class multi_logical_processor : public logical_processor {
public:
	multi_logical_processor() : mLogicHandler(), logical_processor() {}

	GENERIC enqueue_logic_handler(logic_handler_ref in_handler) { mLogicHandler.push_back(&in_handler); }
	mbase::vector<logic_handler*>& get_logic_handler() { return mLogicHandler; }
protected:
	mbase::vector<logic_handler*> mLogicHandler;
};

MBASE_END

#endif // !MBASE_LOGICAL_PROCESSING_H
