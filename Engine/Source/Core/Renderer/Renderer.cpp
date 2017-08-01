#include "Core/Renderer/Renderer.h"
#include "Common/primitive_type.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Core/Camera/Camera.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/Description.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderData.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <utility>

namespace ph
{

Renderer::Renderer() : 
	m_numThreads(0)
{

}

Renderer::~Renderer() = default;

void Renderer::render(const Description& description)
{
	clearWorkerData();
	genFullRegionRenderWorkers(description, m_numThreads);

	std::vector<std::thread> renderThreads(m_numThreads);
	for(std::size_t ti = 0; ti < m_numThreads; ti++)
	{
		renderThreads[ti] = std::thread(&RenderWorker::run, &m_workers[ti]);

		std::cout << "worker<" << ti << "> started" << std::endl;
	}

	for(std::size_t ti = 0; ti < m_numThreads; ti++)
	{
		renderThreads[ti].join();

		std::cout << "worker<" << ti << "> finished" << std::endl;
	}

	/*for(auto& worker : m_workers)
	{
		worker.data.film->mergeToParent();
	}*/
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
float32 Renderer::asyncQueryPercentageProgress() const
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

float32 Renderer::asyncQuerySampleFrequency() const
{
	float32 sampleFreq = 0.0f;
	/*for(uint32 threadId = 0; threadId < m_workerSampleFrequencies.size(); threadId++)
	{
		sampleFreq += *(m_workerSampleFrequencies[threadId]);
	}*/

	return sampleFreq;
}

void Renderer::genFullRegionRenderWorkers(const Description& description, const uint32 numWorkers)
{
	Film& film = *(description.getFilm());
	for(uint32 i = 0; i < numWorkers; i++)
	{
		m_workerFilms.push_back(film.genChild(film.getEffectiveWindowPx(), true));
	}

	SampleGenerator& sg = *(description.getSampleGenerator());
	sg.genSplitted(numWorkers, m_workerSgs);

	m_workers.resize(numWorkers);
	for(std::size_t ti = 0; ti < m_numThreads; ti++)
	{
		const RenderData renderData(&description.visualWorld.getScene(),
		                            description.getCamera().get(), 
		                            description.getIntegrator().get(), 
		                            m_workerSgs[ti].get(), 
		                            m_workerFilms[ti].get());
		m_workers[ti] = RenderWorker(RendererProxy(this), renderData);
	}
}

void Renderer::clearWorkerData()
{
	m_workers.clear();
	m_workerSgs.clear();
	m_workerFilms.clear();
	m_updatedRegions.clear();
}

void Renderer::asyncAddUpdatedRegion(const Region& region)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	for(const Region& pendingRegion : m_updatedRegions)
	{
		if(pendingRegion.equals(region))
		{
			return;
		}
	}

	m_updatedRegions.push_back(region);

	std::cout << region.toString() << std::endl;
}

bool Renderer::asyncPollUpdatedRegion(Region* const out_region)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_updatedRegions.empty())
	{
		return false;
	}

	*out_region = m_updatedRegions.front();
	m_updatedRegions.pop_front();

	return true;
}

}// end namespace ph