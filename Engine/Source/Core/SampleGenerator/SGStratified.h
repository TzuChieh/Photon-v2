#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class SGStratified : public SampleGenerator, public TCommandInterface<SGStratified>
{
public:
	explicit SGStratified(std::size_t numSamples);

private:
	void genSamples(const SampleStage& stage, real* out_buffer) override;
	std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;

	void genSamples1D(const SampleStage& stage, real* out_buffer);
	void genSamples2D(const SampleStage& stage, real* out_buffer);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<SGStratified> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  sample-generator                  </category>
	<type_name> stratified                        </type_name>
	<extend>    sample-generator.sample-generator </extend>

	<name> Stratified Sample Generator </name>
	<description>
		Generating samples based on engine provided dimensional hints.
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
