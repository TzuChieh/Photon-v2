#include "Core/Renderer/PM/VanillaPMRenderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Renderer/PM/TPhotonPathTracingWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TVPMRadianceEvaluator.h"
#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "Core/Renderer/RenderObservationInfo.h"
#include "Core/Renderer/RenderProgress.h"
#include "Core/Renderer/RenderStats.h"
#include "Math/math.h"
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
	PH_LOG(PMRenderer, Note, "rendering mode: vanilla photon mapping");

	m_photonsPerSecond.store(0, std::memory_order_relaxed);
	renderWithVanillaPM();
}

void VanillaPMRenderer::renderWithVanillaPM()
{
	PH_PROFILE_SCOPE();

	using Photon = FullPhoton;

	PH_LOG(PMRenderer, Note, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PMRenderer, Note, "target number of photons: {}", getCommonParams().numPhotons);
	PH_LOG(PMRenderer, Note, "size of photon buffer: {} MB", sizeof(Photon) * getCommonParams().numPhotons / 1024 / 1024);
	PH_LOG(PMRenderer, Note, "start shooting photons...");

	Timer tracePhotonTimer;
	tracePhotonTimer.start();

	std::vector<Photon> photonBuffer(getCommonParams().numPhotons);
	std::vector<std::size_t> numPhotonPaths(numWorkers(), 0);
	parallel_work(getCommonParams().numPhotons, numWorkers(),
		[this, &photonBuffer, &numPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
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

	tracePhotonTimer.stop();
	const auto photonsPerSecond = getCommonParams().numPhotons / tracePhotonTimer.getDeltaS<float64>();
	m_photonsPerSecond.store(
		std::isfinite(photonsPerSecond) ? static_cast<uint64>(photonsPerSecond + 0.5) : 0,
		std::memory_order_relaxed);

	PH_LOG(PMRenderer, Note, "building photon map...");

	TPhotonMap<Photon> photonMap;
	photonMap.map.build(std::move(photonBuffer));
	photonMap.numPaths = math::summation(numPhotonPaths);

	PH_LOG(PMRenderer, Note, "estimating radiance...");

	parallel_work(getCommonParams().numSamplesPerPixel, numWorkers(),
		[this, &photonMap](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto sampleGenerator = getSampleGenerator()->makeNewborn(workEnd - workStart);
			auto film            = std::make_unique<HdrRgbFilm>(
				getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), getFilter());

			using RadianceEvaluator = TVPMRadianceEvaluator<FullPhoton>;

			RadianceEvaluator evaluator(
				&photonMap,
				getScene(),
				film.get());
			evaluator.setStatistics(&getStatistics());
			evaluator.setKernelRadius(getCommonParams().kernelRadius);
			evaluator.setStochasticPathSampleBeginLength(5);
			evaluator.setGlossyMergeBeginLength(getCommonParams().glossyMergeBeginLength);

			TViewPathTracingWork<RadianceEvaluator> radianceEvaluator(
				&evaluator,
				getScene(),
				getReceiver(),
				sampleGenerator.get(),
				film->getSampleWindowPx(),
				getRenderRegionPx().getExtents());

			radianceEvaluator.work();

			asyncMergeToPrimaryFilm(*film);
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
	stats.setInteger(0, getStatistics().numProcessedSteps());
	stats.setInteger(1, getStatistics().numTracedPhotons());
	stats.setInteger(2, static_cast<RenderStats::IntegerType>(m_photonsPerSecond.load(std::memory_order_relaxed)));
	return stats;
}

RenderProgress VanillaPMRenderer::asyncQueryRenderProgress()
{
	PH_PROFILE_SCOPE();

	return RenderProgress(
		getCommonParams().numPhotons,
		getStatistics().numTracedPhotons(),
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
