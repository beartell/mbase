#ifndef MBASE_PC_DIAGNOSTICS_H
#define MBASE_PC_DIAGNOSTICS_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/behaviors.h>
#include <mbase/vector.h>

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

	log_list& get_log_list() noexcept;

	bool initialize();
	flags log(flags in_log_type, const mbase::string& in_message) noexcept;
	flags flush_logs() noexcept;
	flags dump_logs_to_file(const mbase::string& in_file) noexcept;

private:
	log_list mLogList;
};

MBASE_END

#endif // !MBASE_PC_DIAGNOSTICS_H
