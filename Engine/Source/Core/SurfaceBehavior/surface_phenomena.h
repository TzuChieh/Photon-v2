#pragma once

#include "Utility/TBitFlags.h"
#include "Common/primitive_type.h"

namespace ph
{

/*
	Available surface phenomena.
*/
enum class ESP : uint32
{
	DIFFUSE_REFLECTION    = uint32(1) << 0,
	SPECULAR_REFLECTION   = uint32(1) << 1,
	SPECULAR_TRANSMISSION = uint32(1) << 2,
	GLOSSY_REFLECTION     = uint32(1) << 3,
	GLOSSY_TRANSMISSION   = uint32(1) << 4
};

using SurfacePhenomena = TBitFlags<uint32, ESP>;

}// end namespace ph