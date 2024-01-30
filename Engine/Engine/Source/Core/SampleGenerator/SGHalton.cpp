#include "Core/SampleGenerator/SGHalton.h"
#include "Core/Sample.h"
#include "Core/SampleGenerator/SamplesND.h"
#include "Math/math.h"
#include "Core/SampleGenerator/Halton/halton.h"
#include "Core/SampleGenerator/SampleContext.h"
#include "Math/Random/Random.h"
#include "Math/math_table.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <iostream>
#include <algorithm>
#include <type_traits>
#include <array>

namespace ph
{

// TODO: we can actually do leap + any permutation, will it be worth?

SGHalton::SGHalton(
	const std::size_t numSamples, 
	const EHaltonPermutation permutation,
	const EHaltonSequence sequence)
	//: SampleGenerator(numSamples, numSamples)
	: SampleGenerator(numSamples, 4)// HARDCODE

	, m_permutationTables()
	, m_dimSeedRecords(halton_detail::MAX_DIMENSIONS, 0)
	, m_permutation(permutation)
	, m_sequence(sequence)
	, m_leapAmount(1)
{
	if(permutation == EHaltonPermutation::Fixed || permutation == EHaltonPermutation::PerDigit)
	{
		m_permutationTables.resize(halton_detail::MAX_DIMENSIONS);
		for(std::size_t dimIndex = 0; dimIndex < m_permutationTables.size(); ++dimIndex)
		{
			if(permutation == EHaltonPermutation::Fixed)
			{
				m_permutationTables[dimIndex] = halton_detail::FixedPermuter::makeTable(dimIndex);
			}
			else
			{
				PH_ASSERT(permutation == EHaltonPermutation::PerDigit);

				m_permutationTables[dimIndex] = halton_detail::PerDigitPermuter::makeTable(dimIndex);
			}
		}
	}

	if(sequence == EHaltonSequence::Leap || sequence == EHaltonSequence::RandomLeap)
	{
		// Sevaral good leap values were found experimentally by minimizing the integration error
		// for several test functions in [1]. Those values are 31, 61, 149, 409 and 1949. The value
		// should be a prime and different than all the bases used for generating Halton sequence.

		constexpr auto maxHaltonBase = math::table::PRIME[halton_detail::MAX_DIMENSIONS - 1];

		// For non random leap, just pick one arbitrarily
		constexpr auto leapPrime = math::table::GOOD_PRIME[256];
		static_assert(leapPrime > maxHaltonBase);

		constexpr uint64 randomLeapRange[2] = {151, math::table::GOOD_PRIME.size()};
		static_assert(randomLeapRange[0] < randomLeapRange[1]);
		static_assert(math::table::GOOD_PRIME[randomLeapRange[0]] > maxHaltonBase);

		m_leapAmount = sequence == EHaltonSequence::Leap
			? leapPrime 
			: math::table::GOOD_PRIME[math::Random::index(randomLeapRange[0], randomLeapRange[1])];
	}

	if(sequence == EHaltonSequence::RandomStart)
	{
		// Note the difference between random-start and random-skip sequences: random-start is using
		// a random starting point in [0, 1] and generate the sequence from there (effectively adding
		// `1 / BASE` with rightward-carry add in each iteration), while random-skip is skipping a
		// certain number of samples from the start. Since we have only finite precision, random-start
		// is effectively the same as random-skip with some skip amount [2].

		for(uint64& dimSeed : m_dimSeedRecords)
		{
			// We do not need to generate a uniform random value in [0, 1] and work out the 
			// corresponding sample index/seed for it. A uniform random sample index value will also 
			// have uniform random digits for any base. After applying radical inverse, the resulting
			// (first) sample will be uniform random in [0, 1].
			dimSeed = math::Random::bits64();
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
	// Seed will not progress if leap == 0
	PH_ASSERT_GE(m_leapAmount, 1);

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
					m_dimSeedRecords[stageDimIndex] + si * m_leapAmount);
			}
			else if(stageDimIndex < halton_detail::MAX_DIMENSIONS)
			{
				dimSample = genSingleGeneralHaltonSample(
					m_permutation,
					stageDimIndex, 
					m_dimSeedRecords[stageDimIndex] + si * m_leapAmount,
					m_permutationTables);
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
		m_dimSeedRecords[i] += out_samples.numSamples() * m_leapAmount;
	}
}

std::unique_ptr<SampleGenerator> SGHalton::genNewborn(const std::size_t numSamples) const
{
	// TODO: settings for reuse permutations or not? be careful that using the same permutation 
	// in different generator instances can lead to exactly the same generated samples

	return std::make_unique<SGHalton>(numSamples, m_permutation, m_sequence);
}

real SGHalton::genSingleGeneralHaltonSample(
	const EHaltonPermutation permutation,
	const std::size_t dimIndex,
	const uint64 seedValue,
	TSpanView<halton_detail::PermutationTable> permutationTables)
{
	switch(permutation)
	{
	case EHaltonPermutation::None:
		return halton_detail::radical_inverse(dimIndex, seedValue);

	case EHaltonPermutation::Fixed:
		return halton_detail::radical_inverse_permuted(
			dimIndex,
			seedValue,
			halton_detail::FixedPermuter(permutationTables[dimIndex]));

	case EHaltonPermutation::PerDigit:
		return halton_detail::radical_inverse_permuted(
			dimIndex,
			seedValue,
			halton_detail::PerDigitPermuter(permutationTables[dimIndex]));

	case EHaltonPermutation::Owen:
		return halton_detail::radical_inverse_permuted(
			dimIndex,
			seedValue,
			halton_detail::OwenScrambledPermuter(dimIndex));

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0.0_r;
	}
}

}// end namespace ph
