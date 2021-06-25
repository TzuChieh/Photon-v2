#include "Core/Renderer/Renderer.h"
#include "Common/primitive_type.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Core/Receiver/Receiver.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Common/Logger.h"
#include "Core/CoreDataGroup.h"

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

void Renderer::update(const CoreDataGroup& data)
{
	logger.log("# render workers = " + std::to_string(numWorkers()));

	const auto resolution = data.getReceiver()->getRasterResolution();
	// HACK
	m_widthPx = static_cast<uint32>(resolution.x);
	m_heightPx = static_cast<uint32>(resolution.y);

	// TODO: render region is the intersection of crop window and resolution
	// Nothing is cropped if no suitable window is present
	if(!m_cropWindowPx.isValid() || m_cropWindowPx.getArea() == 0)
	{
		m_cropWindowPx = TAABB2D<int64>(
			{0, 0}, 
			{m_widthPx, m_heightPx});
	}
	logger.log("render region = " + getCropWindowPx().toString());

	logger.log("updating...");

	Timer updateTimer;
	updateTimer.start();
	m_isUpdating = true;

	doUpdate(data);

	m_isUpdating = false;
	updateTimer.finish();

	logger.log("update time: " + std::to_string(updateTimer.getDeltaMs()) + " ms");
}

void Renderer::render()
{
	logger.log("rendering...");

	Timer renderTimer;
	renderTimer.start();
	m_isRendering = true;

	doRender();

	m_isRendering = false;
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

}// end namespace ph
