#pragma once

#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/Quantity/TimeStep.h"

#include <cstddef>
#include <functional>
#include <type_traits>

namespace ph
{

class CommonCookingConfig final
{
public:
	/*! @brief Time step for cooking time-dependent data.
	*/
	TimeStep timeStep;

	// TODO: prefer hidden emitter

	/*! @brief Get the start time of @ref timeStep.
	*/
	Time getTimeStepStart() const
	{
		return timeStep.sampleTime(0);
	}

	/*! @brief Get the end time of @ref timeStep.
	*/
	Time getTimeStepEnd() const
	{
		return timeStep.sampleTime(1);
	}

	friend bool operator == (const CommonCookingConfig& lhs, const CommonCookingConfig& rhs) = default;
};

// Should be trivially copyable so there are no surprises when being hashed
static_assert(std::is_trivially_copyable_v<CommonCookingConfig>);

}// end namespace ph

namespace std
{

template<>
struct hash<ph::CommonCookingConfig>
{
	std::size_t operator () (const ph::CommonCookingConfig& config) const
	{
		return std::hash<ph::TimeStep>{}(config.timeStep);
	}
};

}// end namespace std
