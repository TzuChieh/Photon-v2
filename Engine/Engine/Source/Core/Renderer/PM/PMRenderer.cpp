#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Filmic/SampleFilters.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "World/VisualWorld.h"
#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "Core/Renderer/PM/TPhotonMappingWork.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/VPMRadianceEvaluator.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"
#include "Utility/Concurrent/concurrent.h"
#include "Core/Renderer/PM/PPMRadianceEvaluationWork.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Renderer/PM/FullViewpoint.h"
#include "Utility/Timer.h"
#include "Core/Renderer/PM/TPPMViewpointCollector.h"
#include "Core/Renderer/PM/TSPPMRadianceEvaluator.h"

#include <Common/logging.h>
#include <Common/profiling.h>

#include <utility>
#include <numeric>
#include <vector>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PhotonMapRenderer, Renderer);

PMRenderer::PMRenderer(
	EPMMode mode,
	PMCommonParams commonParams,
	Viewport viewport,
	SampleFilter filter,
	uint32 numWorkers)

	: Renderer(viewport, numWorkers)

	, m_film(nullptr)

	, m_scene(nullptr)
	, m_receiver(nullptr)
	, m_sg(nullptr)
	, m_filter(std::move(filter))

	, m_mode(mode)
	, m_commonParams(commonParams)

	, m_filmMutex()
	, m_statistics()
	, m_photonsPerSecond()
	, m_isFilmUpdated()
{}

void PMRenderer::doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	m_film = std::make_unique<HdrRgbFilm>(getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), m_filter);

	m_scene = world.getScene();
	m_receiver = cooked.getReceiver();
	m_sg = cooked.getSampleGenerator();

	m_statistics.zero();
	m_photonsPerSecond = 0;
	m_isFilmUpdated.store(false);
}

void PMRenderer::doRender()
{
	PH_PROFILE_SCOPE();

	PH_LOG_WARNING(PhotonMapRenderer, "unsupported PM mode, renders nothing");
}

std::size_t PMRenderer::asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions)
{
	PH_PROFILE_SCOPE();

	if(out_regions.empty())
	{
		return 0;
	}

	if(m_isFilmUpdated.load())
	{
		out_regions[0] = RenderRegionStatus(getRenderRegionPx(), ERegionStatus::Updating);
		m_isFilmUpdated.store(false);
		return 1;
	}
	else
	{
		// Report region finished even if the film update flag is off, so we meet the required ordering
		out_regions[0] = RenderRegionStatus(getRenderRegionPx(), ERegionStatus::Finished);
		return 1;
	}
}

RenderProgress PMRenderer::asyncQueryRenderProgress()
{
	PH_PROFILE_SCOPE();

	return RenderProgress(
		m_mode != EPMMode::VANILLA ? m_commonParams.numPasses : m_commonParams.numSamplesPerPixel,
		m_statistics.getNumIterations(), 
		0);
}

void PMRenderer::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame)
{
	PH_PROFILE_SCOPE();

	std::lock_guard<std::mutex> lock(m_filmMutex);

	if(layerIndex == 0)
	{
		m_film->develop(out_frame, region);
	}
	else
	{
		out_frame.fill(0, math::TAABB2D<uint32>(region));
	}
}

void PMRenderer::retrieveFrame(const std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	PH_PROFILE_SCOPE();

	m_film->develop(out_frame);
}

RenderObservationInfo PMRenderer::getObservationInfo() const
{
	RenderObservationInfo info;
	info.setIntegerStat(0, m_mode != EPMMode::VANILLA ? "finished passes" : "finished samples");
	info.setIntegerStat(1, "traced photons");
	info.setIntegerStat(2, "photons/second");
	return info;
}

RenderStats PMRenderer::asyncQueryRenderStats()
{
	PH_PROFILE_SCOPE();

	RenderStats stats;
	stats.setInteger(0, m_statistics.getNumIterations());
	stats.setInteger(1, m_statistics.getNumTracedPhotons());
	stats.setInteger(2, static_cast<RenderStats::IntegerType>(m_photonsPerSecond.load(std::memory_order_relaxed)));
	return stats;
}

void PMRenderer::asyncMergeFilm(const HdrRgbFilm& srcFilm)
{
	{
		std::lock_guard<std::mutex> lock(m_filmMutex);

		m_film->mergeWith(srcFilm);
	}
	
	m_isFilmUpdated.store(true);
}

void PMRenderer::asyncReplaceFilm(const HdrRgbFilm& srcFilm)
{
	{
		std::lock_guard<std::mutex> lock(m_filmMutex);

		m_film->clear();
		m_film->mergeWith(srcFilm);
	}

	m_isFilmUpdated.store(true);
}

}// end namespace ph
