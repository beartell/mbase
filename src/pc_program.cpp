#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/pc/pc_io_manager.h>

MBASE_BEGIN

PcProgram::PcProgram()
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