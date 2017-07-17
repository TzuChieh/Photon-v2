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
#include "Core/SampleGenerator/PixelJitterSampleGenerator.h"
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
	const Film&        film            = *(description.getFilm());
	const Integrator&  integrator      = *(description.getIntegrator());
	SampleGenerator*   sampleGenerator = description.getSampleGenerator().get();

	m_subFilms.clear();
	m_subFilms.shrink_to_fit();

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

	m_subFilms = std::vector<Film>(m_numThreads, Film(film.getWidthPx(), film.getHeightPx()));
	sampleGenerator->split(m_numThreads, &subSampleGenerators);

	for(std::size_t threadIndex = 0; threadIndex < m_numThreads; threadIndex++)
	{
		SampleGenerator*      subSampleGenerator = subSampleGenerators[threadIndex].get();
		Film*                 subFilm = &(m_subFilms[threadIndex]);
		std::atomic<float32>* workerProgress = m_workerProgresses[threadIndex].get();
		std::atomic<float32>* workerSampleFreq = m_workerSampleFrequencies[threadIndex].get();

		renderWorkers[threadIndex] = std::thread([this, &camera, &integrator, &world, &numSpp, &isLT, subSampleGenerator, subFilm, workerProgress, workerSampleFreq]() -> void
		{
			// ****************************** thread start ****************************** //

			const uint32 widthPx = camera.getFilm()->getWidthPx();
			const uint32 heightPx = camera.getFilm()->getHeightPx();

			std::vector<Sample> samples;
			std::vector<SenseEvent> senseEvents;

			const uint32 totalSpp = subSampleGenerator->getSppBudget();
			uint32 currentSpp = 0;

			std::chrono::time_point<std::chrono::system_clock> t1;
			std::chrono::time_point<std::chrono::system_clock> t2;

			while(subSampleGenerator->hasMoreSamples())
			{
				t1 = std::chrono::system_clock::now();

				samples.clear();
				subSampleGenerator->requestMoreSamples(&samples);

				while(!samples.empty())
				{
					integrator.radianceAlongRay(samples.back(), world.getScene(), camera, senseEvents);
					samples.pop_back();

					// HACK
					if(!isLT)
					{
						for(const auto& senseEvent : senseEvents)
						{
							uint32 x = static_cast<uint32>(senseEvent.filmX * widthPx);
							uint32 y = static_cast<uint32>(senseEvent.filmY * heightPx);
							if(x >= widthPx) x = widthPx - 1;
							if(y >= heightPx) y = heightPx - 1;
							subFilm->accumulateRadiance(x, y, senseEvent.radiance);
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
							uint32 x = static_cast<uint32>(senseEvent.filmX * widthPx);
							uint32 y = static_cast<uint32>(senseEvent.filmY * heightPx);
							if(x >= widthPx) x = widthPx - 1;
							if(y >= heightPx) y = heightPx - 1;
							subFilm->accumulateRadianceWithoutIncrementSenseCount(x, y, senseEvent.radiance);
						}
					}
					senseEvents.clear();
				}// end while

					// HACK
				if(isLT)
				{
					subFilm->incrementAllSenseCounts();
				}

				currentSpp++;
				*workerProgress = static_cast<float32>(currentSpp) / static_cast<float32>(totalSpp);

				m_rendererMutex.lock();
				std::cout << "SPP: " << ++numSpp << std::endl;
				m_rendererMutex.unlock();

				t2 = std::chrono::system_clock::now();

				auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
				*workerSampleFreq = static_cast<float32>(widthPx * heightPx) / static_cast<float32>(msPassed.count()) * 1000.0f;
			}

			m_rendererMutex.lock();
			camera.getFilm()->accumulateRadiance(*subFilm);
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