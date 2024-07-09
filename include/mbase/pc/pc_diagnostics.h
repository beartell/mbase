#ifndef MBASE_PC_DIAGNOSTICS_H
#define MBASE_PC_DIAGNOSTICS_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/behaviors.h>
#include <mbase/vector.h>
#include <mbase/synchronization.h>
#include <iostream>

MBASE_BEGIN

class PcDiagnostics : public mbase::singleton<PcDiagnostics> {
public:
	using log_list = mbase::vector<mbase::string>;

	enum class flags : U8 {
		DIAGNOSTICS_SUCCESS = 0,
		LOGTYPE_SUCCESS,
		LOGTYPE_ERROR,
		LOGTYPE_WARNING,
		LOGTYPE_INFO,
		LOGIMPORTANCE_LOW,
		LOGIMPORTANCE_MID,
		LOGIMPORTANCE_HIGH,
		LOGIMPORTANCE_FATAL,
		DIAGNOSTICS_ERR_MISSING_MESSAGE,
		DIAGNOSTICS_ERR_MISSING_FILE
	};

	PcDiagnostics() = default;
	~PcDiagnostics() = default;

	const log_list& get_log_list() const noexcept;

	bool initialize();
	flags log(flags in_log_type, flags in_log_importance, const mbase::string& in_message) noexcept;
	//template<typename ... Params>
	//flags log(flags in_log_type, flags in_log_importance, MSTRING in_format, Params ... in_params) noexcept;
	GENERIC flush_logs() noexcept;
	GENERIC print_logs() const noexcept;
	flags dump_logs_to_file(const mbase::string& in_file) noexcept;

private:
	mbase::string _build_log_heading(flags in_log_type, flags in_log_importance) noexcept;

	log_list mLogList;
	mbase::mutex mLogMutex;
};



MBASE_END

#endif // !MBASE_PC_DIAGNOSTICS_H
