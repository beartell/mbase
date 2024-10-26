#include <mbase/pc/pc_diagnostics.h>
#include <mbase/io_file.h>

MBASE_BEGIN

typename const PcDiagnostics::log_list& PcDiagnostics::get_log_list() const noexcept
{
	return mLogList;
}

bool PcDiagnostics::initialize(const mbase::string& in_diagnostics_name)
{
	mDiagnosticsName = in_diagnostics_name;
	return true;
}

PcDiagnostics::flags PcDiagnostics::log(flags in_log_type, flags in_log_importance, const mbase::string& in_message) noexcept
{
	if (!in_message.size())
	{
		return flags::DIAGNOSTICS_ERR_MISSING_MESSAGE;
	}

	mbase::string totalLog = _build_log_heading(in_log_type, in_log_importance);
	totalLog += in_message + MBASE_PLATFORM_NEWLINE;

	mLogList.push_back(totalLog);

	return flags::DIAGNOSTICS_SUCCESS;
}

GENERIC PcDiagnostics::flush_logs() noexcept
{
	mLogList.clear();
}

GENERIC PcDiagnostics::print_logs() const noexcept
{
	for (log_list::const_iterator cIt = mLogList.cbegin(); cIt != mLogList.cend(); ++cIt)
	{
		printf(cIt->c_str());
	}
}

GENERIC PcDiagnostics::dump_logs_to_file() noexcept
{
	if (mLogList.size())
	{
		mbase::io_file iof;
		iof.open_file(mbase::from_utf8(mDiagnosticsName + ".txt"), mbase::io_file::access_mode::RW_ACCESS, mbase::io_file::disposition::OPEN);
		iof.set_file_pointer(0, mbase::io_base::move_method::MV_END);

		for(auto & n : mLogList)
		{
			iof.write_data(n);
		}

		mLogList.clear();
	}
}

GENERIC PcDiagnostics::dump_logs_to_file(const mbase::wstring& in_file) noexcept
{
	if (mLogList.size())
	{
		if(!in_file.size())
		{
			return;
		}
		mbase::io_file iof;
		iof.open_file(in_file, mbase::io_file::access_mode::RW_ACCESS, mbase::io_file::disposition::OPEN);
		iof.set_file_pointer(0, mbase::io_base::move_method::MV_END);

		for (auto& n : mLogList)
		{
			iof.write_data(n);
		}

		mLogList.clear();
	}
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