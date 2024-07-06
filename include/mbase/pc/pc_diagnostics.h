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
	template<typename ... Params>
	flags log(flags in_log_type, flags in_log_importance, MSTRING in_format, Params ... in_params) noexcept;
	GENERIC flush_logs() noexcept;
	GENERIC print_logs() const noexcept;
	flags dump_logs_to_file(const mbase::string& in_file) noexcept;

private:
	mbase::string _build_log_heading(flags in_log_type, flags in_log_importance) noexcept;

	log_list mLogList;
	mbase::mutex mLogMutex;
};

typename const PcDiagnostics::log_list& PcDiagnostics::get_log_list() const noexcept
{
	return mLogList;
}

bool PcDiagnostics::initialize()
{
	return true;
}

PcDiagnostics::flags PcDiagnostics::log(flags in_log_type, flags in_log_importance, const mbase::string& in_message) noexcept
{
	if(!in_message.size())
	{
		return flags::DIAGNOSTICS_ERR_MISSING_MESSAGE;
	}

	mbase::string totalLog = _build_log_heading(in_log_type, in_log_importance);
	totalLog += in_message + MBASE_PLATFORM_NEWLINE;

	mbase::lock_guard lockGuard(mLogMutex);
	mLogList.push_back(totalLog);

	return flags::DIAGNOSTICS_SUCCESS;
}

template<typename ... Params>
PcDiagnostics::flags PcDiagnostics::log(flags in_log_type, flags in_log_importance, MSTRING in_format, Params ... in_params) noexcept
{
	if(in_format == NULL || !mbase::string::length_bytes(in_format))
	{
		return flags::DIAGNOSTICS_ERR_MISSING_MESSAGE;
	}

	mbase::string totalLog = _build_log_heading(in_log_type, in_log_importance);

	totalLog += mbase::string::from_format(in_format, std::forward<Params>(in_params)...) + MBASE_PLATFORM_NEWLINE;

	mbase::lock_guard lockGuard(mLogMutex);
	mLogList.push_back(totalLog);

	return flags::DIAGNOSTICS_SUCCESS;
}

GENERIC PcDiagnostics::flush_logs() noexcept
{
	mLogList.clear();
}

GENERIC PcDiagnostics::print_logs() const noexcept
{
	for(log_list::const_iterator cIt = mLogList.cbegin(); cIt != mLogList.cend(); ++cIt)
	{
		printf(cIt->c_str());
	}
}

PcDiagnostics::flags PcDiagnostics::dump_logs_to_file(const mbase::string& in_file) noexcept
{
	return flags::DIAGNOSTICS_SUCCESS;
}

mbase::string PcDiagnostics::_build_log_heading(flags in_log_type, flags in_log_importance) noexcept
{
	mbase::string logHeading;
	switch (in_log_type)
	{
	case mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS:
		logHeading += "SUCCESS-";
		break;
	case mbase::PcDiagnostics::flags::LOGTYPE_ERROR:
		logHeading += "ERROR-";
		break;
	case mbase::PcDiagnostics::flags::LOGTYPE_WARNING:
		logHeading += "WARNING-";
		break;
	case mbase::PcDiagnostics::flags::LOGTYPE_INFO:
		logHeading += "INFO-";
		break;
	default:
		logHeading += "INFO-";
		break;
	}

	switch (in_log_importance)
	{
	case mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW:
		logHeading += "0 : ";
		break;
	case mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID:
		logHeading += "1 : ";
		break;
	case mbase::PcDiagnostics::flags::LOGIMPORTANCE_HIGH:
		logHeading += "2 : ";
		break;
	case mbase::PcDiagnostics::flags::LOGIMPORTANCE_FATAL:
		logHeading += "3 : ";
		break;
	default:
		logHeading += "0 : ";
		break;
	}

	return logHeading;
}

MBASE_END

#endif // !MBASE_PC_DIAGNOSTICS_H
