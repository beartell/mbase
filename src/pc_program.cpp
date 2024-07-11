#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/pc/pc_io_manager.h>
#include <mbase/pc/pc_net_manager.h>

MBASE_BEGIN

PcProgram::PcProgram() :
	mConfig(NULL),
	mDiagnostics(NULL),
	mIoManager(NULL),
	mNetManager(NULL),
	mIsRunning(false),
	mIsAuthorized(false),
	mIsInitialized(false),
	mSessionTime(0),
	mLastSessionTime(0)
{

}

PcProgram::~PcProgram()
{

}

PcConfig* PcProgram::get_config() noexcept
{
	return mConfig;
}

PcDiagnostics* PcProgram::get_diagnostics_manager() noexcept
{
	return mDiagnostics;
}

PcIoManager* PcProgram::get_io_manager() noexcept
{
	return mIoManager;
}

PcNetManager* PcProgram::get_net_manager() noexcept
{
	return mNetManager;
}

PcState* PcProgram::get_program_state() noexcept
{
	return &mState;
}

const PcProgramInfo* PcProgram::get_program_info() const noexcept
{
	return &mProgramInfo;
}

event_manager* PcProgram::get_event_manager() noexcept
{
	return &mEventManager;
}

timer_loop* PcProgram::get_timer_loop() noexcept
{
	return &mTimerLoop;
}

bool PcProgram::is_running() const noexcept
{
	return mIsRunning;
}

bool PcProgram::is_initialized() const noexcept
{
	return mIsInitialized;
}

GENERIC PcProgram::initialize()
{
	mDiagnostics = &PcDiagnostics::get_instance();
	mConfig = &PcConfig::get_instance();
	mIoManager = &PcIoManager::get_instance();
	mNetManager = &PcNetManager::get_instance();

	mDiagnostics->initialize();
	mConfig->initialize();
	mIoManager->initialize();
}

bool PcProgram::authorize(mbase::string in_name, mbase::string in_password)
{
	return false;
}

bool PcProgram::update()
{
	mTimerLoop.run_timers();
	return false;
}

bool PcProgram::halt()
{
	return false;
}

bool PcProgram::exit(I32 in_code, mbase::string in_message)
{
	return false;
}


MBASE_END