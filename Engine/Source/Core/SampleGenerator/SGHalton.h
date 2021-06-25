#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"
#include "Core/SampleGenerator/Detail/RadicalInversePermutations.h"

#include <cstddef>
#include <memory>

namespace ph
{

class SGHalton : public SampleGenerator
{
public:
	explicit SGHalton(std::size_t numSamples);

private:
	void genSamples1D(
		const SampleContext& context,
		const SampleStage&   stage,
		SamplesND            out_samples) override;

	void genSamples2D(
		const SampleContext& context,
		const SampleStage&   stage,
		SamplesND            out_samples) override;

	void genSamplesGE3D(
		const SampleContext& context,
		const SampleStage&   stage,
		SamplesND            out_samples) override;

	bool isSamplesGE3DSupported() const override;

	std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;

	void genSamplesOfAnyDimensions(
		const SampleContext& context,
		const SampleStage&   stage,
		SamplesND            out_samples);

	using Permutations = detail::halton::RadicalInversePermutations;

	std::shared_ptr<Permutations> m_permutations;
	std::vector<std::size_t>      m_dimSeedRecords;
};

}// end namespace ph
