#pragma once

#include "Math/math.h"
#include "Utility/TBitFlags.h"

#include <Common/primitive_type.h>

#include <type_traits>

namespace ph
{

/*! @brief Available surface phenomena.
*/
enum class ESurfacePhenomenon : uint32
{
	DiffuseReflection       = math::flag_bit<uint32, 0>(),
	NearDiffuseReflection   = math::flag_bit<uint32, 1>(),
	DeltaReflection         = math::flag_bit<uint32, 2>(),
	GlossyReflection        = math::flag_bit<uint32, 3>(),
	DiffuseTransmission     = math::flag_bit<uint32, 4>(),
	NearDiffuseTransmission = math::flag_bit<uint32, 5>(),
	DeltaTransmission       = math::flag_bit<uint32, 6>(),
	GlossyTransmission      = math::flag_bit<uint32, 7>()
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(ESurfacePhenomenon);

using SurfacePhenomena = TEnumFlags<ESurfacePhenomenon>;
using SurfaceElemental = int;

static_assert(std::is_signed_v<SurfaceElemental>, 
	"Type of `SurfaceElemental` must be signed, as special meanings are "
	"defined with negative values.");

/*! All available surface phenomena.
*/
inline constexpr ESurfacePhenomenon ALL_SURFACE_PHENOMENA = 
	static_cast<ESurfacePhenomenon>(~uint32(0));

/*! Surface phenomena with pure diffuse distribution.
*/
inline constexpr ESurfacePhenomenon DIFFUSE_SURFACE_PHENOMENA = 
	ESurfacePhenomenon::DiffuseReflection | ESurfacePhenomenon::DiffuseTransmission;

/*! Surface phenomena with pure delta distribution.
*/
inline constexpr ESurfacePhenomenon DELTA_SURFACE_PHENOMENA = 
	ESurfacePhenomenon::DeltaReflection | ESurfacePhenomenon::DeltaTransmission;

/*! Surface phenomena with pure glossy distribution.
*/
inline constexpr ESurfacePhenomenon GLOSSY_SURFACE_PHENOMENA = 
	ESurfacePhenomenon::GlossyReflection | ESurfacePhenomenon::GlossyTransmission;

/*! All available elementals.
*/
inline constexpr SurfaceElemental ALL_SURFACE_ELEMENTALS = -1;

class SurfaceOptics;

}// end namespace ph
