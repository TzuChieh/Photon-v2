#include "Core/SampleGenerator/PixelFixedSampleGenerator.h"
#include "Image/Film/Film.h"
#include "Core/Sample.h"

#include <iostream>

namespace ph
{

PixelFixedSampleGenerator::PixelFixedSampleGenerator(const uint32 sppBudget) :
	SampleGenerator(sppBudget), 
	m_numDispatchedSpp(0), m_filmWidthPx(0), m_filmHeightPx(0)
{

}

PixelFixedSampleGenerator::~PixelFixedSampleGenerator() = default;

bool PixelFixedSampleGenerator::hasMoreSamples() const
{
	return m_numDispatchedSpp < getSppBudget();
}

void PixelFixedSampleGenerator::analyze(const World& world, const Film& film)
{
	m_filmWidthPx = film.getWidthPx();
	m_filmHeightPx = film.getHeightPx();
}

void PixelFixedSampleGenerator::requestMoreSamples(std::vector<Sample>* const out_samples)
{
	if(!hasMoreSamples())
	{
		return;
	}

	const float32 halfWidthPx = static_cast<float32>(m_filmWidthPx) / 2.0f;
	const float32 halfHeightPx = static_cast<float32>(m_filmHeightPx) / 2.0f;
	const float32 invWidthPx = 1.0f / halfWidthPx;
	const float32 invHeightPx = 1.0f / halfHeightPx;

	Sample sample;
	for(uint32 y = 0; y < m_filmHeightPx; y++)
	{
		for(uint32 x = 0; x < m_filmWidthPx; x++)
		{
			sample.m_cameraX = (static_cast<float32>(x) + 0.5f - halfWidthPx) * invWidthPx;
			sample.m_cameraY = (static_cast<float32>(y) + 0.5f - halfHeightPx) * invHeightPx;
			out_samples->push_back(sample);
		}
	}

	m_numDispatchedSpp++;
}

void PixelFixedSampleGenerator::split(const uint32 nSplits, std::vector<std::unique_ptr<SampleGenerator>>* const out_sampleGenerators)
{
	if(!canSplit(nSplits))
	{
		return;
	}

	const uint32 splittedSppBudget = getSppBudget() / nSplits;
	for(uint32 i = 0; i < nSplits; i++)
	{
		auto sampleGenerator = std::make_unique<PixelFixedSampleGenerator>(splittedSppBudget);
		sampleGenerator->m_filmWidthPx = m_filmWidthPx;
		sampleGenerator->m_filmHeightPx = m_filmHeightPx;
		out_sampleGenerators->push_back(std::move(sampleGenerator));
	}

	// works are dispatched into splitted sample generators
	m_numDispatchedSpp = getSppBudget();
}

bool PixelFixedSampleGenerator::canSplit(const uint32 nSplits) const
{
	if(nSplits == 0)
	{
		std::cerr << "warning: at PixelFixedSampleGenerator::canSplit(), number of splits is 0" << std::endl;
		return false;
	}

	if(!hasMoreSamples())
	{
		std::cerr << "warning: at PixelFixedSampleGenerator::canSplit(), generator already run out of samples" << std::endl;
		return false;
	}

	if(getSppBudget() % nSplits != 0)
	{
		std::cerr << "warning: at PixelFixedSampleGenerator::canSplit(), generator cannot evenly split into " << nSplits << " parts" << std::endl;
		std::cerr << "(SPP budget: " << getSppBudget() << ")" << std::endl;
		return false;
	}

	return true;
}

}// end namespace ph