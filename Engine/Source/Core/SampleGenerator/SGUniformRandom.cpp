#include "Core/SampleGenerator/SGUniformRandom.h"
#include "Core/Filmic/Film.h"
#include "Core/Sample.h"
#include "Math/Random.h"
#include "FileIO/InputPacket.h"

#include <iostream>

namespace ph
{

SGUniformRandom::SGUniformRandom(const std::size_t numSamples) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4)// HACK
{

}

SGUniformRandom::~SGUniformRandom() = default;

void SGUniformRandom::genArray1D(SampleArray1D* const out_array)
{
	for(std::size_t i = 0; i < out_array->numElements(); i++)
	{
		out_array->set(i, Random::genUniformReal_i0_e1());
	}
}

void SGUniformRandom::genArray2D(SampleArray2D* const out_array)
{
	for(std::size_t i = 0; i < out_array->numElements(); i++)
	{
		out_array->set(i, 
		               Random::genUniformReal_i0_e1(), 
		               Random::genUniformReal_i0_e1());
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
	const integer numSamples = packet.getInteger("sample-amount", 0, DataTreatment::REQUIRED());

	// HACK: casting
	return std::make_unique<SGUniformRandom>(static_cast<std::size_t>(numSamples));
}

}// end namespace ph