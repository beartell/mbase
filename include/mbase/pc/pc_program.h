#ifndef MBASE_PC_PROGRAM
#define MBASE_PC_PROGRAM

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/framework/event_manager.h>
#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/pc/pc_io_manager.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_termination_handler.h>

MBASE_BEGIN

struct PcProgramInfo {
	mbase::string mProgramName;
	mbase::string mProgramVersion;
	mbase::string mMbaseVersion;
	mbase::string mProductId;
	mbase::string mUserName;
	mbase::string mPassword;
	I32 mProgramVersionInt;
};

class PcProgram : public mbase::singleton<PcProgram> {
public:
	using MBDate = U32; // placeholder: not a real date

	PcConfig* get_config() noexcept;
	PcDiagnostics* get_diagnostics_manager() noexcept;
	PcIoManager* get_io_manager() noexcept;
	PcState* get_state_manager() noexcept;
	const PcProgramInfo* get_program_info() const noexcept;
	event_manager* get_event_manager() noexcept;
	bool is_running() const noexcept;
	bool is_initialized() const noexcept;

	GENERIC initialize();
	bool authorize(mbase::string in_name, mbase::string in_password);
	bool run();
	bool halt();
	bool exit(I32 in_code, mbase::string in_message = "");

private:
	PcConfig* mConfig;
	PcDiagnostics* mDiagnostics;
	PcIoManager* mIoManager;
	PcState* mState;
	PcProgramInfo mProgramInfo;
	event_manager mEventManager;
	bool mIsRunning;
	bool mIsAuthorized;
	MBDate mSessionTime;
	MBDate mLastSessionTime;
};

PcConfig* PcProgram::get_config() noexcept
{
	return nullptr;
}

PcDiagnostics* PcProgram::get_diagnostics_manager() noexcept
{
	return nullptr;
}

PcIoManager* PcProgram::get_io_manager() noexcept
{
	return nullptr;
}

PcState* PcProgram::get_state_manager() noexcept
{
	return nullptr;
}

const PcProgramInfo* PcProgram::get_program_info() const noexcept
{
	return nullptr;
}

event_manager* PcProgram::get_event_manager() noexcept
{
	return nullptr;
}

bool PcProgram::is_running() const noexcept
{
	return false;
}

bool PcProgram::is_initialized() const noexcept
{
	return false;
}

GENERIC PcProgram::initialize()
{
	mDiagnostics = &PcDiagnostics::get_instance();
	mConfig = &PcConfig::get_instance();
	mState = &PcState::get_instance();
	mIoManager = &PcIoManager::get_instance();

	mDiagnostics->initialize();
	mConfig->initialize();
	mState->initialize();
	mIoManager->initialize();
}

bool PcProgram::authorize(mbase::string in_name, mbase::string in_password)
{
	return false;
}

bool PcProgram::run()
{
	return false;
}

bool PcProgram::halt()
{
	return false;
}

bool PcProgram::exit(I32 in_code, mbase::string in_message = "")
{
	return false;
}

MBASE_END

#endif // !MBASE_PC_PROGRAM
