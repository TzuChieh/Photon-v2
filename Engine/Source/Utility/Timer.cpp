#include "Utility/Timer.h"

namespace ph
{

Timer::Timer() :
	m_startTime(), m_accumulatedDuration()// zero-initialization
{}

void Timer::start()
{
	m_startTime = Clock::now();
	m_accumulatedDuration = Clock::duration();// zero-initialization
}

void Timer::accumulatedStart()
{
	m_startTime = Clock::now();
}

void Timer::stop()
{
	m_accumulatedDuration += Clock::now() - m_startTime;
}

uint64 Timer::getDeltaS() const
{
	return getDelta<std::chrono::seconds>();
}

uint64 Timer::getDeltaMs() const
{
	return getDelta<std::chrono::milliseconds>();
}

uint64 Timer::getDeltaUs() const
{
	return getDelta<std::chrono::microseconds>();
}

uint64 Timer::getDeltaNs() const
{
	return getDelta<std::chrono::nanoseconds>();
}

}// end namespace ph
