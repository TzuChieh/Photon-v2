#pragma once

#include "Engine/Math/math.h"
#include "Engine/Utility/TBitFlags.h"

#include <Common/primitive_type.h>

#include <type_traits>

namespace ph
{

namespace detail
{

using SurfacePhenomenonIntType = uint32;

inline constexpr auto sp_diffuse_reflection_bit        = math::flag_bit<SurfacePhenomenonIntType, 0>();
inline constexpr auto sp_near_diffuse_reflection_bit   = math::flag_bit<SurfacePhenomenonIntType, 1>();
inline constexpr auto sp_delta_reflection_bit          = math::flag_bit<SurfacePhenomenonIntType, 2>();
inline constexpr auto sp_glossy_reflection_bit         = math::flag_bit<SurfacePhenomenonIntType, 3>();
inline constexpr auto sp_diffuse_transmission_bit      = math::flag_bit<SurfacePhenomenonIntType, 4>();
inline constexpr auto sp_near_diffuse_transmission_bit = math::flag_bit<SurfacePhenomenonIntType, 5>();
inline constexpr auto sp_delta_transmission_bit        = math::flag_bit<SurfacePhenomenonIntType, 6>();
inline constexpr auto sp_glossy_transmission_bit       = math::flag_bit<SurfacePhenomenonIntType, 7>();

inline constexpr auto sp_all_bit = ~SurfacePhenomenonIntType{0};

}// end namespace detail

/*! @brief Available surface phenomena.
*/
enum class ESurfacePhenomenon : detail::SurfacePhenomenonIntType
{
	DiffuseReflection       = detail::sp_diffuse_reflection_bit,
	NearDiffuseReflection   = detail::sp_near_diffuse_reflection_bit,
	DeltaReflection         = detail::sp_delta_reflection_bit,
	GlossyReflection        = detail::sp_glossy_reflection_bit,
	DiffuseTransmission     = detail::sp_diffuse_transmission_bit,
	NearDiffuseTransmission = detail::sp_near_diffuse_transmission_bit,
	DeltaTransmission       = detail::sp_delta_transmission_bit,
	GlossyTransmission      = detail::sp_glossy_transmission_bit,

	/*! Surface phenomena with pure diffuse distribution. Near diffuse does not count as diffuse. */
	Diffuse = detail::sp_diffuse_reflection_bit | detail::sp_diffuse_transmission_bit,

	/*! Surface phenomena with near diffuse distribution. Does not include diffuse. */
	NearDiffuse = detail::sp_near_diffuse_reflection_bit | detail::sp_near_diffuse_transmission_bit,

	/*! Surface phenomena with pure delta distribution. */
	Delta = detail::sp_delta_reflection_bit | detail::sp_delta_transmission_bit,

	/*! Surface phenomena with pure glossy distribution. */
	Glossy = detail::sp_glossy_reflection_bit | detail::sp_glossy_transmission_bit,
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(ESurfacePhenomenon);

using SurfacePhenomena = TEnumFlags<ESurfacePhenomenon>;
using SurfaceElemental = int;

static_assert(std::is_signed_v<SurfaceElemental>, 
	"Type of `SurfaceElemental` must be signed, as special meanings are "
	"defined with negative values.");

/*! All available surface phenomena.
*/
inline constexpr auto ALL_SURFACE_PHENOMENA = SurfacePhenomena{{
	ESurfacePhenomenon::Diffuse,
	ESurfacePhenomenon::NearDiffuse,
	ESurfacePhenomenon::Delta,
	ESurfacePhenomenon::Glossy}};

/*! All available elementals.
*/
inline constexpr SurfaceElemental ALL_SURFACE_ELEMENTALS = -1;

class SurfaceOptics;

}// end namespace ph
