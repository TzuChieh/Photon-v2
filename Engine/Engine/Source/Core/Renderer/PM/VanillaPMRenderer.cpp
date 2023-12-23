#include "Core/Renderer/PM/VanillaPMRenderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Renderer/PM/TPhotonMappingWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/VPMRadianceEvaluator.h"
#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "Core/Renderer/RenderObservationInfo.h"
#include "Core/Renderer/RenderProgress.h"
#include "Core/Renderer/RenderStats.h"
#include "Utility/Concurrent/concurrent.h"
#include "Utility/Timer.h"

#include <Common/profiling.h>

#include <utility>
#include <vector>
#include <numeric>
#include <cmath>

namespace ph
{

VanillaPMRenderer::VanillaPMRenderer(
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

void VanillaPMRenderer::doRender()
{
	PH_LOG(PMRenderer, "rendering mode: vanilla photon mapping");

	renderWithVanillaPM();
}

void VanillaPMRenderer::renderWithVanillaPM()
{
	PH_PROFILE_SCOPE();

	using Photon = FullPhoton;

	PH_LOG(PMRenderer, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PMRenderer, "target number of photons: {}", getCommonParams().numPhotons);
	PH_LOG(PMRenderer, "size of photon buffer: {} MB", sizeof(Photon) * getCommonParams().numPhotons / 1024 / 1024);
	PH_LOG(PMRenderer, "start shooting photons...");

	Timer tracePhotonTimer;
	tracePhotonTimer.start();
	m_photonsPerSecond.store(0, std::memory_order_relaxed);

	std::vector<Photon> photonBuffer(getCommonParams().numPhotons);
	std::vector<std::size_t> numPhotonPaths(numWorkers(), 0);
	parallel_work(getCommonParams().numPhotons, numWorkers(),
		[this, &photonBuffer, &numPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
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
	const std::size_t totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), std::size_t(0));

	tracePhotonTimer.stop();
	const auto photonsPerSecond = getCommonParams().numPhotons / tracePhotonTimer.getDeltaS<float64>();
	m_photonsPerSecond.store(
		std::isfinite(photonsPerSecond) ? static_cast<uint64>(photonsPerSecond + 0.5) : 0,
		std::memory_order_relaxed);

	PH_LOG(PMRenderer, "building photon map...");

	TPhotonMap<Photon> photonMap(2, TPhotonCenterCalculator<Photon>());
	photonMap.build(std::move(photonBuffer));

	PH_LOG(PMRenderer, "estimating radiance...");

	parallel_work(getCommonParams().numSamplesPerPixel, numWorkers(),
		[this, &photonMap, totalPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto sampleGenerator = getSampleGenerator()->genCopied(workEnd - workStart);
			auto film            = std::make_unique<HdrRgbFilm>(
				getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), getFilter());

			VPMRadianceEvaluator evaluator(
				&photonMap, 
				totalPhotonPaths, 
				film.get(), 
				getScene());
			evaluator.setRenderer(this);
			evaluator.setStatistics(&getStatistics());
			evaluator.setKernelRadius(getCommonParams().kernelRadius);

			TViewPathTracingWork<VPMRadianceEvaluator> radianceEvaluator(
				&evaluator,
				getScene(),
				getReceiver(),
				sampleGenerator.get(),
				getRenderRegionPx());

			radianceEvaluator.work();
		});
}

void VanillaPMRenderer::retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	VanillaPMRenderer::asyncPeekFrame(layerIndex, getRenderRegionPx(), out_frame);
}

RenderStats VanillaPMRenderer::asyncQueryRenderStats()
{
	PH_PROFILE_SCOPE();

	RenderStats stats;
	stats.setInteger(0, getStatistics().getNumIterations());
	stats.setInteger(1, getStatistics().getNumTracedPhotons());
	stats.setInteger(2, static_cast<RenderStats::IntegerType>(m_photonsPerSecond.load(std::memory_order_relaxed)));
	return stats;
}

RenderProgress VanillaPMRenderer::asyncQueryRenderProgress()
{
	PH_PROFILE_SCOPE();

	return RenderProgress(
		getCommonParams().numPhotons,
		getStatistics().getNumTracedPhotons(),
		0);
}

void VanillaPMRenderer::asyncPeekFrame(
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

RenderObservationInfo VanillaPMRenderer::getObservationInfo() const
{
	RenderObservationInfo info;
	info.setIntegerStat(0, "finished samples");
	info.setIntegerStat(1, "traced photons");
	info.setIntegerStat(2, "photons/second");
	return info;
}

}// end namespace ph
