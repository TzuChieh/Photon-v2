#include "Core/SampleGenerator/SGUniformRandom.h"
#include "Math/Random/Random.h"

namespace ph
{

void SGUniformRandom::onRebirth()
{}

void SGUniformRandom::genSamples1D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	for(std::size_t si = 0; si < out_samples.numSamples(); ++si)
	{
		out_samples.setSample<1>(si, math::Random::sampleND<1>());
	}
}

void SGUniformRandom::genSamples2D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	for(std::size_t si = 0; si < out_samples.numSamples(); ++si)
	{
		out_samples.setSample<2>(si, math::Random::sampleND<2>());
	}
}

std::unique_ptr<SampleGenerator> SGUniformRandom::makeNewborn(const std::size_t numSampleBatches) const
{
	return std::make_unique<SGUniformRandom>(numSampleBatches);
}

}// end namespace ph
