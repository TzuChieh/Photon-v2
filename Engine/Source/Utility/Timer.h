#pragma once

#include "Common/primitive_type.h"

#include <chrono>

namespace ph
{

/*! @brief A timer. Measures relative time (not wall clock).
*/
class Timer final
{
public:
	using Clock = std::chrono::steady_clock;

public:
	Timer();

	/*! @brief Zero and start the timer.
	*/
	void start();

	/*! @brief Effectively resumes the timer.
	Start the timer by continuing last result from stop().
	*/
	void accumulatedStart();

	/*! @brief Stop the timer and record elapsed time.
	*/
	void stop();

	/*! @brief Get the time elapsed between calls to markLap().
	If this is the first markLap() call, start() and accumulatedStart() are treated as
	implicit lap marker.
	*/
	Clock::duration markLap();

	/*! @brief Get the time elapsed from last call to markLap().
	Get the result of markLap() as if it was called, without actually making a lap mark--
	the state of the timer is not affected in anyway by this call.
	*/
	Clock::duration peekLap() const;

	/*! @brief Get the time elapsed between start() and stop(). Possibly accumulated.
	Result may be accumulated if accumulatedStart() was used instead of start().
	*/
	Clock::duration getDelta() const;

	/*! @brief Variants that return time in plain number with different units.
	*/
	///@{
	uint64 markLapS();
	uint64 markLapMs();
	uint64 markLapUs();
	uint64 markLapNs();
	uint64 getDeltaS() const;
	uint64 getDeltaMs() const;
	uint64 getDeltaUs() const;
	uint64 getDeltaNs() const;
	///@}

private:
	Clock::time_point m_timeMark;
	Clock::duration   m_totalDuration;

	template<typename NumberUnit>
	static uint64 toNumber(Clock::duration duration);
};

inline Timer::Timer() :
	m_timeMark(), m_totalDuration(Clock::duration::zero())
{}

inline void Timer::start()
{
	m_timeMark = Clock::now();
	m_totalDuration = Clock::duration::zero();
}

inline void Timer::accumulatedStart()
{
	m_timeMark = Clock::now();
}

inline void Timer::stop()
{
	m_totalDuration += Clock::now() - m_timeMark;
}

inline Timer::Clock::duration Timer::markLap()
{
	const auto currentTime = Clock::now();
	const auto lapDuration = currentTime - m_timeMark;

	m_timeMark = currentTime;
	m_totalDuration += lapDuration;

	return lapDuration;
}

inline Timer::Clock::duration Timer::peekLap() const
{
	const auto currentTime = Clock::now();
	const auto lapDuration = currentTime - m_timeMark;

	return lapDuration;
}

inline Timer::Clock::duration Timer::getDelta() const
{
	return m_totalDuration;
}

inline uint64 Timer::markLapS()
{
	return toNumber<std::chrono::seconds>(markLap());
}

inline uint64 Timer::markLapMs()
{
	return toNumber<std::chrono::milliseconds>(markLap());
}

inline uint64 Timer::markLapUs()
{
	return toNumber<std::chrono::microseconds>(markLap());
}

inline uint64 Timer::markLapNs()
{
	return toNumber<std::chrono::nanoseconds>(markLap());
}

inline uint64 Timer::getDeltaS() const
{
	return toNumber<std::chrono::seconds>(getDelta());
}

inline uint64 Timer::getDeltaMs() const
{
	return toNumber<std::chrono::milliseconds>(getDelta());
}

inline uint64 Timer::getDeltaUs() const
{
	return toNumber<std::chrono::microseconds>(getDelta());
}

inline uint64 Timer::getDeltaNs() const
{
	return toNumber<std::chrono::nanoseconds>(getDelta());
}

template<typename NumberUnit>
inline uint64 Timer::toNumber(const Clock::duration duration)
{
	return static_cast<uint64>(
		std::chrono::duration_cast<NumberUnit>(duration).count());
}

}// end namespace ph
