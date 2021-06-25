#include "Core/SampleGenerator/SGHalton.h"
#include "Common/assertion.h"
#include "Core/Sample.h"
#include "Core/SampleGenerator/SamplesND.h"
#include "Math/math.h"
#include "Core/SampleGenerator/Detail/halton.h"
#include "Core/SampleGenerator/SampleContext.h"
#include "Math/Random.h"

#include <iostream>
#include <algorithm>
#include <type_traits>
#include <array>

namespace ph
{

SGHalton::SGHalton(const std::size_t numSamples) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4),// HARDCODE

	m_permutations  (std::make_shared<Permutations>(detail::halton::MAX_DIMENSIONS)),// TODO: able to control when to use a new permutation
	m_dimSeedRecords(detail::halton::MAX_DIMENSIONS, 0)
{}

// TODO: hammersley for fixed number of samples

void SGHalton::genSamples1D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	PH_ASSERT_EQ(stage.numDims(), 1);

	genSamplesOfAnyDimensions(context, stage, out_samples);
}

void SGHalton::genSamples2D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	PH_ASSERT_EQ(stage.numDims(), 2);

	genSamplesOfAnyDimensions(context, stage, out_samples);
}

void SGHalton::genSamplesGE3D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	PH_ASSERT_GE(stage.numDims(), 3);

	genSamplesOfAnyDimensions(context, stage, out_samples);
}

bool SGHalton::isSamplesGE3DSupported() const
{
	return true;
}

void SGHalton::genSamplesOfAnyDimensions(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	for(std::size_t si = 0; si < out_samples.numSamples(); ++si)
	{
		for(std::size_t di = 0, stageDimIndex = stage.getDimIndexRange().first;
			di < out_samples.numDims(); 
			++di, ++stageDimIndex)
		{
			// While generating samples, lower dimensions already have nice 
			// distributions such that permuting them is unnecessary

			real dimSample;
			if(stageDimIndex < 2)
			{
				dimSample = detail::halton::radical_inverse(
					stageDimIndex, 
					m_dimSeedRecords[stageDimIndex] + si);
			}
			else if(stageDimIndex < detail::halton::MAX_DIMENSIONS)
			{
				dimSample = detail::halton::radical_inverse_permuted(
					stageDimIndex,
					m_dimSeedRecords[stageDimIndex] + si,
					m_permutations->getPermutationForDim(stageDimIndex));
			}
			// Run out of available dimensions, use random samples hereafter
			else
			{
				dimSample = math::Random::genUniformReal_i0_e1();
			}
			out_samples[si][di] = dimSample;
		}
	}
	
	// Update sample seed for each dimension
	const auto dimIndexBegin = stage.getDimIndexRange().first;
	const auto dimIndexEnd   = std::min(stage.getDimIndexRange().second, detail::halton::MAX_DIMENSIONS);
	for(std::size_t i = dimIndexBegin; i < dimIndexEnd; ++i)
	{
		m_dimSeedRecords[i] += out_samples.numSamples();
	}
}

std::unique_ptr<SampleGenerator> SGHalton::genNewborn(const std::size_t numSamples) const
{
	// TODO: reuse permutations or not? 
	// be careful that using the same permutation in different generator instances can lead
	// to exactly the same generated samples
	return std::make_unique<SGHalton>(numSamples);
}

}// end namespace ph
