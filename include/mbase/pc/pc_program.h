#ifndef MBASE_PC_PROGRAM
#define MBASE_PC_PROGRAM

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/framework/event_manager.h>
#include <mbase/framework/timer_loop.h>
#include <mbase/pc/pc_state.h>
#include <mbase/pc/pc_termination_handler.h>

#define MBASE_PROGRAM_INVOKE_EVENT(in_event_name) mbase::PcProgram::get_instance().get_event_manager()->dispatch_event(in_event_name, nullptr)
#define MBASE_PROGRAM_INVOKE_EVENT_WDATA(in_event_name, in_data) mbase::PcProgram::get_instance().get_event_manager()->dispatch_event(in_event_name, (mbase::event_manager::user_data)in_data)

#define MBASE_PROGRAM_DIAGNOSTICS_MANAGER() mbase::PcProgram::get_instance().get_diagnostics_manager();
#define MBASE_PROGRAM_IO_MANAGER() mbase::PcProgram::get_instance().get_io_manager();
#define MBASE_PROGRAM_STATE() mbase::PcProgram::get_instance().get_program_state();
#define MBASE_PROGRAM_EVENT_MANAGER() mbase::PcProgram::get_instance().get_event_manager();
#define MBASE_PROGRAM_TIMER() mbase::PcProgram::get_instance().get_timer_loop();
#define MBASE_PROGRAM_NET_MANAGER() mbase::PcProgram::get_instance().get_net_manager();
#define MBASE_PROGRAM_CONFIG_MANAGER() mbase::PcProgram::get_instance().get_config();

MBASE_BEGIN

class PcConfig;
class PcDiagnostics;
class PcIoManager;
class PcNetManager;

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
	using size_type = SIZE_T;

	PcProgram();
	~PcProgram();

	PcConfig* get_config() noexcept;
	PcDiagnostics* get_diagnostics_manager() noexcept;
	PcIoManager* get_io_manager() noexcept;
	PcNetManager* get_net_manager() noexcept;
	const PcProgramInfo* get_program_info() const noexcept;
	event_manager* get_event_manager() noexcept;
	timer_loop* get_timer_loop() noexcept;
	bool is_running() const noexcept;
	bool is_initialized() const noexcept;
	bool get_program_data(const mbase::string& in_key, char_stream& out_data);

	GENERIC initialize();
	bool authorize(mbase::string in_name, mbase::string in_password);
	bool update();
	bool halt();
	bool exit(I32 in_code, mbase::string in_message = "");
	GENERIC set_program_data(const mbase::string& in_key, CBYTEBUFFER in_data, size_type in_size);
	GENERIC set_program_data(const mbase::string& in_key, char_stream& in_data);

private:
	PcConfig* mConfig;
	PcDiagnostics* mDiagnostics;
	PcIoManager* mIoManager;
	PcNetManager* mNetManager;
	PcProgramInfo mProgramInfo;
	PcState mState;
	event_manager mEventManager;
	timer_loop mTimerLoop;
	bool mIsRunning;
	bool mIsAuthorized;
	bool mIsInitialized;
	MBDate mSessionTime;
	MBDate mLastSessionTime;
};

MBASE_END

#endif // !MBASE_PC_PROGRAM
