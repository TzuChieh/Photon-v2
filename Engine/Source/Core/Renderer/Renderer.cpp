#include "Core/Renderer/Renderer.h"
#include "Common/primitive_type.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Core/Camera/Camera.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Utility/Timer.h"
#include "Core/Renderer/Region/BulkScheduler.h"

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <utility>

namespace ph
{

Renderer::~Renderer() = default;

void Renderer::update(const SdlResourcePack& data)
{
	Timer updateTimer;
	updateTimer.start();

	doUpdate(data);

	updateTimer.finish();
	std::cout << "update time: " << updateTimer.getDeltaMs() << " ms" << std::endl;
}

void Renderer::render()
{
	Timer renderTimer;
	renderTimer.start();

	doRender();

	renderTimer.finish();
	std::cout << "render time: " << renderTimer.getDeltaMs() << " ms" << std::endl;
}

void Renderer::setNumWorkers(const uint32 numWorkers)
{
	m_numWorkers = numWorkers;

	/*m_workers.resize(numThreads);
	for(uint32 ti = 0; ti < numThreads; ti++)
	{
		m_workers[ti] = RenderWorker(RendererProxy(this), ti);
	}*/
}

// FIXME: without synchronizing, other threads may never observe m_workers being changed
//void Renderer::asyncQueryStatistics(float32* const out_percentageProgress, 
//                                    float32* const out_samplesPerSecond)
//{
//	uint64  totalWork     = 0;
//	uint64  totalWorkDone = 0;
//	for(auto& worker : m_workers)
//	{
//		const auto progress = worker.asyncQueryProgress();
//
//		// FIXME: this calculation can be wrong if there are more works than workers
//		totalWork     += progress.totalWork;
//		totalWorkDone += progress.workDone;
//	}
//
//	*out_percentageProgress = totalWork != 0 ? 
//		static_cast<float32>(totalWorkDone) / static_cast<float32>(totalWork) * 100.0f : 0.0f;
//
//	// HACK
//	const auto states = asyncQueryRenderStates();
//	*out_samplesPerSecond = static_cast<float32>(states.fltStates[0]) * 1000.0f;
//}

// command interface

Renderer::Renderer(const InputPacket& packet)
{
	setNumWorkers(1);

	const integer filmWidth  = packet.getInteger("width",  1280, DataTreatment::REQUIRED());
	const integer filmHeight = packet.getInteger("height", 720,  DataTreatment::REQUIRED());
	const integer rectX      = packet.getInteger("rect-x", 0);
	const integer rectY      = packet.getInteger("rect-y", 0);
	const integer rectW      = packet.getInteger("rect-w", filmWidth);
	const integer rectH      = packet.getInteger("rect-h", filmHeight);

	m_widthPx  = filmWidth;
	m_heightPx = filmHeight;
	m_windowPx = TAABB2D<int64>({rectX, rectY}, {rectX + rectW, rectY + rectH});

	const std::string regionSchedulerName = packet.getString("region-scheduler", "bulk");
	if(regionSchedulerName == "bulk")
	{
		m_regionScheduler = std::make_unique<BulkScheduler>();
	}
}

SdlTypeInfo Renderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "renderer");
}

void Renderer::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph