#pragma once

#include "Engine/Core/Quantity/Time.h"
#include "Engine/Math/hash.h"

#include <Common/compiler.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <functional>
#include <type_traits>

namespace ph
{

/*! @brief A time interval for cooking time-dependent data.
*/
class TimeStep final
{
public:
	/*! @brief A zero-duration step at time 0. */
	TimeStep() = default;

	/*! @brief Construct a time step.
	@param tick Logical index of the time step.
	@param tickStartS Start time of the step in seconds.
	@param stepSizeS Duration of the step in seconds.
	*/
	TimeStep(uint32 tick, real tickStartS, real stepSizeS)
		: m_tick(tick)
		, m_tickStartS(tickStartS)
		, m_stepSizeS(stepSizeS)
	{}

	/*! @brief Sample a time within this step.
	@param sample Parametric sample in the time step.
	@return The sampled absolute time and its parametric location.
	*/
	Time sampleTime(real sample) const
	{
		return Time(m_tickStartS + m_stepSizeS * sample, sample);
	}

	/*! @brief Whether this step spans a nonzero amount of time.
	*/
	bool hasDuration() const
	{
		return m_stepSizeS != 0;
	}

	/*! @brief Logical index of the time step.
	*/
	uint32 getTick() const
	{
		return m_tick;
	}

	/*! @brief Start time of the step in seconds.
	*/
	real getTickStartS() const
	{
		return m_tickStartS;
	}

	/*! @brief Duration of the step in seconds.
	*/
	real getStepSizeS() const
	{
		return m_stepSizeS;
	}

	bool operator == (const TimeStep& other) const = default;

#if !PH_COMPILER_HAS_P2468R2
	bool operator != (const TimeStep& other) const
	{
		return !(*this == other);
	}
#endif

private:
	uint32 m_tick = 0;
	real m_tickStartS = 0;
	real m_stepSizeS = 0;
};

static_assert(std::is_trivially_copyable_v<TimeStep>);

}// end namespace ph

namespace std
{

template<>
struct hash<ph::TimeStep>
{
	std::size_t operator () (const ph::TimeStep& timeStep) const
	{
		std::size_t hash = std::hash<ph::uint32>{}(timeStep.getTick());
		hash = ph::math::combine_hashes(hash, std::hash<ph::real>{}(timeStep.getTickStartS()));
		hash = ph::math::combine_hashes(hash, std::hash<ph::real>{}(timeStep.getStepSizeS()));
		return hash;
	}
};

}// end namespace std
