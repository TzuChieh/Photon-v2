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

MtImportanceRenderer::MtImportanceRenderer()
{

}

MtImportanceRenderer::~MtImportanceRenderer() = default;

void MtImportanceRenderer::render(const World& world, const Camera& camera) const
{
	m_subFilms.clear();
	m_subFilms.shrink_to_fit();
	m_subFilmMutices.clear();
	m_subFilmMutices.shrink_to_fit();

	BackwardPathIntegrator integrator;
	integrator.cook(world);

	std::atomic<int32> numSpp = 0;

	const uint32 nThreads = 4;

	std::thread renderWorkers[nThreads];
	std::size_t ti = 0;
	m_subFilms = std::vector<Film>(4, Film(camera.getFilm()->getWidthPx(), camera.getFilm()->getHeightPx()));
	for(std::size_t tid = 0; tid < nThreads; tid++)
	{
		m_subFilmMutices.push_back(std::make_unique<std::mutex>());
	}
	for(auto& renderWorker : renderWorkers)
	{
		const std::size_t threadIndex = ti++;

		renderWorker = std::thread([this, &camera, &integrator, &world, &numSpp, threadIndex]()
		{
		// ****************************** thread start ****************************** //

		const uint32 widthPx = camera.getFilm()->getWidthPx();
		const uint32 heightPx = camera.getFilm()->getHeightPx();
		const float32 aspectRatio = static_cast<float32>(widthPx) / static_cast<float32>(heightPx);

		std::vector<Sample> samples;

		Ray primaryRay;
		Vector3f radiance;

		while(true)
		{
			m_subFilmMutices[threadIndex]->lock();

			samples.clear();

			m_mutex.lock();
			const bool shouldRun = m_sampleGenerator->hasMoreSamples();
			if(shouldRun)
			{
				m_sampleGenerator->requestMoreSamples(m_subFilms[threadIndex], &samples);
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

				m_subFilms[threadIndex].accumulateRadiance(x, y, radiance);
			}// end while

			m_mutex.lock();
			std::cout << "SPP: " << ++numSpp << std::endl;
			m_mutex.unlock();

			m_subFilmMutices[threadIndex]->unlock();
		}

		m_mutex.lock();
		camera.getFilm()->accumulateRadiance(m_subFilms[threadIndex]);
		m_mutex.unlock();

		// ****************************** thread end ****************************** //
		});
	}

	for(auto& renderWorker : renderWorkers)
	{
		renderWorker.join();
	}
}

void MtImportanceRenderer::queryIntermediateFilm(Film* const out_film) const
{
	if(out_film == nullptr)
	{
		std::cerr << "warning: at MtImportanceRenderer::queryIntermediateFilm(), input is null" << std::endl;
		return;
	}

	out_film->clear();

	for(uint32 threadId = 0; threadId < m_subFilmMutices.size(); threadId++)
	{
		m_subFilmMutices[threadId]->lock();
		out_film->accumulateRadiance(m_subFilms[threadId]);
		m_subFilmMutices[threadId]->unlock();
	}
}

float32 MtImportanceRenderer::queryPercentageProgress() const
{
	return 0.0f;
}

}// end namespace ph