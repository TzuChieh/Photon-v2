#pragma once

#include "Utility/TBitFlags.h"

#include <Common/primitive_type.h>

#include <type_traits>

namespace ph
{

/*! @brief Available surface phenomena.
*/
enum class ESurfacePhenomenon : uint32
{
	DiffuseReflection  = uint32(1) << 0,
	DeltaReflection    = uint32(1) << 1,
	GlossyReflection   = uint32(1) << 2,
	DeltaTransmission  = uint32(1) << 3,
	GlossyTransmission = uint32(1) << 4
};

enum class ETransport
{
	Radiance,
	Importance
};

using SurfacePhenomena = TBitFlags<uint32, ESurfacePhenomenon>;
using SurfaceElemental = int;

static_assert(std::is_signed_v<SurfaceElemental>, 
	"Type of SurfaceElemental must be signed, as special meanings are "
	"defined with negative values.");

inline constexpr SurfaceElemental ALL_ELEMENTALS = -1;

class SurfaceOptics;

}// end namespace ph
