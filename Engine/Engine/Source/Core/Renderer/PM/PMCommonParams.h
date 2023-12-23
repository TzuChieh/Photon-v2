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
	amounts of memory for some algorithms.
	*/
	std::size_t numSamplesPerPixel = 4;

	/*! Energy contribution radius for each photon. For progressive techniques, this value is for
	setting up the initial radius.
	*/
	real kernelRadius = 0.1_r;
};

}// end namespace ph
