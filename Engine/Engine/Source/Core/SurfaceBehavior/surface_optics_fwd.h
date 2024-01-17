#pragma once

#include "Utility/TBitFlags.h"
#include "Math/math.h"

#include <Common/primitive_type.h>

#include <type_traits>

namespace ph
{

/*! @brief Available surface phenomena.
*/
enum class ESurfacePhenomenon : uint32
{
	DiffuseReflection  = math::flag_bit<uint32, 0>(),
	DeltaReflection    = math::flag_bit<uint32, 1>(),
	GlossyReflection   = math::flag_bit<uint32, 2>(),
	DeltaTransmission  = math::flag_bit<uint32, 3>(),
	GlossyTransmission = math::flag_bit<uint32, 4>()
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(ESurfacePhenomenon);

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

/*! Targets all available surface phenomena.
*/
inline constexpr ESurfacePhenomenon ALL_SURFACE_PHENOMENA = 
	static_cast<ESurfacePhenomenon>(~uint32(0));

/*! Targets surface phenomena with delta distribution.
*/
inline constexpr ESurfacePhenomenon DELTA_SURFACE_PHENOMENA = 
	ESurfacePhenomenon::DeltaReflection | ESurfacePhenomenon::DeltaTransmission;

/*! Targets all available elementals.
*/
inline constexpr SurfaceElemental ALL_SURFACE_ELEMENTALS = -1;

class SurfaceOptics;

}// end namespace ph
