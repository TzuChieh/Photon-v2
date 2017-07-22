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

	//m_subFilms.clear();
	//m_subFilms.shrink_to_fit();

	//BackwardPathIntegrator integrator;
	//BackwardLightIntegrator integrator;
	//BackwardMisIntegrator integrator;
	//NormalBufferIntegrator integrator;
	//LightTracingIntegrator integrator;

	//const bool isLT = true;
	const bool isLT = false;

	//integrator.update(world);

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

		renderWorkers[threadIndex] = std::thread([this, &camera, &integrator, &world, &numSpp, &isLT, subSampleGenerator, subFilm, workerProgress, workerSampleFreq]() -> void
		{
			// ****************************** thread start ****************************** //

			const uint32 widthPx = camera.getFilm()->getEffectiveResPx().x;
			const uint32 heightPx = camera.getFilm()->getEffectiveResPx().x;

			TSamplePhase<const Vector2R*> camSamplePhase = subSampleGenerator->declareArray2DPhase(widthPx * heightPx);

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
					Sample sample;
					sample.m_cameraX = camSamples[si].x;
					sample.m_cameraY = camSamples[si].y;

					//std::cerr << sample.m_cameraX << ", " << sample.m_cameraY << std::endl;

					integrator.radianceAlongRay(sample, world.getScene(), camera, senseEvents);

					// HACK
					if(!isLT)
					{
						for(const auto& senseEvent : senseEvents)
						{
							//uint32 x = static_cast<uint32>(senseEvent.filmX * widthPx);
							//uint32 y = static_cast<uint32>(senseEvent.filmY * heightPx);
							//if(x >= widthPx) x = widthPx - 1;
							//if(y >= heightPx) y = heightPx - 1;
							//subFilm->accumulateRadiance(x, y, senseEvent.radiance);
							subFilm->addSample(senseEvent.filmX * widthPx, senseEvent.filmY * heightPx, senseEvent.radiance);
						}

						if(senseEvents.size() != 1)
						{
							std::cerr << "unexpected event occured" << std::endl;
						}
					}
					else
					{
						for(const auto& senseEvent : senseEvents)
						{
							//uint32 x = static_cast<uint32>(senseEvent.filmX * widthPx);
							//uint32 y = static_cast<uint32>(senseEvent.filmY * heightPx);
							//if(x >= widthPx) x = widthPx - 1;
							//if(y >= heightPx) y = heightPx - 1;
							//subFilm->accumulateRadianceWithoutIncrementSenseCount(x, y, senseEvent.radiance);
						}
					}
					senseEvents.clear();
				}// end for

					// HACK
				if(isLT)
				{
					//subFilm->incrementAllSenseCounts();
				}

				currentSamples++;
				*workerProgress = static_cast<float32>(currentSamples) / static_cast<float32>(totalSamples);

				m_rendererMutex.lock();
				std::cout << "SPP: " << ++numSpp << std::endl;
				m_rendererMutex.unlock();

				t2 = std::chrono::system_clock::now();

				auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
				*workerSampleFreq = static_cast<float32>(widthPx * heightPx) / static_cast<float32>(msPassed.count()) * 1000.0f;

				subSampleGenerator->singleSampleEnd();
			}

			m_rendererMutex.lock();
			//camera.getFilm()->accumulateRadiance(*subFilm);
			subFilm->mergeToParent();
			m_rendererMutex.unlock();

			// ****************************** thread end ****************************** //
		});

		//renderWorkers[threadIndex] = std::make_unique<std::thread>(workerFunction);
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