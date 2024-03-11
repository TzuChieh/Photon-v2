#pragma once

#include "Math/math.h"
#include "Utility/TBitFlags.h"

#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Categorizing face topology.
*/
enum class EFaceTopology : uint8
{
	/*! Basically cannot assume anything about the face. */
	General = math::flag_bit<uint8, 0>(),

	/*! Flat face. */
	Planar = math::flag_bit<uint8, 1>(),

	/*! A bump into the hemisphere of face normal. */
	Convex = math::flag_bit<uint8, 2>(),

	/*! A dent into the opposite hemisphere of face normal. */
	Concave = math::flag_bit<uint8, 3>(),

	/*! The face is a triangle. */
	Triangular = math::flag_bit<uint8, 4>(),

	/*! The face is a quadrilateral. */
	Quadrilateral = math::flag_bit<uint8, 5>()
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(EFaceTopology);

using FaceTopology = TEnumFlags<EFaceTopology>;

}// end namespace ph
