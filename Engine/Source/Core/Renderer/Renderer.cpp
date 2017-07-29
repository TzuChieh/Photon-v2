#include "Core/Renderer/Renderer.h"
#include "Common/primitive_type.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Core/Camera/Camera.h"
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
#include "Core/Renderer/RenderData.h"
#include "Core/Renderer/RenderWorker.h"

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
	std::vector<std::unique_ptr<Film>> subFilms;
	for(std::size_t ti = 0; ti < m_numThreads; ti++)
	{
		Film& film = *(description.getFilm());
		subFilms.push_back(film.genChild(film.getEffectiveWindowPx()));
	}

	SampleGenerator* sg = description.getSampleGenerator().get();
	std::vector<std::unique_ptr<SampleGenerator>> subSampleGenerators;
	sg->genSplitted(m_numThreads, subSampleGenerators);

	m_workers.resize(m_numThreads);
	std::vector<std::thread> renderThreads(m_numThreads);
	for(std::size_t ti = 0; ti < m_numThreads; ti++)
	{
		const RenderData renderData(&description.visualWorld.getScene(),
		                            description.getCamera().get(), 
		                            description.getIntegrator().get(), 
		                            subSampleGenerators[ti].get(), 
		                            subFilms[ti].get());
		m_workers[ti] = RenderWorker(renderData);
		renderThreads[ti] = std::thread(&RenderWorker::run, &m_workers[ti]);

		std::cout << "worker<" << ti << "> started" << std::endl;
	}

	for(std::size_t ti = 0; ti < m_numThreads; ti++)
	{
		renderThreads[ti].join();

		std::cout << "worker<" << ti << "> finished" << std::endl;
	}

	for(auto& worker : m_workers)
	{
		worker.data.film->mergeToParent();
	}
}

void Renderer::setNumRenderThreads(const uint32 numThreads)
{
	m_numThreads = numThreads;

	/*m_workerProgresses.clear();
	m_workerProgresses.shrink_to_fit();
	m_workerSampleFrequencies.clear();
	m_workerSampleFrequencies.shrink_to_fit();
	for(std::size_t threadIndex = 0; threadIndex < m_numThreads; threadIndex++)
	{
		m_workerProgresses.push_back(std::make_unique<std::atomic<float32>>(0.0f));
		m_workerSampleFrequencies.push_back(std::make_unique<std::atomic<float32>>(0.0f));
	}*/
}

// TODO: avoid outputting NaN
float32 Renderer::queryPercentageProgress() const
{
	std::size_t totalWork = 0;
	std::size_t workDone  = 0;
	for(const auto& worker : m_workers)
	{
		const auto& progress = worker.queryProgress();
		totalWork += progress.totalWork;
		workDone  += progress.workDone;
	}

	return static_cast<float32>(workDone) / static_cast<float32>(totalWork) * 100.0f;
}

float32 Renderer::querySampleFrequency() const
{
	float32 sampleFreq = 0.0f;
	/*for(uint32 threadId = 0; threadId < m_workerSampleFrequencies.size(); threadId++)
	{
		sampleFreq += *(m_workerSampleFrequencies[threadId]);
	}*/

	return sampleFreq;
}

}// end namespace ph