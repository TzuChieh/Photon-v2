#pragma once

#include "Engine/Core/SampleGenerator/SampleGenerator.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>

namespace ph
{

class SGStratified : public SampleGenerator
{
public:
	explicit SGStratified(std::size_t numSamples);

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

	// TODO: >= 3-D sample is straightforward

	void reviseSampleStage(SampleStageReviser reviser) override;

	static std::vector<std::size_t> reviseDimSizeHints(
		std::size_t                     numSamples, 
		const std::vector<std::size_t>& originalDimSizeHints);
};

}// end namespace ph
