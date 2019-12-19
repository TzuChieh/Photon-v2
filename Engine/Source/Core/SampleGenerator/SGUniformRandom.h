#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class SGUniformRandom : public SampleGenerator, public TCommandInterface<SGUniformRandom>
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

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<SGUniformRandom> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  sample-generator                  </category>
	<type_name> uniform-random                    </type_name>
	<extend>    sample-generator.sample-generator </extend>

	<name> Uniform Random Sample Generator </name>
	<description>
		Generating samples in a completely random fashion.
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
