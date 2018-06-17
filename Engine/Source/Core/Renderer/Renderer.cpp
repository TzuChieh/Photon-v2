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
#include "Utility/Timer.h"

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <utility>

namespace ph
{

Renderer::Renderer() : 
	m_numThreads(0)
{}

Renderer::~Renderer() = default;

void Renderer::render(const Description& description)
{
	Timer renderTimer;
	renderTimer.start();

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

	renderTimer.finish();
	std::cout << "rendering time: " << renderTimer.getDeltaMs() << " ms" << std::endl;
}

void Renderer::setNumRenderThreads(const uint32 numThreads)
{
	m_numThreads = numThreads;
}

void Renderer::asyncQueryStatistics(float32* const out_percentageProgress, 
                                    float32* const out_samplesPerSecond) const
{
	uint64  totalWork     = 0;
	uint64  totalWorkDone = 0;
	float64 samplesPerMs  = 0.0;
	for(const auto& worker : m_workers)
	{
		const auto& statistics = worker.getStatistics();

		// FIXME: this calculation can be wrong if there are more works than workers
		totalWork     += statistics.totalWork;
		totalWorkDone += statistics.workDone;

		if(statistics.numMsElapsed != 0)
		{
			samplesPerMs += static_cast<float64>(statistics.numSamplesTaken) / statistics.numMsElapsed;
		}
	}

	*out_percentageProgress = totalWork != 0 ? 
		static_cast<float32>(totalWorkDone) / static_cast<float32>(totalWork) * 100.0f : 0.0f;

	*out_samplesPerSecond = static_cast<float32>(samplesPerMs) * 1000.0f;
}

// command interface

Renderer::Renderer(const InputPacket& packet) :
	Renderer()
{}

SdlTypeInfo Renderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "renderer");
}

void Renderer::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph