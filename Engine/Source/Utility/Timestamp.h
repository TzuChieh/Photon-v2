#pragma once

#include <string>
#include <chrono>
#include <mutex>

namespace ph
{

/*
	This class represents the time on which an instance of it has been created.
	It is guaranteed to be thread-safe.
*/
class Timestamp final
{
public:
	Timestamp();

	std::string toString() const;

private:
	std::chrono::system_clock::time_point m_time;

	typedef std::mutex Mutex;

	static Mutex& MUTEX();
};

}// end namespace ph
