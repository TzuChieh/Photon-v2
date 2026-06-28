#pragma once

#include <Common/primitive_type.h>
#include <Engine/Math/hash.h>

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
		return ph::math::murmur3_32(flags, 0);
	}
};

}// end namespace std
