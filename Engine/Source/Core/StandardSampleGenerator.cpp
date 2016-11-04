#include "Core/StandardSampleGenerator.h"
#include "Image/Frame.h"
#include "Core/Sample.h"
#include "Math/random_number.h"

#include <iostream>

namespace ph
{

StandardSampleGenerator::StandardSampleGenerator(const uint32 numSpp) : 
	m_numSpp(numSpp), m_numDispatchedSpp(0)
{

}

StandardSampleGenerator::~StandardSampleGenerator() = default;

bool StandardSampleGenerator::hasMoreSamples() const
{
	return m_numDispatchedSpp < m_numSpp;
}

void StandardSampleGenerator::requestMoreSamples(const Frame& frame, std::vector<Sample>* out_samples)
{
	if(!hasMoreSamples())
	{
		return;
	}

	const float32 halfWidthPx = static_cast<float32>(frame.getWidthPx()) / 2.0f;
	const float32 halfHeightPx = static_cast<float32>(frame.getHeightPx()) / 2.0f;

	Sample sample;
	for(uint32 y = 0; y < frame.getHeightPx(); y++)
	{
		for(uint32 x = 0; x < frame.getWidthPx(); x++)
		{
			sample.m_cameraX = (static_cast<float32>(x) + genRandomFloat32_0_1_uniform() - halfWidthPx) / halfWidthPx;
			sample.m_cameraY = (static_cast<float32>(y) + genRandomFloat32_0_1_uniform() - halfHeightPx) / halfHeightPx;
			out_samples->push_back(sample);
		}
	}

	m_numDispatchedSpp++;
}

}// end namespace ph