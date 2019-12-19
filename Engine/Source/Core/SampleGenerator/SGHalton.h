#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"
#include "Core/SampleGenerator/Detail/RadicalInversePermutations.h"

#include <cstddef>
#include <memory>

namespace ph
{

class SGHalton : public SampleGenerator, public TCommandInterface<SGHalton>
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

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<SGHalton> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  sample-generator                  </category>
	<type_name> halton                            </type_name>
	<extend>    sample-generator.sample-generator </extend>

	<name> Halton Sample Generator </name>
	<description>
	</description>

	<command type="creator">
		<input name="sample-amount" type="integer">
			<description>
				Controls the number of sample batches that will be generated.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
