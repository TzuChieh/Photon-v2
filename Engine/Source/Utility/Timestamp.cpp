#include "Utility/Timestamp.h"
#include "Common/assertion.h"

#include <format>

namespace ph
{

std::string Timestamp::toYMD() const
{
	using namespace std::chrono;

	const zoned_time zonedTime{current_zone(), round<days>(m_time)};
	return std::format("%F", zonedTime);
}

std::string Timestamp::toHMS() const
{
	using namespace std::chrono;

	const zoned_time zonedTime{current_zone(), round<seconds>(m_time)};
	return std::format("%T", zonedTime);
}

std::string Timestamp::toYMDHMS() const
{
	using namespace std::chrono;

	const zoned_time zonedTime{current_zone(), round<seconds>(m_time)};
	return std::format("%F %T", zonedTime);
}

std::string Timestamp::toYMDHMSMilliseconds() const
{
	using namespace std::chrono;

	const zoned_time zonedTime{current_zone(), round<milliseconds>(m_time)};
	return std::format("%F %T", zonedTime);
}

std::string Timestamp::toYMDHMSMicroseconds() const
{
	using namespace std::chrono;

	const zoned_time zonedTime{current_zone(), round<microseconds>(m_time)};
	return std::format("%F %T", zonedTime);
}

std::string Timestamp::toString() const
{
	return toYMDHMS();
}

}// end namespace ph
