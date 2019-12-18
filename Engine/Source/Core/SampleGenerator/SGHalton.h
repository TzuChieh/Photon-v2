#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"
#include "Core/SampleGenerator/Detail/RadicalInversePermutations.h"

#include <memory>

namespace ph
{

class SGHalton : public SampleGenerator, public TCommandInterface<SGHalton>
{
public:
	explicit SGHalton(std::size_t numSamples);

private:
	void genSamples1D(const SampleStage& stage, SamplesND out_samples) override;
	void genSamples2D(const SampleStage& stage, SamplesND out_samples) override;
	std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;

	std::shared_ptr<detail::halton::RadicalInversePermutations> m_permutations;

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
