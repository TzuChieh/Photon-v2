#include "Core/SampleGenerator/SGUniformRandom.h"
#include "Core/Filmic/Film.h"
#include "Core/Sample.h"
#include "Math/Random.h"

#include <iostream>

namespace ph
{

SGUniformRandom::SGUniformRandom(const std::size_t numSamples) :
	SampleGenerator(numSamples, numSamples)
{

}

SGUniformRandom::~SGUniformRandom() = default;

void SGUniformRandom::genArray1D(real* coordArray1Ds,
                                 std::size_t num1Ds,
                                 EPhaseType type)
{
	for(std::size_t i = 0; i < num1Ds; i++)
	{
		coordArray1Ds[i] = Random::genUniformReal_i0_e1();
	}
}

void SGUniformRandom::genArray2D(Vector2R* coordArray2Ds,
                                 std::size_t num2Ds,
                                 EPhaseType type)
{
	for(std::size_t i = 0; i < num2Ds; i++)
	{
		coordArray2Ds[i].x = Random::genUniformReal_i0_e1();
		coordArray2Ds[i].y = Random::genUniformReal_i0_e1();

		//std::cerr << "gen" << std::endl;
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

SGUniformRandom::SGUniformRandom(const InputPacket& packet) :
	SampleGenerator(packet)
{

}

SdlTypeInfo SGUniformRandom::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLER, "uniform-random");
}

std::unique_ptr<SGUniformRandom> SGUniformRandom::ciLoad(const InputPacket& packet)
{
	return std::make_unique<SGUniformRandom>(packet);
}

ExitStatus SGUniformRandom::ciExecute(const std::shared_ptr<SGUniformRandom>& targetResource, 
                                      const std::string& functionName, 
                                      const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph