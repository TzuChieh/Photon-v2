#pragma once

/*! @file

@brief Halton sample generator.

References:
[1] Ladislav Kocis and William J. Whiten, "Computational Investigations of Low-Discrepancy Sequences", 
last section of p. 274
[2] X. Wang and F.J. Hickernell, "Randomized Halton sequences", Mathematical and Computer Modelling,
Volume 32, Issues 7--8, 2000
*/

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

	void genSamplesGE3D(
		const SampleContext& context,
		const SampleStage&   stage,
		SamplesND            out_samples) override;

	bool isSamplesGE3DSupported() const override;

	void startNewSequence();

	void genSamplesOfAnyDimensions(
		const SampleContext& context,
		const SampleStage&   stage,
		SamplesND            out_samples);

	static real genSingleGeneralHaltonSample(
		EHaltonPermutation permutation,
		std::size_t dimIndex, 
		uint64 seedValue,
		TSpanView<halton_detail::PermutationTable> permutationTables);

	EHaltonPermutation m_permutation;
	EHaltonSequence m_sequence;

	std::vector<halton_detail::PermutationTable> m_permutationTables;
	std::vector<uint64> m_dimSeedRecords;
	uint64 m_leapAmount;
};

}// end namespace ph
