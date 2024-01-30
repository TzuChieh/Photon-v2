#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"

namespace ph
{

class SGUniformRandom : public SampleGenerator
{
public:
	using SampleGenerator::SampleGenerator;

	std::unique_ptr<SampleGenerator> makeNewborn(std::size_t numSampleBatches) const override;

private:
	void onRebirth() override;

	void genSamples1D(
		const SampleContext& context,
		const SampleStage&   stage, 
		SamplesND            out_samples) override;

	void genSamples2D(
		const SampleContext& context,
		const SampleStage&   stage,
		SamplesND            out_samples) override;
};

}// end namespace ph
