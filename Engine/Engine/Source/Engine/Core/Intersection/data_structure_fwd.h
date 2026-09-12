#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

template<std::size_t N>
class TIndexedPolygonBuffer;

using IndexedTriangleBuffer = TIndexedPolygonBuffer<3>;
using IndexedQuadBuffer = TIndexedPolygonBuffer<4>;

/*! @brief Attribute identifiers for primitives.
*/
enum class EPrimitiveAttribute : uint8
{
	Position_0 = 0,
	Normal_0,
	Tangent_0,

	/*! @brief MikkTSpace-compatible tangent. Must use an `EAttributeElement` with custom bits.
	Custom bit 0 is the sign: 0 for +1 and 1 for -1.
	*/
	MikkTSpaceTangent_0,

	TexCoord_0,
	TexCoord_1,
	Color_0,
	Custom_0,
	Custom_1,
	Custom_2,
	Custom_3,

	// Special values
	SIZE
};

/*! @brief Indexing domains for primitive attributes.
*/
enum class EAttributeDomain : uint8
{
	Vertex = 0,
	Face,

	// Special values
	SIZE
};

}// end namespace ph
