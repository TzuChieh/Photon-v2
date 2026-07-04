#pragma once

#include <Common/compiler.h>
#include <Common/primitive_type.h>

#include <type_traits>

namespace ph
{

/*! @brief A sampled time within a time step.

Stores both the absolute time in seconds and the parametric sample location in
its source time step.
*/
class Time final
{
public:
	/*! @brief Linearly interpolate between two sampled times.
	*/
	static Time lerp(const Time& timeA, const Time& timeB, real parametricT)
	{
		const real oneMinusT = 1.0_r - parametricT;
		
		return Time(
			timeA.m_absoluteS * oneMinusT + timeB.m_absoluteS * parametricT,
			timeA.m_stepT * oneMinusT + timeB.m_stepT * parametricT);
	}

	Time() = default;

	/*! @brief Construct a sampled time.
	@param absoluteS Time in seconds from the global start.
	@param stepT Parametric sample in the source time step.
	*/
	Time(real absoluteS, real stepT)
		: m_absoluteS(absoluteS)
		, m_stepT(stepT)
	{}

	/*! @brief Get the time in seconds from the global start.
	*/
	real getAbsoluteS() const
	{
		return m_absoluteS;
	}

	/*! @brief Get the parametric sample in the source time step.
	*/
	real getStepT() const
	{
		return m_stepT;
	}

	bool operator == (const Time& other) const = default;

#if !PH_COMPILER_HAS_P2468R2
	bool operator != (const Time& other) const
	{
		return !(*this == other);
	}
#endif

private:
	real m_absoluteS = 0;
	real m_stepT = 0;
};

static_assert(std::is_trivially_copyable_v<Time>);

}// end namespace ph
