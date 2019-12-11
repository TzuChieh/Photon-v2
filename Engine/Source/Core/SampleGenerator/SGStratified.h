#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

#include <cstddef>
#include <vector>

namespace ph
{

// TODO: >= 3-D sample is straightforward

class SGStratified : public SampleGenerator, public TCommandInterface<SGStratified>
{
public:
	explicit SGStratified(std::size_t numSamples);

private:
	void genSamples1D(const SampleStage& stage, SamplesND& out_samples) override;
	void genSamples2D(const SampleStage& stage, SamplesND& out_samples) override;
	void reviseSampleStage(SampleStageReviser& reviser) override;
	std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;

	static std::vector<std::size_t> reviseDimSizeHints(
		std::size_t                     numSamples, 
		const std::vector<std::size_t>& originalDimSizeHints);

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
