#pragma once

#include <string>
#include <chrono>

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
	std::string toYMDHMS() const;
	std::string toYMDHMSMilliseconds() const;
	std::string toYMDHMSMicroseconds() const;
	std::string toString() const;

private:
	std::chrono::system_clock::time_point m_time;
};

// In-header Implementations:

inline Timestamp::Timestamp() :
	m_time(std::chrono::system_clock::now())
{}

}// end namespace ph
