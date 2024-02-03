#pragma once

#include "Math/math.h"
#include "Utility/TBitFlags.h"

#include <Common/primitive_type.h>

namespace ph
{

enum class EFaceTopology : uint8
{
	General       = math::flag_bit<uint8, 0>(),
	Planar        = math::flag_bit<uint8, 1>(),
	Convex        = math::flag_bit<uint8, 2>(),
	Concave       = math::flag_bit<uint8, 3>(),
	Triangular    = math::flag_bit<uint8, 4>(),
	Quadrilateral = math::flag_bit<uint8, 5>()
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(EFaceTopology);

using FaceTopology = TEnumFlags<EFaceTopology>;

}// end namespace ph
