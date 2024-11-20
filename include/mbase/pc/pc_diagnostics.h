#ifndef MBASE_PC_DIAGNOSTICS_H
#define MBASE_PC_DIAGNOSTICS_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/behaviors.h>
#include <mbase/vector.h>
#include <mbase/synchronization.h>
#include <iostream>

MBASE_BEGIN

class MBASE_API PcDiagnostics {
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

	bool initialize(const mbase::string& in_diagnostics_name);
	flags log(flags in_log_type, flags in_log_importance, const mbase::string& in_message) noexcept;
	template<typename ... Params>
	flags log(flags in_log_type, flags in_log_importance, MSTRING in_format, Params ... in_params) noexcept
	{
		if (in_format == NULL || !mbase::string::length_bytes(in_format))
		{
			return flags::DIAGNOSTICS_ERR_MISSING_MESSAGE;
		}

		mbase::string totalLog = _build_log_heading(in_log_type, in_log_importance);

		totalLog += mbase::string::from_format(in_format, std::forward<Params>(in_params)..., MBASE_PLATFORM_NEWLINE);

		mLogList.push_back(totalLog);

		return flags::DIAGNOSTICS_SUCCESS;
	}
	GENERIC flush_logs() noexcept;
	GENERIC print_logs() const noexcept;
	GENERIC dump_logs_to_file() noexcept;
	GENERIC dump_logs_to_file(const mbase::wstring& in_file) noexcept;

private:
	mbase::string _build_log_heading(flags in_log_type, flags in_log_importance) noexcept;

	mbase::string mDiagnosticsName; // This will be the name of the log file on dump_logs_to_file
	log_list mLogList;
};

MBASE_END

#endif // !MBASE_PC_DIAGNOSTICS_H
