#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/SampleGenerator/Halton/halton_randomization_enums.h"
#include "Core/SampleGenerator/Halton/halton_fwd.h"
#include "Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>

namespace ph
{

class SGHalton : public SampleGenerator
{
public:
	SGHalton(
		std::size_t numSamples, 
		EHaltonPermutation permutation,
		EHaltonSequence sequence);

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

	static real genSingleGeneralHaltonSample(
		EHaltonPermutation permutation,
		std::size_t dimIndex, 
		uint64 seedValue,
		TSpanView<halton_detail::PermutationTable> permutationTables);

	std::vector<halton_detail::PermutationTable> m_permutationTables;
	std::vector<std::size_t> m_dimSeedRecords;
	EHaltonPermutation m_permutation;
	EHaltonSequence m_sequence;
	std::size_t m_leapAmount;
};

}// end namespace ph
