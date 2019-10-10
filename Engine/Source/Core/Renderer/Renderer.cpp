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
#include "Common/Logger.h"
#include "FileIO/SDL/InputPacket.h"
#include "FileIO/SDL/DataTreatment.h"

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <utility>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Renderer"));
}

Renderer::~Renderer() = default;

void Renderer::update(const SdlResourcePack& data)
{
	logger.log("# render workers = " + std::to_string(numWorkers()));
	logger.log("render region = " + getCropWindowPx().toString());

	logger.log("updating...");

	Timer updateTimer;
	updateTimer.start();
	m_isUpdating.store(true, std::memory_order_relaxed);

	doUpdate(data);

	m_isUpdating.store(false, std::memory_order_relaxed);
	updateTimer.finish();

	logger.log("update time: " + std::to_string(updateTimer.getDeltaMs()) + " ms");
}

void Renderer::render()
{
	logger.log("rendering...");

	Timer renderTimer;
	renderTimer.start();
	m_isRendering.store(true, std::memory_order_relaxed);

	doRender();

	m_isRendering.store(false, std::memory_order_relaxed);
	renderTimer.finish();

	logger.log("render time: " + std::to_string(renderTimer.getDeltaMs()) + " ms");
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

Renderer::Renderer(const InputPacket& packet) : 
	m_isUpdating(false),
	m_isRendering(false)
{
	setNumWorkers(1);

	const integer filmWidth  = packet.getInteger("width",  1280, DataTreatment::REQUIRED());
	const integer filmHeight = packet.getInteger("height", 720,  DataTreatment::REQUIRED());
	const integer rectX      = packet.getInteger("rect-x", 0);
	const integer rectY      = packet.getInteger("rect-y", 0);
	const integer rectW      = packet.getInteger("rect-w", filmWidth);
	const integer rectH      = packet.getInteger("rect-h", filmHeight);

	m_widthPx      = filmWidth;
	m_heightPx     = filmHeight;
	m_cropWindowPx = TAABB2D<int64>({rectX, rectY}, {rectX + rectW, rectY + rectH});
}

SdlTypeInfo Renderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "renderer");
}

void Renderer::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
