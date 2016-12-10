#include "Core/ImportanceRenderer.h"
#include "Common/primitive_type.h"
#include "Image/Film/Film.h"
#include "World/World.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Material/Material.h"
#include "Model/Material/Integrand/SurfaceIntegrand.h"
#include "Math/constant.h"
#include "Core/SampleGenerator.h"
#include "Core/Sample.h"
#include "Math/random_number.h"
#include "Math/Color.h"
#include "Math/Math.h"
#include "Core/BackwardPathIntegrator.h"

#include <cmath>
#include <iostream>
#include <vector>

namespace ph
{

ImportanceRenderer::~ImportanceRenderer() = default;

void ImportanceRenderer::render(const World& world, const Camera& camera) const
{
	Film* film = camera.getFilm();
	const Intersector& intersector = world.getIntersector();
	const uint32 widthPx = film->getWidthPx();
	const uint32 heightPx = film->getHeightPx();
	const float32 aspectRatio = static_cast<float32>(widthPx) / static_cast<float32>(heightPx);

	std::vector<Sample> samples;

	BackwardPathIntegrator integrator;
	integrator.update(intersector);

	Vector3f radiance;
	Ray primaryRay;
	int32 numSpp = 0;

	while(m_sampleGenerator->hasMoreSamples())
	{
		samples.clear();
		m_sampleGenerator->requestMoreSamples(*film, &samples);

		Sample sample;
		while(!samples.empty())
		{
			sample = samples.back();
			samples.pop_back();
			camera.genSampleRay(sample, &primaryRay, aspectRatio);
			
			integrator.radianceAlongRay(primaryRay, intersector, &radiance);

			uint32 x = static_cast<uint32>((sample.m_cameraX + 1.0f) / 2.0f * widthPx);
			uint32 y = static_cast<uint32>((sample.m_cameraY + 1.0f) / 2.0f * heightPx);
			if(x >= widthPx) x = widthPx - 1;
			if(y >= heightPx) y = heightPx - 1;

			film->accumulateRadiance(x, y, radiance);
		}// end while

		std::cout << "SPP: " << ++numSpp << std::endl;
	}
}

float32 ImportanceRenderer::queryPercentageProgress() const
{
	return 0.0f;
}

float32 ImportanceRenderer::querySampleFrequency() const
{
	return 0.0f;
}

}// end namespace ph