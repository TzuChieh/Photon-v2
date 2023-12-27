#include "Core/Renderer/PM/StochasticProgressivePMRenderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Renderer/PM/FullViewpoint.h"
#include "Core/Renderer/PM/TPhotonMappingWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "Core/Renderer/PM/TSPPMRadianceEvaluator.h"
#include "Core/Renderer/RenderObservationInfo.h"
#include "Core/Renderer/RenderProgress.h"
#include "Core/Renderer/RenderStats.h"
#include "Utility/Concurrent/concurrent.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"
#include "Utility/Timer.h"

#include <Common/profiling.h>

#include <utility>
#include <vector>
#include <numeric>
#include <cmath>

namespace ph
{

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
	PH_LOG(PMRenderer, "rendering mode: stochastic progressive photon mapping");

	m_photonsPerSecond.store(0, std::memory_order_relaxed);
	renderWithStochasticProgressivePM();
}

void StochasticProgressivePMRenderer::renderWithStochasticProgressivePM()
{
	PH_PROFILE_SCOPE();

	using Photon    = FullPhoton;
	using Viewpoint = FullViewpoint;

	FixedSizeThreadPool workers(numWorkers());

	PH_LOG(PMRenderer, "start generating viewpoints...");

	std::vector<Viewpoint> viewpoints(getRenderRegionPx().getArea());
	for(std::size_t y = 0; y < static_cast<std::size_t>(getRenderRegionPx().getHeight()); ++y)
	{
		for(std::size_t x = 0; x < static_cast<std::size_t>(getRenderRegionPx().getWidth()); ++x)
		{
			auto& viewpoint = viewpoints[y * getRenderRegionPx().getWidth() + x];

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

	std::mutex resultFilmMutex;
	auto resultFilm = std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), getFilter());

	Timer passTimer;
	std::size_t numFinishedPasses = 0;
	std::size_t totalPhotonPaths  = 0;
	while(numFinishedPasses < getCommonParams().numPasses)
	{
		PH_PROFILE_NAMED_SCOPE("SPPM pass");

		passTimer.start();
		std::vector<Photon> photonBuffer(numPhotonsPerPass);

		std::vector<std::size_t> numPhotonPaths(numWorkers(), 0);
		parallel_work(workers, numPhotonsPerPass,
			[this, &photonBuffer, &numPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				PH_PROFILE_NAMED_SCOPE("SPPM photon shooting");

				auto sampleGenerator = getSampleGenerator()->genCopied(1);

				TPhotonMappingWork<Photon> photonMappingWork(
					getScene(),
					getReceiver(),
					sampleGenerator.get(),
					&(photonBuffer[workStart]),
					workEnd - workStart,
					&(numPhotonPaths[workerIdx]));
				photonMappingWork.setStatistics(&getStatistics());

				photonMappingWork.work();
			});
		totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), totalPhotonPaths);

		TPhotonMap<Photon> photonMap(2, TPhotonCenterCalculator<Photon>());
		photonMap.build(std::move(photonBuffer));

		parallel_work(workers, getRenderRegionPx().getWidth(),
			[this, &photonMap, &viewpoints, &resultFilm, &resultFilmMutex, totalPhotonPaths, numFinishedPasses](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				PH_PROFILE_NAMED_SCOPE("SPPM energy estimation");

				Region region = getRenderRegionPx();
				auto [minVertex, maxVertex] = region.getVertices();
				minVertex.x() = getRenderRegionPx().getMinVertex().x() + workStart;
				maxVertex.x() = getRenderRegionPx().getMinVertex().x() + workEnd;
				region.setVertices({minVertex, maxVertex});

				auto sampleGenerator = getSampleGenerator()->genCopied(1);

				using RadianceEvaluator = TSPPMRadianceEvaluator<Viewpoint, Photon>;

				RadianceEvaluator radianceEvaluator(
					viewpoints.data(),
					viewpoints.size(),
					&photonMap,
					totalPhotonPaths,
					getScene(),
					resultFilm.get(),
					region,
					numFinishedPasses + 1,
					16384);

				TViewPathTracingWork<RadianceEvaluator> viewpointWork(
					&radianceEvaluator,
					getScene(),
					getReceiver(),
					sampleGenerator.get(),
					math::TAABB2D<float64>(region),
					region.getExtents());

				viewpointWork.work();
			});

		asyncReplacePrimaryFilm(*resultFilm);
		resultFilm->clear();

		passTimer.stop();

		const auto passTimeMs   = passTimer.getDeltaMs<float64>();
		const auto photonsPerMs = passTimeMs != 0 ? numPhotonsPerPass / passTimeMs : 0;
		m_photonsPerSecond.store(static_cast<std::uint64_t>(photonsPerMs * 1000 + 0.5), std::memory_order_relaxed);

		getStatistics().incrementNumIterations();
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
	stats.setInteger(0, getStatistics().getNumIterations());
	stats.setInteger(1, getStatistics().getNumTracedPhotons());
	stats.setInteger(2, static_cast<RenderStats::IntegerType>(m_photonsPerSecond.load(std::memory_order_relaxed)));
	return stats;
}

RenderProgress StochasticProgressivePMRenderer::asyncQueryRenderProgress()
{
	PH_PROFILE_SCOPE();

	return RenderProgress(
		getCommonParams().numPasses,
		getStatistics().getNumIterations(),
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
