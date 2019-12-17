#include "Core/SampleGenerator/SGHalton.h"
#include "Common/assertion.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/Sample.h"
#include "Core/SampleGenerator/SamplesND.h"
#include "Math/math.h"
#include "Core/SampleGenerator/Detail/halton.h"

#include <iostream>
#include <algorithm>
#include <type_traits>

namespace ph
{

SGHalton::SGHalton(const std::size_t numSamples) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4)// HACK
{}

void SGHalton::genSamples1D(const SampleStage& stage, SamplesND out_samples)
{
	PH_ASSERT_EQ(stage.numDims(), 1);
	PH_ASSERT_GE(stage.numSamples(), 1);

	// TODO
}

void SGHalton::genSamples2D(const SampleStage& stage, SamplesND out_samples)
{
	PH_ASSERT_EQ(stage.numDims(), 2);
	PH_ASSERT_GE(stage.numSamples(), 1);
	PH_ASSERT_EQ(stage.getDimSizeHints().size(), 2);

	// TODO
}

std::unique_ptr<SampleGenerator> SGHalton::genNewborn(const std::size_t numSamples) const
{
	return std::make_unique<SGHalton>(numSamples);
}

// command interface

SdlTypeInfo SGHalton::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "halton");
}

void SGHalton::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<SGHalton>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<SGHalton> SGHalton::ciLoad(const InputPacket& packet)
{
	const integer numSamples = packet.getInteger("sample-amount", 1, DataTreatment::REQUIRED());

	// HACK: casting
	return std::make_unique<SGHalton>(static_cast<std::size_t>(numSamples));
}

}// end namespace ph
