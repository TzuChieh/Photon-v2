#include "Core/StandardSampleGenerator.h"
#include "Image/Film.h"
#include "Core/Sample.h"
#include "Math/random_number.h"

#include <iostream>

namespace ph
{

StandardSampleGenerator::StandardSampleGenerator(const uint32 sppBudget) :
	SampleGenerator(sppBudget), 
	m_numDispatchedSpp(0)
{

}

StandardSampleGenerator::~StandardSampleGenerator() = default;

bool StandardSampleGenerator::hasMoreSamples() const
{
	return m_numDispatchedSpp < getSppBudget();
}

void StandardSampleGenerator::requestMoreSamples(const Film& film, std::vector<Sample>* const out_samples)
{
	if(!hasMoreSamples())
	{
		return;
	}

	const float32 halfWidthPx = static_cast<float32>(film.getWidthPx()) / 2.0f;
	const float32 halfHeightPx = static_cast<float32>(film.getHeightPx()) / 2.0f;
	const float32 invWidthPx = 1.0f / halfWidthPx;
	const float32 invHeightPx = 1.0f / halfHeightPx;

	Sample sample;
	for(uint32 y = 0; y < film.getHeightPx(); y++)
	{
		for(uint32 x = 0; x < film.getWidthPx(); x++)
		{
			sample.m_cameraX = (static_cast<float32>(x) + genRandomFloat32_0_1_uniform() - halfWidthPx) * invWidthPx;
			sample.m_cameraY = (static_cast<float32>(y) + genRandomFloat32_0_1_uniform() - halfHeightPx) * invHeightPx;
			out_samples->push_back(sample);
		}
	}

	m_numDispatchedSpp++;
}

void StandardSampleGenerator::split(const uint32 nSplits, std::vector<std::unique_ptr<SampleGenerator>>* const out_sampleGenerators)
{
	if(!canSplit(nSplits))
	{
		return;
	}

	const uint32 splittedSppBudget = getSppBudget() / nSplits;
	for(uint32 i = 0; i < nSplits; i++)
	{
		out_sampleGenerators->push_back(std::make_unique<StandardSampleGenerator>(splittedSppBudget));
	}

	// works are dispatched into splitted sample generators
	m_numDispatchedSpp = getSppBudget();
}

bool StandardSampleGenerator::canSplit(const uint32 nSplits) const
{
	if(nSplits == 0)
	{
		std::cerr << "warning: at StandardSampleGenerator::canSplit(), number of splits is 0" << std::endl;
		return false;
	}

	if(!hasMoreSamples())
	{
		std::cerr << "warning: at StandardSampleGenerator::canSplit(), generator already run out of samples" << std::endl;
		return false;
	}

	if(getSppBudget() % nSplits != 0)
	{
		std::cerr << "warning: at StandardSampleGenerator::canSplit(), generator cannot evenly split into " << nSplits << " parts" << std::endl;
		std::cerr << "(SPP budget: " << getSppBudget() << ")" << std::endl;
		return false;
	}

	return true;
}

}// end namespace ph