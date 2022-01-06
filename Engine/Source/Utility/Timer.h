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
	Timer();

	void start();
	void accumulatedStart();
	void stop();

	uint64 getDeltaS() const;
	uint64 getDeltaMs() const;
	uint64 getDeltaUs() const;
	uint64 getDeltaNs() const;

private:
	using Clock = std::chrono::steady_clock;

	Clock::time_point m_startTime;
	Clock::duration   m_accumulatedDuration;

	template<typename TimeUnit>
	inline uint64 getDelta() const
	{
		return static_cast<uint64>(
			std::chrono::duration_cast<TimeUnit>(m_accumulatedDuration).count());
	}
};

}// end namespace ph
