#include "Core/SampleGenerator/SGStratified.h"
#include "Core/Filmic/Film.h"
#include "Core/Sample.h"
#include "Math/Random.h"
#include "FileIO/SDL/InputPacket.h"
#include "Common/assertion.h"
#include "Core/SampleGenerator/TSamplesND.h"
#include "Math/TVector2.h"

#include <iostream>

namespace ph
{

SGStratified::SGStratified(const std::size_t numSamples) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4)// HACK
{}

void SGStratified::genSamples(const SampleStage& stage, real* const out_buffer)
{
	PH_ASSERT(out_buffer);

	if(stage.numDimensions() == 1)
	{
		genSamples1D(stage, out_buffer);
	}
	else if(stage.numDimensions() == 2)
	{
		genSamples2D(stage, out_buffer);
	}
	else
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

void SGStratified::genSamples1D(const SampleStage& stage, real* const out_buffer)
{
	PH_ASSERT(out_buffer);
	PH_ASSERT_EQ(stage.numDimensions(), 1);
	PH_ASSERT_NE(stage.numSamples(), 0);

	const real dx = 1.0_r / static_cast<real>(stage.numSamples());

	TSamplesND<1> samples(out_buffer, stage.numSamples());
	for(std::size_t x = 0; x < stage.numSamples(); ++x)
	{
		const real jitter = math::Random::genUniformReal_i0_e1();
		samples.setSample(x, {(static_cast<real>(x) + jitter) * dx});
	}
	samples.shuffle();
}

void SGStratified::genSamples2D(const SampleStage& stage, real* const out_buffer)
{
	PH_ASSERT(out_buffer);
	PH_ASSERT_EQ(stage.numDimensions(), 2);
	PH_ASSERT_NE(stage.numSamples(), 0);
	PH_ASSERT_EQ(stage.getDimSizeHints().size(), 2);

	// TODO: automatically pick some nice hint if (1, 1) was given
	const auto        strataSizes = math::Vector2S(stage.getDimSizeHints());
	const std::size_t numStrata   = strataSizes.product();
	PH_ASSERT_GT(numStrata, 0);

	// OPT: It is possible to precompute how many samples will be in a
	// stratum and generate them together. 

	// Tries to generate <numStrata> samples over and over again until there 
	// is no room in <out_array> to fit another <numStrata> samples.

	const real dx = 1.0_r / static_cast<real>(strataSizes.x);
	const real dy = 1.0_r / static_cast<real>(strataSizes.y);

	TSamplesND<2> samples(out_buffer, stage.numSamples());

	std::size_t currentIndex = 0;
	while(currentIndex + numStrata <= stage.numSamples())
	{
		for(std::size_t y = 0; y < strataSizes.y; ++y)
		{
			for(std::size_t x = 0; x < strataSizes.x; ++x)
			{
				const real jitterX = math::Random::genUniformReal_i0_e1();
				const real jitterY = math::Random::genUniformReal_i0_e1();
				samples.setSample(
					currentIndex, 
					{(static_cast<real>(x) + jitterX) * dx, (static_cast<real>(y) + jitterY) * dy});

				++currentIndex;
			}
		}
	}

	// There is no room to fit another <numStrata> samples. We fill the remaining
	// spaces with random ones.
	PH_ASSERT_LT(samples.numSamples() - currentIndex, numStrata);

	// TODO: use hypercube sampling?
	for(std::size_t i = currentIndex; i < samples.numSamples(); ++i)
	{
		samples.setSample(
			i,
			{math::Random::genUniformReal_i0_e1(), math::Random::genUniformReal_i0_e1()});
	}

	samples.shuffle();
}

std::unique_ptr<SampleGenerator> SGStratified::genNewborn(const std::size_t numSamples) const
{
	return std::make_unique<SGStratified>(numSamples);
}

// command interface

SdlTypeInfo SGStratified::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "stratified");
}

void SGStratified::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<SGStratified>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<SGStratified> SGStratified::ciLoad(const InputPacket& packet)
{
	const integer numSamples = packet.getInteger("sample-amount", 0, DataTreatment::REQUIRED());

	// HACK: casting
	return std::make_unique<SGStratified>(static_cast<std::size_t>(numSamples));
}

}// end namespace ph
