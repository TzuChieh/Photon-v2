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

void StandardSampleGenerator::requestMoreSamples(const Film& film, std::vector<Sample>* out_samples)
{
	if(!hasMoreSamples())
	{
		return;
	}

	const float32 halfWidthPx = static_cast<float32>(film.getWidthPx()) / 2.0f;
	const float32 halfHeightPx = static_cast<float32>(film.getHeightPx()) / 2.0f;

	Sample sample;
	for(uint32 y = 0; y < film.getHeightPx(); y++)
	{
		for(uint32 x = 0; x < film.getWidthPx(); x++)
		{
			sample.m_cameraX = (static_cast<float32>(x) + genRandomFloat32_0_1_uniform() - halfWidthPx) / halfWidthPx;
			sample.m_cameraY = (static_cast<float32>(y) + genRandomFloat32_0_1_uniform() - halfHeightPx) / halfHeightPx;
			out_samples->push_back(sample);

			/*if(sample.m_cameraX < -1.0f) std::cout << "x -1: " << sample.m_cameraX << std::endl;
			if(sample.m_cameraY < -1.0f) std::cout << "y -1: " << sample.m_cameraY << std::endl;
			if(sample.m_cameraX > 1.0f) std::cout << "x 1: " << sample.m_cameraX << std::endl;
			if(sample.m_cameraY > 1.0f) std::cout << "y 1: " << sample.m_cameraY << std::endl;*/
		}
	}

	m_numDispatchedSpp++;
}

}// end namespace ph