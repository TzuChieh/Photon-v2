#pragma once

#include <Common/primitive_type.h>
#include <Math/math.h>
#include <Utility/TBitFlags.h>

namespace ph::editor
{

enum class EClearTarget : uint32f
{
	None    = math::flag_bit<uint32f, 0>(),
	Color   = math::flag_bit<uint32f, 1>(),
	Depth   = math::flag_bit<uint32f, 2>(),
	Stencil = math::flag_bit<uint32f, 3>()
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(EClearTarget);

}// end namespace ph::editor
