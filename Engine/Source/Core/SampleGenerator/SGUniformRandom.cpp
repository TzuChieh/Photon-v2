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

void SGUniformRandom::genSplitted(uint32 numSplits,
                                  std::vector<std::unique_ptr<SampleGenerator>>& out_sgs)
{
	if(!canSplit(numSplits))
	{
		return;
	}

	const std::size_t splittedNumSamples = numSamples() / numSplits;
	for(uint32 i = 0; i < numSplits; i++)
	{
		auto sampleGenerator = std::make_unique<SGUniformRandom>(splittedNumSamples);
		out_sgs.push_back(std::move(sampleGenerator));
	}
}

bool SGUniformRandom::canSplit(const uint32 nSplits) const
{
	if(nSplits == 0)
	{
		std::cerr << "warning: at PixelJitterSampleGenerator::canSplit(), " 
		          << "number of splits is 0" << std::endl;
		return false;
	}

	if(numSamples() % nSplits != 0)
	{
		std::cerr << "warning: at PixelJitterSampleGenerator::canSplit(), " 
		          << "generator cannot evenly split into " << nSplits << " parts" << std::endl;
		std::cerr << "(sample count: " << numSamples() << ")" << std::endl;
		return false;
	}

	return true;
}

// command interface

SdlTypeInfo SGUniformRandom::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "uniform-random");
}

std::unique_ptr<SGUniformRandom> SGUniformRandom::ciLoad(const InputPacket& packet)
{
	const integer numSamples = packet.getInteger("sample-amount", 0, DataTreatment::REQUIRED());

	// HACK: casting
	return std::make_unique<SGUniformRandom>(static_cast<std::size_t>(numSamples));
}

ExitStatus SGUniformRandom::ciExecute(const std::shared_ptr<SGUniformRandom>& targetResource, 
                                      const std::string& functionName, 
                                      const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph