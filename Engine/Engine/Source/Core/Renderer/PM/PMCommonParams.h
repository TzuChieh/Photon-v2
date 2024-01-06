#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

class PMCommonParams final
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

	/*! Minimum number of bounces for a photon. 0-bounce photon is not supported, as it is like
	approximating emissive surface with photons and the result is overly blurry for no good reason.
	0-bounce lighting is done via traditional path sampling.
	*/
	uint32 minPhotonBounces = 1;

	/*! Maximum number of bounces for a photon. The default value is effectively infinite bounces.
	Combined with `minPhotonBounces`, these parameters are useful if only some number of bounces
	are estimated via photons.
	*/
	uint32 maxPhotonBounces = 16384;
};

}// end namespace ph