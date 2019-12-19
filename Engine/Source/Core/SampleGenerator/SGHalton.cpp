#include "Core/SampleGenerator/SGHalton.h"
#include "Common/assertion.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/Sample.h"
#include "Core/SampleGenerator/SamplesND.h"
#include "Math/math.h"
#include "Core/SampleGenerator/Detail/halton.h"
#include "Core/SampleGenerator/SampleContext.h"

#include <iostream>
#include <algorithm>
#include <type_traits>
#include <array>

namespace ph
{

SGHalton::SGHalton(const std::size_t numSamples) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4),// HARDCODE

	m_permutations         (std::make_shared<Permutations>(detail::halton::MAX_DIMENSIONS)),
	m_dimSampleValueRecords(detail::halton::MAX_DIMENSIONS, 0)
{}

// TODO: hammersley?
// TODO: just provide an impl. for any dimension?

void SGHalton::genSamples1D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	PH_ASSERT_EQ(stage.numDims(), 1);
	PH_ASSERT_GE(stage.numSamples(), 1);

	const auto stageDimIndex = stage.getDimIndexRange().first;
	const auto valueRecord   = m_dimSampleValueRecords[stageDimIndex];

	if(stageDimIndex < 2)
	{
		for(std::size_t si = 0; si < out_samples.numSamples(); ++si)
		{
			out_samples.setSample<1>(
				si, 
				{detail::halton::radical_inverse(stageDimIndex, valueRecord + si)});
		}
	}
	else
	{
		const auto* const permutation = m_permutations->getPermutationForDim(stageDimIndex);

		for(std::size_t si = 0; si < out_samples.numSamples(); ++si)
		{
			out_samples.setSample<1>(
				si, 
				{detail::halton::radical_inverse_permuted(
					stageDimIndex, valueRecord + si, permutation)});
		}
	}

	m_dimSampleValueRecords[stageDimIndex] += out_samples.numSamples();
}

void SGHalton::genSamples2D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	PH_ASSERT_EQ(stage.numDims(), 2);
	PH_ASSERT_GE(stage.numSamples(), 1);
	PH_ASSERT_EQ(stage.getDimSizeHints().size(), 2);

	const std::array<std::size_t, 2> stageDimIndices{
		stage.getDimIndexRange().first,
		stage.getDimIndexRange().first + 1};

	const std::array<std::size_t, 2> valueRecords{
		m_dimSampleValueRecords[stageDimIndices[0]],
		m_dimSampleValueRecords[stageDimIndices[1]]};

	for(std::size_t si = 0; si < out_samples.numSamples(); ++si)
	{
		std::array<real, 2> sample;
		for(std::size_t i = 0; i < 2; ++i)
		{
			if(stageDimIndices[i] < 2)
			{
				sample[i] = detail::halton::radical_inverse(stageDimIndices[i], valueRecords[i] + si);
			}
			else
			{
				sample[i] = detail::halton::radical_inverse_permuted(
					stageDimIndices[i], 
					valueRecords[i] + si, 
					m_permutations->getPermutationForDim(stageDimIndices[i]));
			}
		}

		out_samples.setSample(si, sample);
	}

	m_dimSampleValueRecords[stageDimIndices[0]] += out_samples.numSamples();
	m_dimSampleValueRecords[stageDimIndices[1]] += out_samples.numSamples();
}

std::unique_ptr<SampleGenerator> SGHalton::genNewborn(const std::size_t numSamples) const
{
	// TODO: reuse permutations or not?
	return std::make_unique<SGHalton>(numSamples);
}

// command interface

SdlTypeInfo SGHalton::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "halton");
}

void SGHalton::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<SGHalton>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<SGHalton> SGHalton::ciLoad(const InputPacket& packet)
{
	const integer numSamples = packet.getInteger("sample-amount", 1, DataTreatment::REQUIRED());

	// HACK: casting
	return std::make_unique<SGHalton>(static_cast<std::size_t>(numSamples));
}

}// end namespace ph
