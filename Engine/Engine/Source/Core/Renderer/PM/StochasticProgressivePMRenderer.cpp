#include "Core/Renderer/PM/StochasticProgressivePMRenderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Renderer/PM/FullViewpoint.h"
#include "Core/Renderer/PM/TPhotonPathTracingWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "Core/Renderer/PM/TSPPMRadianceEvaluator.h"
#include "Core/Renderer/RenderObservationInfo.h"
#include "Core/Renderer/RenderProgress.h"
#include "Core/Renderer/RenderStats.h"
#include "Math/math.h"
#include "Utility/Concurrent/concurrent.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "Utility/Timer.h"

#include <Common/profiling.h>

#include <utility>
#include <vector>
#include <numeric>
#include <cmath>

namespace ph
{

namespace
{

template<CViewpoint Viewpoint>
struct TSPPMRadianceEvaluationRegion final
{
	/*! The area within the film to do evaluation. */
	Region region;

	/*! The film to store the evaluation result. */
	HdrRgbFilm film;

	/*! The resolution of the stored statistics for evaluation. */
	math::TVector2<int64> statisticsRes;

	/*! Buffer that stores the statistics. */
	std::vector<Viewpoint> viewpoints;

	std::unique_ptr<SampleGenerator> viewSampleGenerator;
};

}// end anonymous namespace

StochasticProgressivePMRenderer::StochasticProgressivePMRenderer(
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

void StochasticProgressivePMRenderer::doRender()
{
	PH_LOG(PMRenderer, Note, "rendering mode: stochastic progressive photon mapping");

	m_photonsPerSecond.store(0, std::memory_order_relaxed);
	renderWithStochasticProgressivePM();
}

void StochasticProgressivePMRenderer::renderWithStochasticProgressivePM()
{
	PH_PROFILE_SCOPE();

	using Photon                   = FullPhoton;
	using Viewpoint                = FullViewpoint;
	using RadianceEvaluationRegion = TSPPMRadianceEvaluationRegion<Viewpoint>;

	// TODO: log once for viewpoint not needing raster coord

	FixedSizeThreadPool workers(numWorkers());

	PH_LOG(PMRenderer, Note, "start generating viewpoints...");

	// Keep the same number of statistics in x & y so the image will not look stretched
	auto numStatisticsPerDim = static_cast<int64>(std::sqrt(getCommonParams().numSamplesPerPixel));
	numStatisticsPerDim = numStatisticsPerDim != 0 ? numStatisticsPerDim : 1;

	std::size_t totalViewpoints = 0;

	std::vector<RadianceEvaluationRegion> radianceEvalRegions(numWorkers());
	for(std::size_t workerIdx = 0; workerIdx < numWorkers(); ++workerIdx)
	{
		RadianceEvaluationRegion& radianceEvalRegion = radianceEvalRegions[workerIdx];

		// Divide render region on y-axis, so iterations will be more cache friendly on each worker
		const auto [workBegin, workEnd] = math::ith_evenly_divided_range(
			workerIdx, getRenderRegionPx().getHeight(), numWorkers());

		Region region = getRenderRegionPx();
		auto [minVertex, maxVertex] = region.getVertices();
		minVertex.y() = getRenderRegionPx().getMinVertex().y() + workBegin;
		maxVertex.y() = getRenderRegionPx().getMinVertex().y() + workEnd;
		region.setVertices({minVertex, maxVertex});

		// Resolution of photon statistics gathered. If more than one statistics per pixel are
		// gathered, we evenly divide them within the pixel.
		math::TVector2<int64> statisticsRes = region.getExtents() * numStatisticsPerDim;

		radianceEvalRegion.region = region;
		radianceEvalRegion.film = HdrRgbFilm(
			getRenderWidthPx(), getRenderHeightPx(), region, getFilter());
		radianceEvalRegion.statisticsRes = statisticsRes;

		radianceEvalRegion.viewpoints.resize(statisticsRes.product());
		for(Viewpoint& viewpoint : radianceEvalRegion.viewpoints)
		{
			if constexpr(Viewpoint::template has<EViewpointData::Radius>())
			{
				viewpoint.template set<EViewpointData::Radius>(getCommonParams().kernelRadius);
			}
			if constexpr(Viewpoint::template has<EViewpointData::NumPhotons>())
			{
				viewpoint.template set<EViewpointData::NumPhotons>(0.0_r);
			}
			if constexpr(Viewpoint::template has<EViewpointData::Tau>())
			{
				viewpoint.template set<EViewpointData::Tau>(math::Spectrum(0));
			}
			if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
			{
				viewpoint.template set<EViewpointData::ViewRadiance>(math::Spectrum(0));
			}
		}

		totalViewpoints += radianceEvalRegion.viewpoints.size();

		radianceEvalRegion.viewSampleGenerator = getSampleGenerator()->makeNewborn(1);
	}

	PH_LOG(PMRenderer, Note, "viewpoint (statistics record) resolution: {}", 
		getRenderRegionPx().getExtents() * numStatisticsPerDim);
	PH_LOG(PMRenderer, Note, "viewpoint size: {} bytes", sizeof(Viewpoint));
	PH_LOG(PMRenderer, Note, "size of viewpoint buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Viewpoint) * totalViewpoints));

	const std::size_t numPhotonsPerPass = getCommonParams().numPhotons;

	PH_LOG(PMRenderer, Note, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PMRenderer, Note, "number of photons per pass: {}", numPhotonsPerPass);
	PH_LOG(PMRenderer, Note, "size of photon buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Photon) * numPhotonsPerPass));

	TSynchronized<HdrRgbFilm> resultFilm(HdrRgbFilm(
		getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), getFilter()));

	std::vector<std::size_t> numPhotonPaths(numWorkers(), 0);
	std::vector<Photon> photonBuffer(numPhotonsPerPass);

	TPhotonMap<Photon> photonMap;
	photonMap.minPhotonPathLength = getCommonParams().minPhotonPathLength;
	photonMap.maxPhotonPathLength = getCommonParams().maxPhotonPathLength;

	PH_LOG(PMRenderer, Note, "start accumulating passes...");

	Timer passTimer;
	std::size_t numFinishedPasses = 0;
	while(numFinishedPasses < getCommonParams().numPasses)
	{
		PH_PROFILE_NAMED_SCOPE("SPPM pass");

		passTimer.start();

		parallel_work(workers, numPhotonsPerPass,
			[this, &photonBuffer, &numPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				PH_PROFILE_NAMED_SCOPE("SPPM photon shooting");

				auto sampleGenerator = getSampleGenerator()->makeNewborn(1);

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

		photonMap.map.build(photonBuffer);
		photonMap.numPhotonPaths = math::summation<std::size_t>(numPhotonPaths);

		for(RadianceEvaluationRegion& radianceEvalRegion : radianceEvalRegions)
		{
			workers.queueWork([this, &radianceEvalRegion, &photonMap, &resultFilm, numFinishedPasses]()
			{
				PH_PROFILE_NAMED_SCOPE("SPPM energy estimation");

				radianceEvalRegion.film.clear();
				SampleGenerator* viewSampleGenerator = radianceEvalRegion.viewSampleGenerator.get();

				using RadianceEvaluator = TSPPMRadianceEvaluator<Viewpoint, Photon>;

				RadianceEvaluator radianceEvaluator(
					radianceEvalRegion.viewpoints,
					&photonMap,
					getScene(),
					&radianceEvalRegion.film,
					radianceEvalRegion.region,
					radianceEvalRegion.statisticsRes,
					numFinishedPasses + 1);

				TViewPathTracingWork<RadianceEvaluator> viewpointWork(
					&radianceEvaluator,
					getScene(),
					getReceiver(),
					viewSampleGenerator,
					// Must from within the region, shared statistics do not need out-of-region samples
					math::TAABB2D<float64>(radianceEvalRegion.region),
					radianceEvalRegion.statisticsRes);

				viewpointWork.work();

				resultFilm->mergeWith(radianceEvalRegion.film);
				viewSampleGenerator->rebirth();
			});
		}
		workers.waitAllWorks();

		asyncReplacePrimaryFilm(resultFilm.unsafeGetReference());
		resultFilm.unsafeGetReference().clear();

		passTimer.stop();

		const auto passTimeMs   = passTimer.getDeltaMs<float64>();
		const auto photonsPerMs = passTimeMs != 0 ? numPhotonsPerPass / passTimeMs : 0;
		m_photonsPerSecond.store(static_cast<std::uint64_t>(photonsPerMs * 1000 + 0.5), std::memory_order_relaxed);

		getStatistics().incrementNumProcessedSteps();
		++numFinishedPasses;
	}// end while more pass needed
}

void StochasticProgressivePMRenderer::retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	StochasticProgressivePMRenderer::asyncPeekFrame(layerIndex, getRenderRegionPx(), out_frame);
}

RenderStats StochasticProgressivePMRenderer::asyncQueryRenderStats()
{
	PH_PROFILE_SCOPE();

	RenderStats stats;
	stats.setInteger(0, getStatistics().numProcessedSteps());
	stats.setInteger(1, getStatistics().numTracedPhotons());
	stats.setInteger(2, static_cast<RenderStats::IntegerType>(m_photonsPerSecond.load(std::memory_order_relaxed)));
	return stats;
}

RenderProgress StochasticProgressivePMRenderer::asyncQueryRenderProgress()
{
	PH_PROFILE_SCOPE();

	return RenderProgress(
		getCommonParams().numPasses,
		getStatistics().numProcessedSteps(),
		0);
}

void StochasticProgressivePMRenderer::asyncPeekFrame(
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

RenderObservationInfo StochasticProgressivePMRenderer::getObservationInfo() const
{
	RenderObservationInfo info;
	info.setIntegerStat(0, "finished passes");
	info.setIntegerStat(1, "traced photons");
	info.setIntegerStat(2, "photons/second");
	return info;
}

}// end namespace ph
