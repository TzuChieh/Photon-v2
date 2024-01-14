#include "Core/SampleGenerator/SGHalton.h"
#include "Core/Sample.h"
#include "Core/SampleGenerator/SamplesND.h"
#include "Math/math.h"
#include "Core/SampleGenerator/Halton/halton.h"
#include "Core/SampleGenerator/SampleContext.h"
#include "Math/Random/Random.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <iostream>
#include <algorithm>
#include <type_traits>
#include <array>

namespace ph
{

SGHalton::SGHalton(const std::size_t numSamples)
	: SGHalton(numSamples, EHaltonPermutation::Fixed)
{}

SGHalton::SGHalton(const std::size_t numSamples, const EHaltonPermutation scheme)
	//: SampleGenerator(numSamples, numSamples)
	: SampleGenerator(numSamples, 4)// HARDCODE

	, m_permutationTables()
	, m_dimSeedRecords(halton_detail::MAX_DIMENSIONS, 0)
	, m_scheme(scheme)
{
	if(scheme == EHaltonPermutation::Fixed || scheme == EHaltonPermutation::PerDigit)
	{
		m_permutationTables.resize(halton_detail::MAX_DIMENSIONS);
		for(std::size_t dimIndex = 0; dimIndex < m_permutationTables.size(); ++dimIndex)
		{
			if(scheme == EHaltonPermutation::Fixed)
			{
				m_permutationTables[dimIndex] = halton_detail::FixedPermuter::makeTable(dimIndex);
			}
			else
			{
				PH_ASSERT(scheme == EHaltonPermutation::PerDigit);

				m_permutationTables[dimIndex] = halton_detail::PerDigitPermuter::makeTable(dimIndex);
			}
		}
	}
}

// TODO: able to control when to use a new permutation
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
				dimSample = halton_detail::radical_inverse(
					stageDimIndex, 
					m_dimSeedRecords[stageDimIndex] + si);
			}
			else if(stageDimIndex < halton_detail::MAX_DIMENSIONS)
			{
				dimSample = genSingleGeneralHaltonSample(
					stageDimIndex, 
					m_dimSeedRecords[stageDimIndex] + si);
			}
			// Run out of available dimensions, use random samples hereafter
			else
			{
				dimSample = math::Random::sample();
			}
			out_samples[si][di] = dimSample;
		}
	}
	
	// Update sample seed for each dimension
	const auto dimIndexBegin = stage.getDimIndexRange().first;
	const auto dimIndexEnd = std::min(stage.getDimIndexRange().second, halton_detail::MAX_DIMENSIONS);
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

real SGHalton::genSingleGeneralHaltonSample(const std::size_t dimIndex, const uint64 seedValue) const
{
	switch(m_scheme)
	{
	case EHaltonPermutation::None:
		return halton_detail::radical_inverse(dimIndex, seedValue);

	case EHaltonPermutation::Fixed:
		return halton_detail::radical_inverse_permuted(
			dimIndex,
			seedValue,
			halton_detail::FixedPermuter(m_permutationTables[dimIndex]));

	case EHaltonPermutation::PerDigit:
		return halton_detail::radical_inverse_permuted(
			dimIndex,
			seedValue,
			halton_detail::PerDigitPermuter(m_permutationTables[dimIndex]));

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0.0_r;
	}
}

}// end namespace ph
