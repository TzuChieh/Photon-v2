#include "Core/MtImportanceRenderer.h"
#include "Common/primitive_type.h"
#include "Image/Film.h"
#include "World/World.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Material/Material.h"
#include "Model/Material/SurfaceIntegrand.h"
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
#include <thread>
#include <atomic>

namespace ph
{

MtImportanceRenderer::~MtImportanceRenderer() = default;

void MtImportanceRenderer::render(const World& world, const Camera& camera) const
{
	BackwardPathIntegrator integrator;
	integrator.cook(world);

	std::atomic<int32> numSpp = 0;

	const uint32 nThreads = 4;

	std::thread renderWorkers[nThreads];
	for(auto& renderWorker : renderWorkers)
	{
		renderWorker = std::thread([this, &camera, &integrator, &world, &numSpp]()
		{
		// ****************************** thread start ****************************** //

		Film subFilm(camera.getFilm()->getWidthPx(), camera.getFilm()->getHeightPx());
		const uint32 widthPx = subFilm.getWidthPx();
		const uint32 heightPx = subFilm.getHeightPx();
		const float32 aspectRatio = static_cast<float32>(widthPx) / static_cast<float32>(heightPx);

		std::vector<Sample> samples;

		Ray primaryRay;
		Vector3f radiance;

		while(true)
		{
			samples.clear();

			m_mutex.lock();
			const bool shouldRun = m_sampleGenerator->hasMoreSamples();
			if(shouldRun)
			{
				m_sampleGenerator->requestMoreSamples(subFilm, &samples);
			}
			m_mutex.unlock();

			if(!shouldRun)
			{
				break;
			}

			Sample sample;
			while(!samples.empty())
			{
				sample = samples.back();
				samples.pop_back();
				camera.genSampleRay(sample, &primaryRay, aspectRatio);


				integrator.radianceAlongRay(primaryRay, world, &radiance);


				uint32 x = static_cast<uint32>((sample.m_cameraX + 1.0f) / 2.0f * widthPx);
				uint32 y = static_cast<uint32>((sample.m_cameraY + 1.0f) / 2.0f * heightPx);
				if(x >= widthPx) x = widthPx - 1;
				if(y >= heightPx) y = heightPx - 1;

				subFilm.accumulateRadiance(x, y, radiance);
			}// end while

			m_mutex.lock();
			std::cout << "SPP: " << ++numSpp << std::endl;
			m_mutex.unlock();
		}

		m_mutex.lock();
		camera.getFilm()->accumulateRadiance(subFilm);
		m_mutex.unlock();

		// ****************************** thread end ****************************** //
		});
	}

	for(auto& renderWorker : renderWorkers)
	{
		renderWorker.join();
	}
}

}// end namespace ph