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
	init(description);

	std::vector<std::thread> renderThreads(m_numThreads);

	m_workers.resize(m_numThreads);
	for(uint32 ti = 0; ti < m_numThreads; ti++)
	{
		m_workers[ti] = RenderWorker(RendererProxy(this), ti);
	}

	for(uint32 ti = 0; ti < m_numThreads; ti++)
	{
		renderThreads[ti] = std::thread(&RenderWorker::run, &m_workers[ti]);

		std::cout << "worker<" << ti << "> started" << std::endl;
	}

	for(std::size_t ti = 0; ti < m_numThreads; ti++)
	{
		renderThreads[ti].join();

		std::cout << "worker<" << ti << "> finished" << std::endl;
	}
}

void Renderer::setNumRenderThreads(const uint32 numThreads)
{
	m_numThreads = numThreads;
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

// TODO
float32 Renderer::asyncQuerySampleFrequency() const
{
	float32 sampleFreq = 0.0f;
	/*for(uint32 threadId = 0; threadId < m_workerSampleFrequencies.size(); threadId++)
	{
	sampleFreq += *(m_workerSampleFrequencies[threadId]);
	}*/

	return sampleFreq;
}

}// end namespace ph