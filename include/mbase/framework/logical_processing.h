#ifndef MBASE_LOGICAL_PROCESSING_H
#define MBASE_LOGICAL_PROCESSING_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/thread.h>
#include <mbase/synchronization.h>
#include <mbase/behaviors.h>
#include <atomic>

MBASE_BEGIN

class processor_signal {
public:
	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE processor_signal() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE bool get_signal_state() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE bool get_signal() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE GENERIC set_signal_finished() noexcept;
	MBASE_INLINE GENERIC set_signal() noexcept;
	MBASE_INLINE GENERIC set_signal_state() noexcept;
	MBASE_INLINE GENERIC set_signal_with_state() noexcept;
	MBASE_INLINE GENERIC reset_signal() noexcept;
	MBASE_INLINE GENERIC reset_signal_state() noexcept;
	MBASE_INLINE GENERIC reset_signal_with_state() noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

private:
	volatile bool mSignalState = false;
	volatile bool mSignal = false;
};

class logical_processor : public mbase::non_copymovable {
public:
	logical_processor() : mProcessorThread(_update_t_static, this), mIsProcessorRunning(false) {}
	~logical_processor() 
	{ 
		stop_processor();
		mIsProcessorRunning = false;
	}

	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) bool is_processor_running() const noexcept { return mIsProcessorRunning; }

	static GENERIC _update_t_static(logical_processor* in_self) 
	{
		in_self->update_t();
		in_self->mIsProcessorRunning = false;
	}

	GENERIC start_processor() 
	{
		if(mIsProcessorRunning)
		{
			// if it is running, do nothing
			return;
		}
		else
		{
			stop_processor();
		}
		mIsProcessorRunning = true;
		mProcessorThread.run();
	}

	GENERIC stop_processor()
	{
		mIsProcessorRunning = false;
		mLogicSynchronizer.release();
		mProcessorThread.join(); // Internally checks the validity of the thread
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

MBASE_INLINE processor_signal::processor_signal() noexcept : mSignalState(false), mSignal(false) 
{
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE bool processor_signal::get_signal_state() const noexcept 
{ 
	return mSignalState; 
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE bool processor_signal::get_signal() const noexcept 
{ 
	return mSignal; 
}

MBASE_INLINE GENERIC processor_signal::set_signal_finished() noexcept
{
	mSignalState = true;
	mSignal = false;
}

MBASE_INLINE GENERIC processor_signal::set_signal() noexcept
{
	mSignal = true;
}

MBASE_INLINE GENERIC processor_signal::set_signal_state() noexcept
{
	mSignalState = true;
}

MBASE_INLINE GENERIC processor_signal::set_signal_with_state() noexcept
{
	mSignalState = true;
	mSignal = true;
}

MBASE_INLINE GENERIC processor_signal::reset_signal() noexcept
{
	mSignal = false;
}

MBASE_INLINE GENERIC processor_signal::reset_signal_state() noexcept
{
	mSignalState = false;
}

MBASE_INLINE GENERIC processor_signal::reset_signal_with_state() noexcept
{
	mSignalState = false;
	mSignal = false;
}

MBASE_END

#endif // !MBASE_LOGICAL_PROCESSING_H
