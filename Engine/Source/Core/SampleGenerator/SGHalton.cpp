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

	m_permutations  (std::make_shared<Permutations>(detail::halton::MAX_DIMENSIONS)),
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
			else
			{
				dimSample = detail::halton::radical_inverse_permuted(
					stageDimIndex,
					m_dimSeedRecords[stageDimIndex] + si,
					m_permutations->getPermutationForDim(stageDimIndex));
			}
			out_samples[si][di] = dimSample;
		}
	}
	
	// Update sample seed for each dimension
	for(std::size_t stageDimIndex = stage.getDimIndexRange().first;
		stageDimIndex < stage.getDimIndexRange().second;
		++stageDimIndex)
	{
		m_dimSeedRecords[stageDimIndex] += out_samples.numSamples();
	}
}

std::unique_ptr<SampleGenerator> SGHalton::genNewborn(const std::size_t numSamples) const
{
	// TODO: reuse permutations or not? 
	// be careful that using the same permutation in different generator instances can lead
	// to exactly the same generated samples
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
