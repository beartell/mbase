#ifndef MBASE_PC_PROGRAM
#define MBASE_PC_PROGRAM

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/framework/event_manager.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_termination_handler.h>

#define MBASE_PROGRAM_INVOKE_EVENT(in_event_name) PcProgram::get_instance().get_event_manager()->dispatch_event(in_event_name, nullptr)
#define MBASE_PROGRAM_INVOKE_EVENT_WDATA(in_event_name, in_data) PcProgram::get_instance().get_event_manager()->dispatch_event(in_event_name, (mbase::event_manager::user_data)in_data)

MBASE_BEGIN

class PcConfig;
class PcDiagnostics;
class PcIoManager;

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

MBASE_END

#endif // !MBASE_PC_PROGRAM
