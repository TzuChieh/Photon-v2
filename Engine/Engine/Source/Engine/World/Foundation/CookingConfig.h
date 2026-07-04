#pragma once

#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/Quantity/TimeStep.h"
#include "Engine/Math/hash.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <functional>
#include <type_traits>

namespace ph
{

class CookingConfig final
{
public:
	uint64 preferTriangulated : 1 = 0;
	uint64 preferIndexedVertices : 1 = 0;

	/*! Force triangulated result. Generally this is handled by the base `Geometry` class and do
	not need to be explicitly handled in the derived classes. It is an error if triangulation 
	is not possible.
	*/
	uint64 forceTriangulated : 1 = 0;

	// TODO: prefer hidden emitter

	/*! @brief Time step for cooking time-dependent data.
	*/
	TimeStep timeStep;

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

	friend bool operator == (const CookingConfig& lhs, const CookingConfig& rhs) = default;
};

// Should be trivially copyable so there are no surprises when being hashed
static_assert(std::is_trivially_copyable_v<CookingConfig>);

}// end namespace ph

namespace std
{

template<>
struct hash<ph::CookingConfig>
{
	std::size_t operator () (const ph::CookingConfig& config) const
	{
		const ph::uint64 flags =
			(config.preferTriangulated << 0) |
			(config.preferIndexedVertices << 1) |
			(config.forceTriangulated << 2);

		std::size_t hash = ph::math::murmur3_32(flags, 0);
		hash = ph::math::combine_hashes(hash, std::hash<ph::TimeStep>{}(config.timeStep));
		return hash;
	}
};

}// end namespace std
