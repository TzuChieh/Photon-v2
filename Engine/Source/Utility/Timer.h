#pragma once

#include "Common/primitive_type.h"

#include <chrono>

namespace ph
{

// TODO: add a high resolution timer for profiling
class Timer final
{
public:
	void start();
	void finish();

	uint64 getDeltaS() const;
	uint64 getDeltaMs() const;
	uint64 getDeltaUs() const;
	uint64 getDeltaNs() const;

private:
	using Clock = std::chrono::steady_clock;

	Clock::time_point m_startTime;
	Clock::time_point m_finishTime;

	template<typename TimeUnit>
	inline uint64 getDelta() const
	{
		const auto& duration = m_finishTime - m_startTime;
		const auto& delta    = std::chrono::duration_cast<TimeUnit>(duration).count();
		return static_cast<uint64>(delta);
	}
};

}// end namespace ph