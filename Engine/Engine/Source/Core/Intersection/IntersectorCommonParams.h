#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

/*! @brief Common parameters of intersector.
*/
class IntersectorCommonParams final
{
public:
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

	/*! Hint for the minimum path length to start estimating energy using photons on glossy surface.
	If the scene contains diffuse surface and is easily reachable by photons, it is recommended
	to set this to a lower value.
	*/
	uint32 glossyMergeBeginLength = 1;

	/*! Hint for the view path length to start random path sampling. If this value differ too much
	from the mean specular path length from the scene, the energy estimation result may contain higher
	variance or bias. Beware when using higher values as non-stochastic path may be branched,
	which can result in exponential growth of number of rays.
	*/
	uint32 stochasticViewSampleBeginLength = 3;
};

}// end namespace ph
