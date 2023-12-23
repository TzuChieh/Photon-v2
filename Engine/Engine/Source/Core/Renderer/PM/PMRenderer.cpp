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

	if(m_mode == EPMMode::PROGRESSIVE)
	{
		PH_LOG(PhotonMapRenderer, "rendering mode: progressive photon mapping");

		renderWithProgressivePM();
	}
	else if(m_mode == EPMMode::STOCHASTIC_PROGRESSIVE)
	{
		PH_LOG(PhotonMapRenderer, "rendering mode: stochastic progressive photon mapping");

		renderWithStochasticProgressivePM();
	}
	else
	{
		PH_LOG_WARNING(PhotonMapRenderer, "unsupported PM mode, renders nothing");
	}
}

void PMRenderer::renderWithProgressivePM()
{
	PH_PROFILE_SCOPE();

	using Photon    = FullPhoton;
	using Viewpoint = FullViewpoint;

	PH_LOG(PhotonMapRenderer, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PhotonMapRenderer, "viewpoint size: {} bytes", sizeof(Viewpoint));

	PH_LOG(PhotonMapRenderer, "start gathering viewpoints...");

	std::vector<Viewpoint> viewpoints;
	{
		using ViewpointCollector = TPPMViewpointCollector<Viewpoint>;
		ViewpointCollector viewpointCollector(6, m_commonParams.kernelRadius);

		auto viewpointSampleGenerator = m_sg->genCopied(m_commonParams.numSamplesPerPixel);

		TViewPathTracingWork<ViewpointCollector> viewpointWork(
			&viewpointCollector,
			m_scene, 
			m_receiver,
			viewpointSampleGenerator.get(),
			getRenderRegionPx());

		viewpointWork.work();

		viewpoints = viewpointCollector.claimViewpoints();
	}
	
	PH_LOG(PhotonMapRenderer, "size of viewpoint buffer: {} MiB", 
		math::bytes_to_MiB<real>(sizeof(Viewpoint) * viewpoints.size()));

	const std::size_t numPhotonsPerPass = m_commonParams.numPhotons;

	PH_LOG(PhotonMapRenderer, "number of photons per pass: {}", numPhotonsPerPass);
	PH_LOG(PhotonMapRenderer, "size of photon buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Photon) * numPhotonsPerPass));

	PH_LOG(PhotonMapRenderer, "start accumulating passes...");

	std::mutex resultFilmMutex;
	auto resultFilm = std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), m_filter);

	Timer passTimer;
	std::size_t numFinishedPasses = 0;
	std::size_t totalPhotonPaths  = 0;
	while(numFinishedPasses < m_commonParams.numPasses)
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
				auto sampleGenerator = m_sg->genCopied(1);

				TPhotonMappingWork<Photon> photonMappingWork(
					m_scene,
					m_receiver,
					sampleGenerator.get(),
					&(photonBuffer[workStart]),
					workEnd - workStart,
					&(numPhotonPaths[workerIdx]));
				photonMappingWork.setStatistics(&m_statistics);

				photonMappingWork.work();
			});
		totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), totalPhotonPaths);

		TPhotonMap<Photon> photonMap(2, TPhotonCenterCalculator<Photon>());
		photonMap.build(std::move(photonBuffer));

		parallel_work(viewpoints.size(), numWorkers(),
			[this, &photonMap, &viewpoints, &resultFilm, &resultFilmMutex, totalPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				auto film = std::make_unique<HdrRgbFilm>(
					getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), m_filter);

				PPMRadianceEvaluationWork radianceEstimator(
					&photonMap, 
					totalPhotonPaths,
					film.get(),
					&(viewpoints[workStart]),
					workEnd - workStart,
					m_scene);
				radianceEstimator.setStatistics(&m_statistics);

				radianceEstimator.work();

				{
					std::lock_guard<std::mutex> lock(resultFilmMutex);

					resultFilm->mergeWith(*film);
				}
			});

		asyncReplaceFilm(*resultFilm);
		resultFilm->clear();

		passTimer.stop();

		const real passTimeMs   = static_cast<real>(passTimer.getDeltaMs());
		const real photonsPerMs = passTimeMs != 0 ? static_cast<real>(numPhotonsPerPass) / passTimeMs : 0;
		m_photonsPerSecond.store(static_cast<std::uint32_t>(photonsPerMs * 1000 + 0.5_r), std::memory_order_relaxed);

		m_statistics.incrementNumIterations();
		++numFinishedPasses;
	}// end while more pass needed
}

void PMRenderer::renderWithStochasticProgressivePM()
{
	PH_PROFILE_SCOPE();

	using Photon    = FullPhoton;
	using Viewpoint = FullViewpoint;

	PH_LOG(PhotonMapRenderer, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PhotonMapRenderer, "viewpoint size: {} bytes", sizeof(Viewpoint));

	PH_LOG(PhotonMapRenderer, "start generating viewpoints...");

	std::vector<Viewpoint> viewpoints(getRenderRegionPx().getArea());
	for(std::size_t y = 0; y < static_cast<std::size_t>(getRenderRegionPx().getHeight()); ++y)
	{
		for(std::size_t x = 0; x < static_cast<std::size_t>(getRenderRegionPx().getWidth()); ++x)
		{
			auto& viewpoint = viewpoints[y * getRenderRegionPx().getWidth() + x];

			if constexpr(Viewpoint::template has<EViewpointData::RADIUS>()) {
				viewpoint.template set<EViewpointData::RADIUS>(m_commonParams.kernelRadius);
			}
			if constexpr(Viewpoint::template has<EViewpointData::NUM_PHOTONS>()) {
				viewpoint.template set<EViewpointData::NUM_PHOTONS>(0.0_r);
			}
			if constexpr(Viewpoint::template has<EViewpointData::TAU>()) {
				viewpoint.template set<EViewpointData::TAU>(math::Spectrum(0));
			}
			if constexpr(Viewpoint::template has<EViewpointData::VIEW_RADIANCE>()) {
				viewpoint.template set<EViewpointData::VIEW_RADIANCE>(math::Spectrum(0));
			}
		}
	}

	PH_LOG(PhotonMapRenderer, "size of viewpoint buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Viewpoint) * viewpoints.size()));

	const std::size_t numPhotonsPerPass = m_commonParams.numPhotons;

	PH_LOG(PhotonMapRenderer, "number of photons per pass: {}", numPhotonsPerPass);
	PH_LOG(PhotonMapRenderer, "size of photon buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Photon) * numPhotonsPerPass));

	PH_LOG(PhotonMapRenderer, "start accumulating passes...");

	std::mutex resultFilmMutex;
	auto resultFilm = std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), m_filter);

	Timer passTimer;
	std::size_t numFinishedPasses = 0;
	std::size_t totalPhotonPaths  = 0;
	while(numFinishedPasses < m_commonParams.numPasses)
	{
		PH_PROFILE_NAMED_SCOPE("SPPM pass");

		passTimer.start();
		std::vector<Photon> photonBuffer(numPhotonsPerPass);

		std::vector<std::size_t> numPhotonPaths(numWorkers(), 0);
		parallel_work(numPhotonsPerPass, numWorkers(),
			[this, &photonBuffer, &numPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				PH_PROFILE_NAMED_SCOPE("SPPM photon shooting");

				auto sampleGenerator = m_sg->genCopied(1);

				TPhotonMappingWork<Photon> photonMappingWork(
					m_scene,
					m_receiver,
					sampleGenerator.get(),
					&(photonBuffer[workStart]),
					workEnd - workStart,
					&(numPhotonPaths[workerIdx]));
				photonMappingWork.setStatistics(&m_statistics);

				photonMappingWork.work();
			});
		totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), totalPhotonPaths);

		TPhotonMap<Photon> photonMap(2, TPhotonCenterCalculator<Photon>());
		photonMap.build(std::move(photonBuffer));

		parallel_work(getRenderRegionPx().getWidth(), numWorkers(),
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

				auto sampleGenerator = m_sg->genCopied(1);

				using RadianceEvaluator = TSPPMRadianceEvaluator<Viewpoint, Photon>;

				RadianceEvaluator radianceEvaluator(
					viewpoints.data(),
					viewpoints.size(),
					&photonMap,
					totalPhotonPaths,
					m_scene,
					resultFilm.get(),
					region,
					numFinishedPasses + 1,
					16384);

				TViewPathTracingWork<RadianceEvaluator> viewpointWork(
					&radianceEvaluator,
					m_scene,
					m_receiver,
					sampleGenerator.get(),
					region);

				viewpointWork.work();
			});

		asyncReplaceFilm(*resultFilm);
		resultFilm->clear();

		passTimer.stop();

		const real passTimeMs   = static_cast<real>(passTimer.getDeltaMs());
		const real photonsPerMs = passTimeMs != 0 ? static_cast<real>(numPhotonsPerPass) / passTimeMs : 0;
		m_photonsPerSecond.store(static_cast<std::uint32_t>(photonsPerMs * 1000 + 0.5_r), std::memory_order_relaxed);

		m_statistics.incrementNumIterations();
		++numFinishedPasses;
	}// end while more pass needed
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
