#include "Core/SampleGenerator/SGUniformRandom.h"
#include "Core/Filmic/Film.h"
#include "Core/Sample.h"
#include "Math/Random.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

void SGUniformRandom::genSamples1D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	for(std::size_t si = 0; si < out_samples.numSamples(); ++si)
	{
		out_samples.setSample<1>(
			si, 
			{math::Random::genUniformReal_i0_e1()});
	}
}

void SGUniformRandom::genSamples2D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	for(std::size_t si = 0; si < out_samples.numSamples(); ++si)
	{
		out_samples.setSample<2>(
			si, 
			{math::Random::genUniformReal_i0_e1(), math::Random::genUniformReal_i0_e1()});
	}
}

std::unique_ptr<SampleGenerator> SGUniformRandom::genNewborn(const std::size_t numSamples) const
{
	return std::make_unique<SGUniformRandom>(numSamples);
}

}// end namespace ph
