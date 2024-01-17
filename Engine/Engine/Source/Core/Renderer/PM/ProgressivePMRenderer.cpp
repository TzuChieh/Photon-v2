#include "Core/Renderer/PM/ProgressivePMRenderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Renderer/PM/FullViewpoint.h"
#include "Core/Renderer/PM/TPhotonPathTracingWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "Core/Renderer/PM/TPPMViewpointCollector.h"
#include "Core/Renderer/PM/PPMRadianceEvaluationWork.h"
#include "Core/Renderer/RenderObservationInfo.h"
#include "Core/Renderer/RenderProgress.h"
#include "Core/Renderer/RenderStats.h"
#include "Utility/Concurrent/concurrent.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "Utility/Timer.h"

#include <Common/profiling.h>

#include <utility>
#include <vector>
#include <numeric>
#include <cmath>

namespace ph
{

ProgressivePMRenderer::ProgressivePMRenderer(
	PMCommonParams commonParams,
	Viewport       viewport,
	SampleFilter   filter,
	uint32         numWorkers)

	: PMRendererBase(
		commonParams,
		viewport,
		std::move(filter),
		numWorkers)

	, m_photonsPerSecond(0)
{}

void ProgressivePMRenderer::doRender()
{
	PH_LOG(PMRenderer, "rendering mode: progressive photon mapping");

	m_photonsPerSecond.store(0, std::memory_order_relaxed);
	renderWithProgressivePM();
}

void ProgressivePMRenderer::renderWithProgressivePM()
{
	PH_PROFILE_SCOPE();

	using Photon    = FullPhoton;
	using Viewpoint = FullViewpoint;

	PH_LOG(PMRenderer, "start gathering viewpoints...");

	std::vector<Viewpoint> viewpoints;
	{
		using ViewpointCollector = TPPMViewpointCollector<Viewpoint>;
		ViewpointCollector viewpointCollector(6, getCommonParams().kernelRadius);

		auto viewpointSampleGenerator = getSampleGenerator()->genCopied(getCommonParams().numSamplesPerPixel);

		TViewPathTracingWork<ViewpointCollector> viewpointWork(
			&viewpointCollector,
			getScene(),
			getReceiver(),
			viewpointSampleGenerator.get(),
			getPrimaryFilm()->getSampleWindowPx(),
			getRenderRegionPx().getExtents());

		viewpointWork.work();

		viewpoints = viewpointCollector.claimViewpoints();
	}
	
	PH_LOG(PMRenderer, "viewpoint size: {} bytes", sizeof(Viewpoint));
	PH_LOG(PMRenderer, "size of viewpoint buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Viewpoint) * viewpoints.size()));

	const std::size_t numPhotonsPerPass = getCommonParams().numPhotons;

	PH_LOG(PMRenderer, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PMRenderer, "number of photons per pass: {}", numPhotonsPerPass);
	PH_LOG(PMRenderer, "size of photon buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Photon) * numPhotonsPerPass));

	PH_LOG(PMRenderer, "start accumulating passes...");

	TSynchronized<HdrRgbFilm> resultFilm(HdrRgbFilm(
		getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), getFilter()));

	Timer passTimer;
	std::size_t numFinishedPasses = 0;
	std::size_t totalPhotonPaths  = 0;
	while(numFinishedPasses < getCommonParams().numPasses)
	{
		passTimer.start();
		std::vector<Photon> photonBuffer(numPhotonsPerPass);

		std::vector<std::size_t> numPhotonPaths(numWorkers(), 0);
		parallel_work(numPhotonsPerPass, numWorkers(),
			[this, &photonBuffer, &numPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				auto sampleGenerator = getSampleGenerator()->genCopied(1);

				TPhotonPathTracingWork<Photon> photonTracingWork(
					getScene(),
					getReceiver(),
					sampleGenerator.get(),
					{photonBuffer.data() + workStart, workEnd - workStart},
					getCommonParams().minPhotonPathLength,
					getCommonParams().maxPhotonPathLength);
				photonTracingWork.setStatistics(&getStatistics());

				photonTracingWork.work();

				numPhotonPaths[workerIdx] = photonTracingWork.numPhotonPaths();
			});
		totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), totalPhotonPaths);

		TPhotonMap<Photon> photonMap;
		photonMap.map.build(std::move(photonBuffer));

		parallel_work(viewpoints.size(), numWorkers(),
			[this, &photonMap, &viewpoints, &resultFilm, totalPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				HdrRgbFilm film(getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), getFilter());

				PPMRadianceEvaluationWork radianceEstimator(
					&photonMap, 
					totalPhotonPaths,
					&film,
					&(viewpoints[workStart]),
					workEnd - workStart,
					getScene());
				radianceEstimator.setStatistics(&getStatistics());

				radianceEstimator.work();

				resultFilm->mergeWith(film);
			});

		asyncReplacePrimaryFilm(resultFilm.unsafeGetReference());
		resultFilm.unsafeGetReference().clear();

		passTimer.stop();

		const auto passTimeMs   = passTimer.getDeltaMs<float64>();
		const auto photonsPerMs = passTimeMs != 0 ? numPhotonsPerPass / passTimeMs : 0;
		m_photonsPerSecond.store(static_cast<std::uint64_t>(photonsPerMs * 1000 + 0.5), std::memory_order_relaxed);

		getStatistics().incrementNumIterations();
		++numFinishedPasses;
	}// end while more pass needed
}

void ProgressivePMRenderer::retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	ProgressivePMRenderer::asyncPeekFrame(layerIndex, getRenderRegionPx(), out_frame);
}

RenderStats ProgressivePMRenderer::asyncQueryRenderStats()
{
	PH_PROFILE_SCOPE();

	RenderStats stats;
	stats.setInteger(0, getStatistics().getNumIterations());
	stats.setInteger(1, getStatistics().getNumTracedPhotons());
	stats.setInteger(2, static_cast<RenderStats::IntegerType>(m_photonsPerSecond.load(std::memory_order_relaxed)));
	return stats;
}

RenderProgress ProgressivePMRenderer::asyncQueryRenderProgress()
{
	PH_PROFILE_SCOPE();

	return RenderProgress(
		getCommonParams().numPasses,
		getStatistics().getNumIterations(),
		0);
}

void ProgressivePMRenderer::asyncPeekFrame(
	std::size_t layerIndex,
	const Region& region,
	HdrRgbFrame& out_frame)
{
	PH_PROFILE_SCOPE();

	if(layerIndex == 0)
	{
		asyncDevelopPrimaryFilm(region, out_frame);
	}
	else
	{
		out_frame.fill(0, math::TAABB2D<uint32>(region));
	}
}

RenderObservationInfo ProgressivePMRenderer::getObservationInfo() const
{
	RenderObservationInfo info;
	info.setIntegerStat(0, "finished passes");
	info.setIntegerStat(1, "traced photons");
	info.setIntegerStat(2, "photons/second");
	return info;
}

}// end namespace ph
