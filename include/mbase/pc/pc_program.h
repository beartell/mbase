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

	PcProgram();
	~PcProgram();

	PcConfig* get_config() noexcept;
	PcDiagnostics* get_diagnostics_manager() noexcept;
	PcIoManager* get_io_manager() noexcept;
	PcState* get_program_state() noexcept;
	const PcProgramInfo* get_program_info() const noexcept;
	event_manager* get_event_manager() noexcept;
	bool is_running() const noexcept;
	bool is_initialized() const noexcept;

	GENERIC initialize();
	bool authorize(mbase::string in_name, mbase::string in_password);
	bool update();
	bool halt();
	bool exit(I32 in_code, mbase::string in_message = "");

private:
	PcConfig* mConfig;
	PcDiagnostics* mDiagnostics;
	PcIoManager* mIoManager;
	PcProgramInfo mProgramInfo;
	PcState mState;
	event_manager mEventManager;
	bool mIsRunning;
	bool mIsAuthorized;
	bool mIsInitialized;
	MBDate mSessionTime;
	MBDate mLastSessionTime;
};

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

#endif // !MBASE_PC_PROGRAM
