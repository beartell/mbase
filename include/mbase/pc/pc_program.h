#ifndef MBASE_PC_PROGRAM
#define MBASE_PC_PROGRAM

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/framework/event_manager.h>
#include <mbase/framework/timer_loop.h>

MBASE_BEGIN

class PcConfig;
class PcIoManager;
class PcNetManager;
class PcDiagnostics;
class PcState;

struct MBASE_API PcProgramInformation {
	U16 mVersionMajor;
	U16 mVersionMinor;
	U16 mVersionPatch;
	mbase::string mProgramName;
	mbase::string mProgramVersion;
	mbase::string mProductId;
	mbase::string mProductName;
	mbase::string mProductAuthor;
	mbase::string mProductLicence;
	mbase::string mMbaseVersion;
};

class MBASE_API PcProgramBase : public non_copymovable {
public:
	using MBDate = U32; // placeholder: not a real date
	using size_type = SIZE_T;

	PcProgramBase();
	~PcProgramBase();

	PcConfig* get_config() noexcept;
	PcDiagnostics* get_diagnostics_manager() noexcept;
	PcIoManager* get_io_manager() noexcept;
	PcNetManager* get_net_manager() noexcept;
	PcProgramInformation get_program_info() const noexcept;
	event_manager* get_event_manager() noexcept;
	timer_loop* get_timer_loop() noexcept;
	bool is_running() const noexcept;
	bool is_initialized() const noexcept;

	GENERIC initialize(
		PcProgramInformation in_program_info,
		PcConfig* in_configurator = NULL, 
		PcDiagnostics* in_diagnostics = NULL,
		PcIoManager* in_io_manager = NULL,
		PcNetManager* in_net_manager = NULL,
		PcState* in_program_state = NULL
	);
	virtual GENERIC update() = 0;
	GENERIC update_defaults();
	bool halt();
	bool exit(I32 in_code, mbase::string in_message = "");

private:
	PcConfig* mConfig;
	PcDiagnostics* mDiagnostics;
	PcIoManager* mIoManager;
	PcNetManager* mNetManager;
	PcState* mProgramState;
	PcProgramInformation mProgramInfo;
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
