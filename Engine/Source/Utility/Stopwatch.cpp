#include "Utility/Stopwatch.h"

namespace ph
{

Stopwatch::Stopwatch() :
	m_startTime(), m_accumulatedDuration()// zero-initialization
{}

void Stopwatch::start()
{
	m_startTime = Clock::now();
	m_accumulatedDuration = Clock::duration();// zero-initialization
}

void Stopwatch::accumulatedStart()
{
	m_startTime = Clock::now();
}

void Stopwatch::stop()
{
	m_accumulatedDuration += Clock::now() - m_startTime;
}

uint64 Stopwatch::getDeltaS() const
{
	return getDelta<std::chrono::seconds>();
}

uint64 Stopwatch::getDeltaMs() const
{
	return getDelta<std::chrono::milliseconds>();
}

uint64 Stopwatch::getDeltaUs() const
{
	return getDelta<std::chrono::microseconds>();
}

uint64 Stopwatch::getDeltaNs() const
{
	return getDelta<std::chrono::nanoseconds>();
}

}// end namespace ph
