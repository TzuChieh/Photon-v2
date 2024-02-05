#include "Common/Utility/Timestamp.h"
#include "Common/assertion.h"
#include "Common/os.h"

#include <format>
#include <mutex>
#include <sstream>

namespace ph
{

namespace
{

std::mutex& TIMESTAMP_MUTEX()
{
	static std::mutex mutex;
	return mutex;
}

}

// FIXME: Note that currently on 2021/10/07, there is an issue on get_tzdb_list() which will affect
// current_zone() (trigger system_error on older OSes and Windows 10 builds).
// See: https://github.com/microsoft/STL/issues/1911

std::string Timestamp::toYMD() const
{
	using namespace std::chrono;

	if(os::get_windows_version() == os::EWindowsVersion::Windows_10)
	{
		const zoned_time zonedTime{current_zone(), round<days>(m_time)};
		return std::format("{:%F}", zonedTime);
	}
	else
	{
		return toYMDWithOldAPI();
	}
}

std::string Timestamp::toHMS() const
{
	using namespace std::chrono;

	if(os::get_windows_version() == os::EWindowsVersion::Windows_10)
	{
		const zoned_time zonedTime{current_zone(), round<seconds>(m_time)};
		return std::format("{:%T}", zonedTime);
	}
	else
	{
		return toHMSWithOldAPI();
	}
}

std::string Timestamp::toYMDWithOldAPI() const
{
	const auto time = toCTime();

	std::stringstream stringBuilder;

	// Locks access to std::localtime(1) since it may cause data race
	{
		std::lock_guard<std::mutex> lock(TIMESTAMP_MUTEX());

		// FIXME: unsecure! need to disable secure warnings in order to use...
		stringBuilder << std::put_time(std::localtime(&time), "%F");
	}
		
	return stringBuilder.str();
}

std::string Timestamp::toHMSWithOldAPI() const
{
	const auto time = toCTime();

	std::stringstream stringBuilder;

	// Locks access to std::localtime(1) since it may cause data race
	{
		std::lock_guard<std::mutex> lock(TIMESTAMP_MUTEX());

		// FIXME: unsecure! need to disable secure warnings in order to use...
		stringBuilder << std::put_time(std::localtime(&time), "%T");
	}

	return stringBuilder.str();
}

std::string Timestamp::toHMSMilliseconds() const
{
	using namespace std::chrono;

	if(os::get_windows_version() == os::EWindowsVersion::Windows_10)
	{
		const zoned_time zonedTime{current_zone(), round<milliseconds>(m_time)};
		return std::format("{:%T}", zonedTime);
	}
	else
	{
		return toHMS();
	}
}

std::string Timestamp::toHMSMicroseconds() const
{
	using namespace std::chrono;

	if(os::get_windows_version() == os::EWindowsVersion::Windows_10)
	{
		const zoned_time zonedTime{current_zone(), round<microseconds>(m_time)};
		return std::format("{:%T}", zonedTime);
	}
	else
	{
		return toHMS();
	}
}

std::string Timestamp::toYMDHMS() const
{
	return std::format("{} {}", toYMD(), toHMS());
}

std::string Timestamp::toYMDHMSMilliseconds() const
{
	return std::format("{} {}", toYMD(), toHMSMilliseconds());
}

std::string Timestamp::toYMDHMSMicroseconds() const
{
	return std::format("{} {}", toYMD(), toHMSMicroseconds());
}

std::string Timestamp::toString() const
{
	return std::to_string(toCTime());
}

}// end namespace ph
