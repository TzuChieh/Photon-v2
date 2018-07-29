#pragma once

#include "Common/primitive_type.h"

#include <algorithm>
#include <cmath>

namespace ph
{
	
class RoughnessToAlphaMapping final
{
	// Maps roughness value in [0, 1] to alpha parameter used by many 
	// microfacet distribution functions. Generally, the mapping can be quite
	// beneficial for artists since directly consider roughness as alpha value 
	// often does not result in linearly varying specular highlight across
	// equally spaced roughness values.
	//
	// Strangely, the original paper: 
	// Microfacet Models for Refraction through Rough Surfaces by Walter et al. 
	// does not include such mapping for GGX distribution, only the ones for 
	// other kinds of distribution.

public:

	static inline real equaled(const real roughness)
	{
		return roughness;
	}

	// Mapping for a perceptually linear roughness.
	//
	// According to a course note in SIGGRAPH 2014:
	// Moving Frostbite to Physically Based Rendering 3.0, P.68,
	// they concluded that a squared mapping gives slightly better distribution 
	// of the profiles (blur amount) among all mip levels in the case of pre-
	// integrated diffuse IBL maps.
	//
	static inline real squared(const real roughness)
	{
		return roughness * roughness;
	}

	// This mapping is used in PBRT-v3.
	//
	static inline real pbrtV3(const real roughness)
	{
		const real clampedRoughness = std::max(roughness, 0.001_r);
		const real x                = std::log(clampedRoughness);

		return 1.621420000_r + 
		       0.819955000_r * x + 
		       0.173400000_r * x * x + 
		       0.017120100_r * x * x * x + 
		       0.000640711_r * x * x * x * x;
	}
};

}// end namespace ph