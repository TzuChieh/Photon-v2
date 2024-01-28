#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Core/Receiver/Receiver.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Core/Receiver/Receiver.h"

#include <Common/primitive_type.h>
#include <Common/logging.h>

#include <utility>
#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(Renderer, Renderer);

Renderer::Renderer(Viewport viewport, const uint32 numWorkers) : 
	m_viewport  (std::move(viewport)),
	m_numWorkers(numWorkers)
{
	PH_ASSERT(m_viewport.hasView());
	PH_ASSERT_GE(m_numWorkers, 1);
}

Renderer::~Renderer() = default;

void Renderer::update(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	PH_LOG(Renderer, Note, "# render workers = {}", numWorkers());
	PH_LOG(Renderer, Note, "render dimensions = {}", getViewport().toString());
	PH_LOG(Renderer, Note, "actual render resolution = {}", getRenderRegionPx().getExtents().toString());

	PH_LOG(Renderer, Note, "updating...");

	Timer updateTimer;
	updateTimer.start();
	m_isUpdating = true;

	doUpdate(cooked, world);

	m_isUpdating = false;
	updateTimer.stop();

	PH_LOG(Renderer, Note, "update time: {} ms", updateTimer.getDeltaMs());
}

void Renderer::render()
{
	PH_LOG(Renderer, Note, "rendering...");

	Timer renderTimer;
	renderTimer.start();
	m_isRendering = true;

	doRender();

	m_isRendering = false;
	renderTimer.stop();

	PH_LOG(Renderer, Note, "render time: {} ms", renderTimer.getDeltaMs());
}

void Renderer::setNumWorkers(uint32 numWorkers)
{
	if(numWorkers == 0)
	{
		PH_LOG(Renderer, Warning, "# workers cannot be 0, set to 1");
		numWorkers = 1;
	}

	m_numWorkers = numWorkers;
}

std::size_t Renderer::asyncPollMergedUpdatedRegions(
	TSpan<RenderRegionStatus> out_regions,
	std::size_t mergeSize)
{
	auto numRegions = asyncPollUpdatedRegions(out_regions);
	mergeSize = mergeSize == 0 ? 1 : mergeSize;

	// Merge regions in-place with a group size == `mergeSize`
	for(std::size_t ri = 0; ri < out_regions.size(); ++ri)
	{
		const auto mergedRegionIdx = ri / mergeSize;
		out_regions[mergedRegionIdx] = out_regions[mergedRegionIdx].getMerged(out_regions[ri]);
	}

	return math::ceil_div(out_regions.size(), mergeSize);
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
//	const auto states = asyncQueryRenderStats();
//	*out_samplesPerSecond = static_cast<float32>(states.fltStates[0]) * 1000.0f;
//}

}// end namespace ph
