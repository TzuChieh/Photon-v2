#include "Utility/Timer.h"

namespace ph
{

void Timer::start()
{
	m_startTime = std::chrono::high_resolution_clock::now();
}

void Timer::finish()
{
	m_finishTime = std::chrono::high_resolution_clock::now();
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