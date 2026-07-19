#pragma once

#include "Engine/Math/hash.h"
#include "Engine/Math/TDecomposedTransform.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <functional>
#include <type_traits>

namespace ph
{

class GeometryCookingConfig final
{
public:
	uint64 preferTriangulated : 1 = 0;
	uint64 preferIndexedVertices : 1 = 0;

	/*! Force triangulated result. Generally this is handled by the base `Geometry` class and does
	not need to be explicitly handled in derived classes. It is an error if triangulation is not
	possible.
	*/
	uint64 forceTriangulated : 1 = 0;

	/*! Force `bakedTransform` to be baked into geometry data during cooking.
	It is an error if baking is unsupported. Vertex order is preserved and the resulting winding
	orientation is recorded in `CookedGeometry`.
	*/
	uint64 forceBakedTransform : 1 = 0;
	math::TDecomposedTransform<real> bakedTransform;

	friend bool operator == (const GeometryCookingConfig& lhs, const GeometryCookingConfig& rhs) = default;
};

// Should be trivially copyable so there are no surprises when being hashed
static_assert(std::is_trivially_copyable_v<GeometryCookingConfig>);

}// end namespace ph

namespace std
{

template<>
struct hash<ph::GeometryCookingConfig>
{
	std::size_t operator () (const ph::GeometryCookingConfig& config) const
	{
		const ph::uint64 flags =
			(config.preferTriangulated    << 0) |
			(config.preferIndexedVertices << 1) |
			(config.forceTriangulated     << 2) |
			(config.forceBakedTransform   << 3);

		std::size_t hash = ph::math::murmur3_32(flags, 0);
		return ph::math::combine_hashes(
			hash, std::hash<ph::math::TDecomposedTransform<ph::real>>{}(config.bakedTransform));
	}
};

}// end namespace std
