#pragma once

#include <Common/primitive_type.h>

#include <chrono>
#include <ratio>

namespace ph
{

/*! @brief A timer. Measures relative time (not wall clock).
*/
class Timer final
{
public:
	using Clock = std::chrono::steady_clock;
	using DefaultTimeUnit = Clock::duration;

public:
	Timer();

	/*! @brief Zero and start the timer.
	*/
	Timer& start();

	/*! @brief Effectively resumes the timer.
	Start the timer by continuing last result from stop().
	*/
	Timer& accumulatedStart();

	/*! @brief Stop the timer and record elapsed time.
	*/
	Timer& stop();

	/*! @brief Get the time elapsed between calls to markLap().
	If this is the first markLap() call, start() and accumulatedStart() are treated as
	implicit lap marker.
	*/
	DefaultTimeUnit markLap();

	/*! @brief Get the time elapsed from last call to markLap().
	Get the result of markLap() as if it was called, without actually making a lap mark--
	the state of the timer is not affected in anyway by this call.
	*/
	DefaultTimeUnit peekLap() const;

	/*! @brief Get the time elapsed between start() and stop(). Possibly accumulated.
	Result may be accumulated if accumulatedStart() was used instead of start().
	*/
	DefaultTimeUnit getDelta() const;

	/*! @brief Variants that return time in plain number with different units and types.
	As casting a numeric value across different types, these methods may induce precision loss 
	depending on @p NumberType. Stick to base methods (the ones without time unit suffix) if 
	numeric precision is important.
	*/
	///@{
	template<typename NumberType = uint64>
	NumberType markLapS();

	template<typename NumberType = uint64>
	NumberType markLapMs();

	template<typename NumberType = uint64>
	NumberType markLapUs();

	template<typename NumberType = uint64>
	NumberType markLapNs();

	template<typename NumberType = uint64>
	NumberType getDeltaS() const;

	template<typename NumberType = uint64>
	NumberType getDeltaMs() const;

	template<typename NumberType = uint64>
	NumberType getDeltaUs() const;

	template<typename NumberType = uint64>
	NumberType getDeltaNs() const;
	///@}

private:
	template<typename TimeRepresentation, typename TimePeriod>
	static TimeRepresentation castToNumber(DefaultTimeUnit duration);

	Clock::time_point m_timeMark;
	DefaultTimeUnit   m_totalDuration;
};

inline Timer::Timer() :
	m_timeMark(), m_totalDuration(DefaultTimeUnit::zero())
{}

inline Timer& Timer::start()
{
	m_timeMark = Clock::now();
	m_totalDuration = DefaultTimeUnit::zero();

	return *this;
}

inline Timer& Timer::accumulatedStart()
{
	m_timeMark = Clock::now();

	return *this;
}

inline Timer& Timer::stop()
{
	m_totalDuration += Clock::now() - m_timeMark;

	return *this;
}

inline Timer::DefaultTimeUnit Timer::markLap()
{
	const auto currentTime = Clock::now();
	const auto lapDuration = currentTime - m_timeMark;

	m_timeMark = currentTime;
	m_totalDuration += lapDuration;

	return lapDuration;
}

inline Timer::DefaultTimeUnit Timer::peekLap() const
{
	const auto currentTime = Clock::now();
	const auto lapDuration = currentTime - m_timeMark;

	return lapDuration;
}

inline Timer::DefaultTimeUnit Timer::getDelta() const
{
	return m_totalDuration;
}

template<typename NumberType>
inline NumberType Timer::markLapS()
{
	return castToNumber<NumberType, std::ratio<1>>(markLap());
}

template<typename NumberType>
inline NumberType Timer::markLapMs()
{
	return castToNumber<NumberType, std::milli>(markLap());
}

template<typename NumberType>
inline NumberType Timer::markLapUs()
{
	return castToNumber<NumberType, std::micro>(markLap());
}

template<typename NumberType>
inline NumberType Timer::markLapNs()
{
	return castToNumber<NumberType, std::nano>(markLap());
}

template<typename NumberType>
inline NumberType Timer::getDeltaS() const
{
	return castToNumber<NumberType, std::ratio<1>>(getDelta());
}

template<typename NumberType>
inline NumberType Timer::getDeltaMs() const
{
	return castToNumber<NumberType, std::milli>(getDelta());
}

template<typename NumberType>
inline NumberType Timer::getDeltaUs() const
{
	return castToNumber<NumberType, std::micro>(getDelta());
}

template<typename NumberType>
inline NumberType Timer::getDeltaNs() const
{
	return castToNumber<NumberType, std::nano>(getDelta());
}

template<typename TimeRepresentation, typename TimePeriod>
inline TimeRepresentation Timer::castToNumber(const DefaultTimeUnit duration)
{
	using DstDuration = std::chrono::duration<TimeRepresentation, TimePeriod>;

	// We do not simply do `return DstDuration(duration);` since the implicit conversion will not
	// allow any precision loss. Here we want behavior similar to `static_cast`.
	return std::chrono::duration_cast<DstDuration>(duration).count();
}

}// end namespace ph
