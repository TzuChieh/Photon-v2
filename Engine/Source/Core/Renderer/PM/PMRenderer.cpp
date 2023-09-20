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
#include "Common/logging.h"
#include "Core/Renderer/PM/PPMRadianceEvaluationWork.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Renderer/PM/FullViewpoint.h"
#include "Utility/Timer.h"
#include "Core/Renderer/PM/TPPMViewpointCollector.h"
#include "Core/Renderer/PM/TSPPMRadianceEvaluator.h"

#include <numeric>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PhotonMapRenderer, Renderer);

void PMRenderer::doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	m_film = std::make_unique<HdrRgbFilm>(getRenderWidthPx(), getRenderHeightPx(), getCropWindowPx(), m_filter);

	m_scene    = world.getScene();
	m_receiver = cooked.getReceiver();
	m_sg       = cooked.getSampleGenerator();

	m_statistics.zero();
	m_photonsPerSecond = 0;
	m_isFilmUpdated.store(false);
}

void PMRenderer::doRender()
{
	if(m_mode == EPMMode::VANILLA)
	{
		PH_LOG(PhotonMapRenderer, "rendering mode: vanilla photon mapping");

		renderWithVanillaPM();
	}
	else if(m_mode == EPMMode::PROGRESSIVE)
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

void PMRenderer::renderWithVanillaPM()
{
	using Photon = FullPhoton;

	PH_LOG(PhotonMapRenderer, "photon size: {} bytes", sizeof(Photon));

	PH_LOG(PhotonMapRenderer, "target number of photons: {}", m_numPhotons);
	PH_LOG(PhotonMapRenderer, "size of photon buffer: {} MB", sizeof(Photon) * m_numPhotons / 1024 / 1024);
	PH_LOG(PhotonMapRenderer, "start shooting photons...");

	std::vector<Photon>  photonBuffer(m_numPhotons);
	std::vector<std::size_t> numPhotonPaths(numWorkers(), 0);
	parallel_work(m_numPhotons, numWorkers(),
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
			photonMappingWork.setPMStatistics(&m_statistics);

			photonMappingWork.work();
		});
	const std::size_t totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), std::size_t(0));

	PH_LOG(PhotonMapRenderer, "building photon map...");

	TPhotonMap<Photon> photonMap(2, TPhotonCenterCalculator<Photon>());
	photonMap.build(std::move(photonBuffer));

	PH_LOG(PhotonMapRenderer, "estimating radiance...");

	parallel_work(m_numSamplesPerPixel, numWorkers(),
		[this, &photonMap, totalPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto sampleGenerator = m_sg->genCopied(workEnd - workStart);
			auto film            = std::make_unique<HdrRgbFilm>(
				getRenderWidthPx(), getRenderHeightPx(), getCropWindowPx(), m_filter);

			VPMRadianceEvaluator evaluator(
				&photonMap, 
				totalPhotonPaths, 
				film.get(), 
				m_scene);
			evaluator.setPMRenderer(this);
			evaluator.setPMStatistics(&m_statistics);
			evaluator.setKernelRadius(m_kernelRadius);

			TViewPathTracingWork<VPMRadianceEvaluator> radianceEvaluator(
				&evaluator,
				m_scene,
				m_receiver,
				sampleGenerator.get(),
				getCropWindowPx());

			radianceEvaluator.work();
		});
}

void PMRenderer::renderWithProgressivePM()
{
	using Photon    = FullPhoton;
	using Viewpoint = FullViewpoint;

	PH_LOG(PhotonMapRenderer, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PhotonMapRenderer, "viewpoint size: {} bytes", sizeof(Viewpoint));

	PH_LOG(PhotonMapRenderer, "start gathering viewpoints...");

	std::vector<Viewpoint> viewpoints;
	{
		using ViewpointCollector = TPPMViewpointCollector<Viewpoint>;
		ViewpointCollector viewpointCollector(6, m_kernelRadius);

		auto viewpointSampleGenerator = m_sg->genCopied(m_numSamplesPerPixel);

		TViewPathTracingWork<ViewpointCollector> viewpointWork(
			&viewpointCollector,
			m_scene, 
			m_receiver,
			viewpointSampleGenerator.get(),
			getCropWindowPx());

		viewpointWork.work();

		viewpoints = viewpointCollector.claimViewpoints();
	}
	
	PH_LOG(PhotonMapRenderer, "size of viewpoint buffer: {} MiB", 
		math::bytes_to_MiB<real>(sizeof(Viewpoint) * viewpoints.size()));

	const std::size_t numPhotonsPerPass = m_numPhotons;

	PH_LOG(PhotonMapRenderer, "number of photons per pass: {}", numPhotonsPerPass);
	PH_LOG(PhotonMapRenderer, "size of photon buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Photon) * numPhotonsPerPass));

	PH_LOG(PhotonMapRenderer, "start accumulating passes...");

	std::mutex resultFilmMutex;
	auto resultFilm = std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getCropWindowPx(), m_filter);

	Timer passTimer;
	std::size_t numFinishedPasses = 0;
	std::size_t totalPhotonPaths  = 0;
	while(numFinishedPasses < m_numPasses)
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
				photonMappingWork.setPMStatistics(&m_statistics);

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
					getRenderWidthPx(), getRenderHeightPx(), getCropWindowPx(), m_filter);

				PPMRadianceEvaluationWork radianceEstimator(
					&photonMap, 
					totalPhotonPaths,
					film.get(),
					&(viewpoints[workStart]),
					workEnd - workStart,
					m_scene);
				radianceEstimator.setPMStatistics(&m_statistics);

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

		m_statistics.asyncIncrementNumIterations();
		++numFinishedPasses;
	}// end while more pass needed
}

void PMRenderer::renderWithStochasticProgressivePM()
{
	using Photon    = FullPhoton;
	using Viewpoint = FullViewpoint;

	PH_LOG(PhotonMapRenderer, "photon size: {} bytes", sizeof(Photon));
	PH_LOG(PhotonMapRenderer, "viewpoint size: {} bytes", sizeof(Viewpoint));

	PH_LOG(PhotonMapRenderer, "start generating viewpoints...");

	std::vector<Viewpoint> viewpoints(getCropWindowPx().getArea());
	for(std::size_t y = 0; y < static_cast<std::size_t>(getCropWindowPx().getHeight()); ++y)
	{
		for(std::size_t x = 0; x < static_cast<std::size_t>(getCropWindowPx().getWidth()); ++x)
		{
			auto& viewpoint = viewpoints[y * getCropWindowPx().getWidth() + x];

			if constexpr(Viewpoint::template has<EViewpointData::RADIUS>()) {
				viewpoint.template set<EViewpointData::RADIUS>(m_kernelRadius);
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

	const std::size_t numPhotonsPerPass = m_numPhotons;

	PH_LOG(PhotonMapRenderer, "number of photons per pass: {}", numPhotonsPerPass);
	PH_LOG(PhotonMapRenderer, "size of photon buffer: {} MiB",
		math::bytes_to_MiB<real>(sizeof(Photon) * numPhotonsPerPass));

	PH_LOG(PhotonMapRenderer, "start accumulating passes...");

	std::mutex resultFilmMutex;
	auto resultFilm = std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getCropWindowPx(), m_filter);

	Timer passTimer;
	std::size_t numFinishedPasses = 0;
	std::size_t totalPhotonPaths  = 0;
	while(numFinishedPasses < m_numPasses)
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
				photonMappingWork.setPMStatistics(&m_statistics);

				photonMappingWork.work();
			});
		totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), totalPhotonPaths);

		TPhotonMap<Photon> photonMap(2, TPhotonCenterCalculator<Photon>());
		photonMap.build(std::move(photonBuffer));

		parallel_work(getCropWindowPx().getWidth(), numWorkers(),
			[this, &photonMap, &viewpoints, &resultFilm, &resultFilmMutex, totalPhotonPaths, numFinishedPasses](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				Region region = getCropWindowPx();
				auto [minVertex, maxVertex] = region.getVertices();
				minVertex.x() = getCropWindowPx().getMinVertex().x() + workStart;
				maxVertex.x() = getCropWindowPx().getMinVertex().x() + workEnd;
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

		m_statistics.asyncIncrementNumIterations();
		++numFinishedPasses;
	}// end while more pass needed
}

ERegionStatus PMRenderer::asyncPollUpdatedRegion(Region* const out_region)
{
	PH_ASSERT(out_region);

	if(m_isFilmUpdated.load())
	{
		*out_region = getCropWindowPx();
		m_isFilmUpdated.store(false);

		return ERegionStatus::UPDATING;
	}
	else
	{
		return ERegionStatus::INVALID;
	}
}

RenderProgress PMRenderer::asyncQueryRenderProgress()
{
	return RenderProgress(
		m_mode != EPMMode::VANILLA ? m_numPasses : m_numSamplesPerPixel, 
		m_statistics.asyncGetNumIterations(), 
		0);
}

void PMRenderer::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame)
{
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
	m_film->develop(out_frame);
}

ObservableRenderData PMRenderer::getObservableData() const
{
	ObservableRenderData data;

	data.setIntegerState(0, m_mode != EPMMode::VANILLA ? "finished passes" : "finished samples");
	data.setIntegerState(1, "traced photons");
	data.setIntegerState(2, "photons/second");

	return data;
}

RenderStats PMRenderer::asyncQueryRenderStats()
{
	RenderStats stats;
	stats.setInteger(0, m_statistics.asyncGetNumIterations());
	stats.setInteger(1, m_statistics.asyncGetNumTracedPhotons());
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
