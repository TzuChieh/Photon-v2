#include "Core/Renderer.h"
#include "Common/primitive_type.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Actor/Material/Material.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Sample.h"
#include "Math/Random.h"
#include "Math/Color.h"
#include "Math/Math.h"
#include "Core/Integrator/BackwardPathIntegrator.h"
#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Integrator/BackwardMisIntegrator.h"
#include "Core/Integrator/NormalBufferIntegrator.h"
#include "Core/Integrator/LightTracingIntegrator.h"
#include "FileIO/Description.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/RenderData.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

namespace ph
{

Renderer::Renderer() : 
	m_numThreads(0)
{

}

Renderer::~Renderer() = default;

void Renderer::render(const Description& description) const
{
	const VisualWorld& world           = description.visualWorld;
	const Camera&      camera          = *(description.getCamera());
	const Integrator&  integrator      = *(description.getIntegrator());
	Film&              film            = *(description.getFilm());
	SampleGenerator*   sampleGenerator = description.getSampleGenerator().get();

	const RenderData renderData(&world.getScene(), &camera);

	std::atomic<int32> numSpp = 0;
	std::vector<std::thread> renderWorkers(m_numThreads);
	std::vector<std::unique_ptr<SampleGenerator>> subSampleGenerators;
	
	std::vector<std::unique_ptr<Film>> subFilms;
	for(std::size_t ti = 0; ti < m_numThreads; ti++)
	{
		subFilms.push_back(film.genChild(film.getEffectiveResPx().x, film.getEffectiveResPx().y));
	}

	sampleGenerator->genSplitted(m_numThreads, subSampleGenerators);

	for(std::size_t threadIndex = 0; threadIndex < m_numThreads; threadIndex++)
	{
		SampleGenerator*      subSampleGenerator = subSampleGenerators[threadIndex].get();
		Film*                 subFilm = subFilms[threadIndex].get();
		std::atomic<float32>* workerProgress = m_workerProgresses[threadIndex].get();
		std::atomic<float32>* workerSampleFreq = m_workerSampleFrequencies[threadIndex].get();
		

		renderWorkers[threadIndex] = std::thread([this, &renderData, &integrator, &numSpp, subSampleGenerator, subFilm, workerProgress, workerSampleFreq]() -> void
		{
			// ****************************** thread start ****************************** //

			const uint64 filmWpx = subFilm->getEffectiveResPx().x;
			const uint64 filmHpx = subFilm->getEffectiveResPx().y;

			const Vector2D flooredSampleMinVertex = subFilm->getSampleWindowPx().minVertex.floor();
			const Vector2D ceiledSampleMaxVertex  = subFilm->getSampleWindowPx().maxVertex.ceil();
			const uint64 filmSampleWpx = static_cast<uint64>(ceiledSampleMaxVertex.x - flooredSampleMinVertex.x);
			const uint64 filmSampleHpx = static_cast<uint64>(ceiledSampleMaxVertex.y - flooredSampleMinVertex.y);
			const uint64 numCamPhaseSamples = filmSampleWpx * filmSampleHpx;

			TSamplePhase<const Vector2R*> camSamplePhase = subSampleGenerator->declareArray2DPhase(numCamPhaseSamples);


			std::vector<SenseEvent> senseEvents;

			const std::size_t totalSamples = subSampleGenerator->numSamples();
			std::size_t currentSamples = 0;

			std::chrono::time_point<std::chrono::system_clock> t1;
			std::chrono::time_point<std::chrono::system_clock> t2;

			while(subSampleGenerator->singleSampleStart())
			{
				t1 = std::chrono::system_clock::now();

				const Vector2R* camSamples = subSampleGenerator->getNextArray2D(camSamplePhase);

				for(std::size_t si = 0; si < camSamplePhase.numElements(); si++)
				{
					const Vector2D rasterPosPx(camSamples[si].x * filmSampleWpx + flooredSampleMinVertex.x,
					                           camSamples[si].y * filmSampleHpx + flooredSampleMinVertex.y);

					if(!subFilm->getSampleWindowPx().isIntersectingArea(rasterPosPx))
					{
						continue;
					}

					Ray ray;
					renderData.camera->genSensingRay(Vector2R(rasterPosPx), &ray);

					integrator.radianceAlongRay(ray, renderData, senseEvents);

					// HACK: sense event
					for(const auto& senseEvent : senseEvents)
					{
						subFilm->addSample(rasterPosPx.x, rasterPosPx.y, senseEvent.radiance);
					}

					if(senseEvents.size() != 1)
					{
						std::cerr << "unexpected event occured" << std::endl;
					}

					senseEvents.clear();
				}// end for

				currentSamples++;
				*workerProgress = static_cast<float32>(currentSamples) / static_cast<float32>(totalSamples);

				m_rendererMutex.lock();
				std::cout << "SPP: " << ++numSpp << std::endl;
				m_rendererMutex.unlock();

				t2 = std::chrono::system_clock::now();

				auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
				*workerSampleFreq = static_cast<float32>(filmWpx * filmHpx) / static_cast<float32>(msPassed.count()) * 1000.0f;

				subSampleGenerator->singleSampleEnd();
			}

			m_rendererMutex.lock();
			subFilm->mergeToParent();
			m_rendererMutex.unlock();

			// ****************************** thread end ****************************** //
		});
	}

	for(auto& renderWorker : renderWorkers)
	{
		renderWorker.join();
	}
}

void Renderer::setNumRenderThreads(const uint32 numThreads)
{
	m_numThreads = numThreads;

	m_workerProgresses.clear();
	m_workerProgresses.shrink_to_fit();
	m_workerSampleFrequencies.clear();
	m_workerSampleFrequencies.shrink_to_fit();
	for(std::size_t threadIndex = 0; threadIndex < m_numThreads; threadIndex++)
	{
		m_workerProgresses.push_back(std::make_unique<std::atomic<float32>>(0.0f));
		m_workerSampleFrequencies.push_back(std::make_unique<std::atomic<float32>>(0.0f));
	}
}

float32 Renderer::queryPercentageProgress() const
{
	float32 avgWorkerProgress = 0.0f;
	for(uint32 threadId = 0; threadId < m_workerProgresses.size(); threadId++)
	{
		avgWorkerProgress += *(m_workerProgresses[threadId]);
	}
	avgWorkerProgress /= static_cast<float32>(m_workerProgresses.size());

	return avgWorkerProgress * 100.0f;
}

float32 Renderer::querySampleFrequency() const
{
	float32 sampleFreq = 0.0f;
	for(uint32 threadId = 0; threadId < m_workerSampleFrequencies.size(); threadId++)
	{
		sampleFreq += *(m_workerSampleFrequencies[threadId]);
	}

	return sampleFreq;
}

}// end namespace ph