#include "Core/SampleGenerator/SGUniformRandom.h"
#include "Core/Filmic/Film.h"
#include "Core/Sample.h"
#include "Math/Random.h"
#include "FileIO/SDL/InputPacket.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

void SGUniformRandom::genSamples(const SampleStage& stage, real* const out_buffer)
{
	PH_ASSERT(out_buffer);

	for(std::size_t i = 0; i < stage.numElements(); ++i)
	{
		out_buffer[i] = math::Random::genUniformReal_i0_e1();
	}
}

std::unique_ptr<SampleGenerator> SGUniformRandom::genNewborn(const std::size_t numSamples) const
{
	return std::make_unique<SGUniformRandom>(numSamples);
}

// command interface

SdlTypeInfo SGUniformRandom::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "uniform-random");
}

void SGUniformRandom::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<SGUniformRandom>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<SGUniformRandom> SGUniformRandom::ciLoad(const InputPacket& packet)
{
	const auto numSamples = packet.getInteger("sample-amount", 0, DataTreatment::REQUIRED());

	// HACK: casting
	return std::make_unique<SGUniformRandom>(static_cast<std::size_t>(numSamples));
}

}// end namespace ph
