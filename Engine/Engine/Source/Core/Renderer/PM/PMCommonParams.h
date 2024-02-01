#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

/*! @brief Common parameters of photon mapping.
*/
class PMCommonParams final
{
public:
	inline static constexpr uint32 DEFAULT_MAX_PATH_LENGTH = 16384;

	/*! Number of photons used. For progressive techniques, this value is for a single pass.
	*/
	std::size_t numPhotons = 200000;

	/*! Number of passes performed by progressive techniques.
	*/
	std::size_t numPasses = 1;

	/*! Number of samples per pixel. Higher values can resolve image aliasing, but can consume large
	amounts of memory for some algorithms. This value can also mean the number of statistics gathered
	in a single pixel for some techniques. If the value is not a power-of-2 number, it may be adjusted.
	*/
	std::size_t numSamplesPerPixel = 4;

	/*! Energy contribution radius for each photon. For progressive techniques, this value is for
	setting up the initial radius.
	*/
	real kernelRadius = 0.1_r;

	/*! Minimum path length for a photon. Photon with 0 path length is not supported, as it is like
	approximating emissive surface with photons and the result is overly blurry for no good reason.
	0-bounce lighting is done via traditional path sampling.
	*/
	uint32 minPhotonPathLength = 1;

	/*! Maximum path length for a photon. The default value is effectively infinite bounces.
	Combined with `minPhotonPathLength`, these parameters are useful if only some number of bounces
	are estimated via photons.
	*/
	uint32 maxPhotonPathLength = DEFAULT_MAX_PATH_LENGTH;

	/*! Controls the kernel radius reduction rate for progressive techniques. Smaller value has
	higher radius reduction rate (higher variance/noise, smaller expected error/bias), larger value
	has lower radius reduction rate (lower variance/noise, higher expected error/bias). Alpha
	effectively determines a trade-off between noise and bias. The value should be within (0, 1).
	*/
	real alpha = 2.0_r / 3.0_r;
};

}// end namespace ph
