#include "Utility/Timestamp.h"
#include "Common/assertion.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace ph
{

Timestamp::Mutex& Timestamp::MUTEX()
{
	static Mutex mutex;
	return mutex;
}

Timestamp::Timestamp() : 
	m_time(std::chrono::system_clock::now())
{}

std::string Timestamp::toString() const
{
	const std::time_t time = std::chrono::system_clock::to_time_t(m_time);
	std::stringstream stringBuilder;

	// Locks the method since std::localtime(1) may cause data races.
	//
	std::lock_guard<Mutex> lock(MUTEX());

	stringBuilder << std::put_time(std::localtime(&time), "%X %Y-%m-%d");// FIXME: unsecure! need to disable secure warnings in order to use...
	return stringBuilder.str();
}

}// end namespace ph
