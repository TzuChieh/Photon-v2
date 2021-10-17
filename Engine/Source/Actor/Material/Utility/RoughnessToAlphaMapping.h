#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/Utility/ERoughnessToAlpha.h"

#include <algorithm>
#include <cmath>

namespace ph
{

/*! @brief Maps roughness value in [0, 1] to the alpha parameter in NDF.

Maps roughness value in [0, 1] to alpha parameter used by many , microfacet
normal distribution functions. Generally, the mapping can be quite beneficial
for artists since directly consider roughness as alpha value often does not 
result in linearly varying specular highlight across equally spaced roughness 
values. Strangely, the original paper: Microfacet Models for Refraction through
Rough Surfaces by Walter et al. does not include such mapping for GGX 
distribution, only the ones for other kinds of distribution.
*/
class RoughnessToAlphaMapping final
{
public:
	/*! @brief Directly assign roughness value as-is to alpha.
	*/
	static real equaled(const real roughness);

	/*! @brief Mapping for a perceptually linear roughness.

	According to a course note in SIGGRAPH 2014: Moving Frostbite to Physically
	Based Rendering 3.0, P.68, hey concluded that a squared mapping gives 
	slightly better distribution of the profiles (blur amount) among all mip
	levels in the case of pre-integrated diffuse IBL maps.
	*/
	static real squared(const real roughness);

	/*! @brief This mapping is used in PBRT-v3.
	*/
	static real pbrtV3(const real roughness);

	/*! @brief Map roughness value to alpha based on map type.
	*/
	static real map(const real roughness, ERoughnessToAlpha mapType);
};

// In-header Implementations:

inline real RoughnessToAlphaMapping::equaled(const real roughness)
{
	return roughness;
}

inline real RoughnessToAlphaMapping::squared(const real roughness)
{
	return roughness * roughness;
}

inline real RoughnessToAlphaMapping::pbrtV3(const real roughness)
{
	const real clampedRoughness = std::max(roughness, 0.001_r);
	const real x                = std::log(clampedRoughness);

	return 1.621420000_r + 
	       0.819955000_r * x + 
	       0.173400000_r * x * x + 
	       0.017120100_r * x * x * x + 
	       0.000640711_r * x * x * x * x;
}

inline real RoughnessToAlphaMapping::map(const real roughness, const ERoughnessToAlpha mapType)
{
	switch(mapType)
	{
	case ERoughnessToAlpha::Equaled:
		return equaled(roughness);

	case ERoughnessToAlpha::Squared:
		return squared(roughness);

	case ERoughnessToAlpha::PbrtV3:
		return pbrtV3(roughness);

	default:
		return squared(roughness);
	}
}

}// end namespace ph
