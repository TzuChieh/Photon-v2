#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class SGUniformRandom : public SampleGenerator
{
public:
	using SampleGenerator::SampleGenerator;

private:
	void genSamples1D(
		const SampleContext& context,
		const SampleStage&   stage, 
		SamplesND            out_samples) override;

	void genSamples2D(
		const SampleContext& context,
		const SampleStage&   stage,
		SamplesND            out_samples) override;

	std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;
};

}// end namespace ph
