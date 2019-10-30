#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class SGUniformRandom : public SampleGenerator, public TCommandInterface<SGUniformRandom>
{
public:
	explicit SGUniformRandom(const std::size_t numSamples);

private:
	std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;
	void genSamples1D(const Samples1DStage& stage, Samples1D* out_array) override;
	void genSamples2D(const Samples2DStage& stage, Samples2D* out_array) override;
	void genSamplesND(const SamplesNDStage& stage, SamplesND* out_array) override;

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
