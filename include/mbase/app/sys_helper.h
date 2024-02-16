#ifndef MBASE_SYSINFO_H
#define MBASE_SYSINFO_H

#include <mbase/common.h>
#include <sysinfoapi.h>

MBASE_BEGIN


namespace sys
{
	struct sys_time {
		sys_time() : 
			sysYear(0),
			sysMonth(0),
			sysWeek(0),
			sysDay(0),
			sysHour(0),
			sysMinute(0),
			sysSecond(0),
			sysMillisecond(0)
		{}

		sys_time(const SYSTEMTIME& in_rhs) {
			sysYear = in_rhs.wYear;
			sysMonth = in_rhs.wMonth;
			sysWeek = in_rhs.wDayOfWeek;
			sysDay = in_rhs.wDay;
			sysHour = in_rhs.wHour;
			sysMinute = in_rhs.wMinute;
			sysSecond = in_rhs.wSecond;
			sysMillisecond = in_rhs.wMilliseconds;
		}

		sys_time& operator=(const SYSTEMTIME& in_rhs) {
			sysYear = in_rhs.wYear;
			sysMonth = in_rhs.wMonth;
			sysWeek = in_rhs.wDayOfWeek;
			sysDay = in_rhs.wDay;
			sysHour = in_rhs.wHour;
			sysMinute = in_rhs.wMinute;
			sysSecond = in_rhs.wSecond;
			sysMillisecond = in_rhs.wMilliseconds;

			return *this;
		}

		I32 sysYear;
		I32 sysMonth;
		I32 sysWeek;
		I32 sysDay;
		I32 sysHour;
		I32 sysMinute;
		I32 sysSecond;
		I32 sysMillisecond;
	};
	
	USED_RETURN sys_time get_system_time() noexcept {
		SYSTEMTIME sysTime;
		GetSystemTime(&sysTime);
		return sysTime;
	}

	USED_RETURN sys_time get_local_time() noexcept {
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		return sysTime;
	}
}

MBASE_END

#endif // !MBASE_SYSINFO_H
