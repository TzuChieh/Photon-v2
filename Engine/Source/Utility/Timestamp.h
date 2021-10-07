#pragma once

#include <string>
#include <chrono>
#include <ctime>

namespace ph
{

/*! @brief Represents a point in time.
* 
This class represents the local time on which an instance of it has been created.
It is guaranteed to be thread-safe.
*/
class Timestamp final
{
public:
	Timestamp();

	std::string toYMD() const;
	std::string toHMS() const;
	std::string toHMSMilliseconds() const;
	std::string toHMSMicroseconds() const;
	std::string toYMDHMS() const;
	std::string toYMDHMSMilliseconds() const;
	std::string toYMDHMSMicroseconds() const;
	std::string toString() const;

private:
	std::chrono::system_clock::time_point m_time;

	std::time_t toCTime() const;
	std::string toYMDWithOldAPI() const;
	std::string toHMSWithOldAPI() const;
};

// In-header Implementations:

inline Timestamp::Timestamp() :
	m_time(std::chrono::system_clock::now())
{}

inline std::time_t Timestamp::toCTime() const
{
	return std::chrono::system_clock::to_time_t(m_time);
}

}// end namespace ph
