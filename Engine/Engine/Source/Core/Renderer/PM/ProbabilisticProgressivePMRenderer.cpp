#include "Core/Renderer/PM/ProbabilisticProgressivePMRenderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Renderer/PM/FullViewpoint.h"
#include "Core/Renderer/PM/TPhotonPathTracingWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TVPMRadianceEvaluator.h"
#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "Core/Renderer/RenderObservationInfo.h"
#include "Core/Renderer/RenderProgress.h"
#include "Core/Renderer/RenderStats.h"
#include "Math/math.h"
#include "Utility/TSpan.h"
#include "Utility/Concurrent/concurrent.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "Utility/Timer.h"

#include <Common/profiling.h>
#include <Common/assertion.h>

#include <utility>
#include <vector>
#include <numeric>
#include <cmath>
#include <optional>

namespace ph
{

namespace
{

template<CPhoton Photon>
struct TPPPMIteration final
{
	using PhotonMapType = TPhotonMap<Photon, TSpan<Photon>>;

	/*! Stores the evaluation result for current iteration. */
	HdrRgbFilm film;

	/*! The photon's radius (bandwidth) for the iteration. */
	real kernelRadius;

	std::vector<Photon> photonBuffer;
	PhotonMapType photonMap;
	PhotonMapType::BuildCacheType photonMapBuildCache;

	std::unique_ptr<SampleGenerator> photonSampleGenerator;
	std::unique_ptr<SampleGenerator> viewSampleGenerator;
};

class KernelRadiusDispatcher final
{
public:
	KernelRadiusDispatcher(const real initialRadius, const real alpha, const std::uint64_t maxIterations)
		: m_r1(initialRadius)
		, m_exponent((alpha - 1.0_r) / 2.0_r)
		, m_iterationNumber(1)
		, m_maxIterations(maxIterations)
	{
		PH_ASSERT_IN_RANGE_EXCLUSIVE(alpha, 0.0_r, 1.0_r);
		PH_ASSERT_GE(m_maxIterations, m_iterationNumber);
	}

	/*! @brief Dispatch the next kernel radius.
	
	Note that this method computes the radius differently to the original paper [1], where they
	uses an iterative approach that is equivalent to the original SPPM. This method computes the
	radius directly for any iteration number using an asymptotically equivalent one introduced in [2]
	(equation C.1).

	References:
	[1] Claude Knaus and Matthias Zwicker, "Progressive Photon Mapping: A Probabilistic Approach",
	ACM Transactions on GraphicsVolume 30 Issue 3, 2011
	[2] Anton S. Kaplanyan and Carsten Dachsbacher, "Adaptive Progressive Photon Mapping",
	ACM Transactions on Graphics Volume 32 Issue 2, 2013

	@note Thread-safe.
	*/
	std::optional<real> nextRadius()
	{
		const auto N = m_iterationNumber.fetch_add(1, std::memory_order_relaxed);

		PH_ASSERT_GE(N, 1);
		PH_ASSERT_GT(m_r1, 0.0_r);

		return N <= m_maxIterations
			? std::make_optional(rN(N))
			: std::nullopt;
	}

private:
	real rN(const std::uint64_t N) const
	{
		return static_cast<real>(m_r1 * std::pow(N, m_exponent));
	}

	real m_r1;
	real m_exponent;
	std::atomic_uint64_t m_iterationNumber;
	std::uint64_t m_maxIterations;
};

}// end anonymous namespace

ProbabilisticProgressivePMRenderer::ProbabilisticProgressivePMRenderer(
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

void ProbabilisticProgressivePMRenderer::doRender()
{
	PH_LOG(PMRenderer, Note, "rendering mode: probabilistic progressive photon mapping");

	m_photonsPerSecond.store(0, std::memory_order_relaxed);
	renderWithProbabilisticProgressivePM();
}

void ProbabilisticProgressivePMRenderer::renderWithProbabilisticProgressivePM()
{
	PH_PROFILE_SCOPE();

	using Photon        = FullPhoton;
	using Viewpoint     = FullViewpoint;
	using PPPMIteration = TPPPMIteration<Photon>;

	const std::size_t numPhotonsPerPass = getCommonParams().numPhotons;

	PH_LOG(PMRenderer, Note, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PMRenderer, Note, "number of photons per pass: {}", numPhotonsPerPass);
	const auto singlePhotonBufferMBs = math::bytes_to_MiB<real>(sizeof(Photon) * numPhotonsPerPass);
	PH_LOG(PMRenderer, Note, "total size of photon buffer: {} MiB ({} MiB per worker)",
		singlePhotonBufferMBs * numWorkers(), singlePhotonBufferMBs);

	std::vector<PPPMIteration> pppmIterations(numWorkers());
	for(PPPMIteration& pppmIteration : pppmIterations)
	{
		// Each worker works independently for the full region
		pppmIteration.film = HdrRgbFilm(
			getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), getFilter());
		pppmIteration.kernelRadius = -1.0_r;
		pppmIteration.photonBuffer = std::vector<Photon>(numPhotonsPerPass);
		pppmIteration.photonMap = PPPMIteration::PhotonMapType{};
		pppmIteration.photonMap.minPathLength = getCommonParams().minPhotonPathLength;
		pppmIteration.photonMap.maxPathLength = getCommonParams().maxPhotonPathLength;
		pppmIteration.photonSampleGenerator = getSampleGenerator()->makeNewborn(1);
		pppmIteration.viewSampleGenerator = getSampleGenerator()->makeNewborn(
			getCommonParams().numSamplesPerPixel);
	}

	PH_LOG(PMRenderer, Note, "start accumulating passes...");

	KernelRadiusDispatcher radiusDispatcher(
		getCommonParams().kernelRadius, getCommonParams().alpha, getCommonParams().numPasses);

	parallel_work(numWorkers(), 
	[this, numPhotonsPerPass, &pppmIterations, &radiusDispatcher](
		const std::size_t workerIdx)
	{
		PPPMIteration& pppmIteration = pppmIterations[workerIdx];
		while(true)
		{
			PH_PROFILE_NAMED_SCOPE("PPPM iteration (pass)");

			Timer iterationTimer;
			iterationTimer.start();

			auto optNextRadius = radiusDispatcher.nextRadius();
			if(!optNextRadius)
			{
				break;
			}

			pppmIteration.kernelRadius = *optNextRadius;
			//PH_DEBUG_LOG(PMRenderer, "PPPM r = {}", *optNextRadius);

			{
				PH_PROFILE_NAMED_SCOPE("PPPM photon shooting");

				SampleGenerator* photonSampleGenerator = pppmIteration.photonSampleGenerator.get();

				TPhotonPathTracingWork<Photon> photonTracingWork(
					getScene(),
					getReceiver(),
					photonSampleGenerator,
					pppmIteration.photonBuffer,
					getCommonParams().minPhotonPathLength,
					getCommonParams().maxPhotonPathLength);
				photonTracingWork.setStatistics(&getStatistics());

				photonTracingWork.work();

				pppmIteration.photonMap.numPaths = photonTracingWork.numPhotonPaths();
				photonSampleGenerator->rebirth();
			}

			{
				PH_PROFILE_NAMED_SCOPE("PPPM build photon map");

				pppmIteration.photonMap.map.build(
					pppmIteration.photonBuffer, pppmIteration.photonMapBuildCache);
			}

			{
				PH_PROFILE_NAMED_SCOPE("PPPM energy estimation");

				using RadianceEvaluator = TVPMRadianceEvaluator<Photon, PPPMIteration::PhotonMapType>;

				SampleGenerator* viewSampleGenerator = pppmIteration.viewSampleGenerator.get();
				pppmIteration.film.clear();

				RadianceEvaluator evaluator(
					&pppmIteration.photonMap,
					getScene(),
					&pppmIteration.film);
				evaluator.setStatistics(&getStatistics());
				evaluator.setKernelRadius(pppmIteration.kernelRadius);
				evaluator.setStochasticPathSampleBeginLength(5);

				TViewPathTracingWork<RadianceEvaluator> radianceEvaluator(
					&evaluator,
					getScene(),
					getReceiver(),
					viewSampleGenerator,
					pppmIteration.film.getSampleWindowPx(),
					getRenderRegionPx().getExtents());

				radianceEvaluator.work();

				asyncMergeToPrimaryFilm(pppmIteration.film);
				viewSampleGenerator->rebirth();
			}

			iterationTimer.stop();

			const auto iterationTimeMs = iterationTimer.getDeltaMs<float64>();
			const auto photonsPerMs = iterationTimeMs != 0 ? numPhotonsPerPass / iterationTimeMs : 0;
			m_photonsPerSecond.store(static_cast<std::uint64_t>(photonsPerMs * 1000 + 0.5), std::memory_order_relaxed);
		}// end while more iterations (passes) needed
	});// end parallel work
}

void ProbabilisticProgressivePMRenderer::retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	ProbabilisticProgressivePMRenderer::asyncPeekFrame(layerIndex, getRenderRegionPx(), out_frame);
}

RenderStats ProbabilisticProgressivePMRenderer::asyncQueryRenderStats()
{
	PH_PROFILE_SCOPE();

	RenderStats stats;
	stats.setInteger(0, getStatistics().numProcessedSteps());
	stats.setInteger(1, getStatistics().numTracedPhotons());
	stats.setInteger(2, static_cast<RenderStats::IntegerType>(m_photonsPerSecond.load(std::memory_order_relaxed)));
	return stats;
}

RenderProgress ProbabilisticProgressivePMRenderer::asyncQueryRenderProgress()
{
	PH_PROFILE_SCOPE();

	return RenderProgress(
		getCommonParams().numPasses * getCommonParams().numSamplesPerPixel,
		getStatistics().numProcessedSteps(),
		0);
}

void ProbabilisticProgressivePMRenderer::asyncPeekFrame(
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

RenderObservationInfo ProbabilisticProgressivePMRenderer::getObservationInfo() const
{
	RenderObservationInfo info;
	info.setIntegerStat(0, "finished passes");
	info.setIntegerStat(1, "traced photons");
	info.setIntegerStat(2, "photons/second");
	return info;
}

}// end namespace ph
