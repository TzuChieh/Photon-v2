#include "Core/SampleGenerator/SGStratified.h"
#include "Core/Filmic/Film.h"
#include "Core/Sample.h"
#include "Math/Random.h"
#include "FileIO/SDL/InputPacket.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

SGStratified::SGStratified(const std::size_t numSamples) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4)// HACK
{}

void SGStratified::genSamples1D(const Samples1DStage& stage, Samples1D* const out_array)
{
	PH_ASSERT(out_array);

	const real dx = 1.0_r / static_cast<real>(out_array->numSamples());
	for(std::size_t x = 0; x < out_array->numSamples(); ++x)
	{
		const real jitter = Random::genUniformReal_i0_e1();
		out_array->set(x, (static_cast<real>(x) + jitter) * dx);
	}

	out_array->perSampleShuffle();
}

void SGStratified::genSamples2D(const Samples2DStage& stage, Samples2D* const out_array)
{
	PH_ASSERT(out_array);

	// TODO: automatically pick some nice hint if (1, 1) was given
	const Vector2S    strataSizes = stage.getDimSizeHints();
	const std::size_t numStrata   = strataSizes.product();
	PH_ASSERT(numStrata > 0);

	// OPT: It is possible to precompute how many samples will be in a
	// stratum and generate them together. 

	// Tries to generate <numStrata> samples over and over again until there 
	// is no room in <out_array> to fit another <numStrata> samples.
	
	const real dx = 1.0_r / static_cast<real>(strataSizes.x);
	const real dy = 1.0_r / static_cast<real>(strataSizes.y);

	std::size_t currentIndex = 0;
	while(currentIndex + numStrata <= out_array->numSamples())
	{
		for(std::size_t y = 0; y < strataSizes.y; ++y)
		{
			for(std::size_t x = 0; x < strataSizes.x; ++x)
			{
				const real jitterX = Random::genUniformReal_i0_e1();
				const real jitterY = Random::genUniformReal_i0_e1();
				out_array->set(currentIndex,
				               (static_cast<real>(x) + jitterX) * dx,
				               (static_cast<real>(y) + jitterY) * dy);
				++currentIndex;
			}
		}
	}

	// There is no room to fit another <numStrata> samples. We fill the resting
	// spaces with random ones.
	//
	// TODO: use hypercube sampling?
	PH_ASSERT(out_array->numSamples() - currentIndex < numStrata);
	for(std::size_t i = currentIndex; i < out_array->numSamples(); ++i)
	{
		out_array->set(i, 
		               Random::genUniformReal_i0_e1(), 
		               Random::genUniformReal_i0_e1());
	}

	out_array->perSampleShuffle();
}

void SGStratified::genSamplesND(const SamplesNDStage& stage, SamplesND* const out_array)
{
	// TODO
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