#include "Core/SampleGenerator/SGUniformRandom.h"
#include "Core/Filmic/Film.h"
#include "Core/Sample.h"
#include "Math/Random.h"
#include "FileIO/SDL/InputPacket.h"

#include <iostream>

namespace ph
{

SGUniformRandom::SGUniformRandom(const std::size_t numSamples) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4)// HACK
{}

void SGUniformRandom::genSamples1D(const Samples1DStage& stage, Samples1D* const out_array)
{
	for(std::size_t i = 0; i < out_array->numSamples(); ++i)
	{
		out_array->set(i, math::Random::genUniformReal_i0_e1());
	}
}

void SGUniformRandom::genSamples2D(const Samples2DStage& stage, Samples2D* const out_array)
{
	for(std::size_t i = 0; i < out_array->numSamples(); ++i)
	{
		out_array->set(
			i, 
			math::Random::genUniformReal_i0_e1(),
			math::Random::genUniformReal_i0_e1());
	}
}

void SGUniformRandom::genSamplesND(const SamplesNDStage& stage, SamplesND* const out_array)
{
	// TODO
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
	const integer numSamples = packet.getInteger("sample-amount", 0, DataTreatment::REQUIRED());

	// HACK: casting
	return std::make_unique<SGUniformRandom>(static_cast<std::size_t>(numSamples));
}

}// end namespace ph
