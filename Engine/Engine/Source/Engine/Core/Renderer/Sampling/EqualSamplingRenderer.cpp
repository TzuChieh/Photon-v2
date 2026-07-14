#include "Engine/Core/Renderer/Sampling/EqualSamplingRenderer.h"
#include "Engine/World/VisualWorld.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/constant.h"
#include "Engine/Core/SampleGenerator/SampleGenerator.h"
#include "Engine/EngineEnv/CoreCookedUnit.h"
#include "Engine/World/VisualWorld.h"
#include "Engine/Core/Renderer/RenderWork.h"
#include "Engine/Core/Renderer/RenderWorker.h"
#include "Engine/Core/Renderer/RendererProxy.h"
#include "Engine/Core/Estimator/Integrand.h"
#include "Engine/Core/Scheduler/PlateScheduler.h"
#include "Engine/Core/Scheduler/StripeScheduler.h"
#include "Engine/Core/Scheduler/GridScheduler.h"
#include "Engine/Core/Scheduler/SpiralGridScheduler.h"
#include "Engine/Core/Scheduler/TileScheduler.h"
#include "Engine/Core/Scheduler/WorkUnit.h"
#include "Engine/Utility/Concurrent/FixedSizeThreadPool.h"
#include "Engine/Utility/utility.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>
#include <Common/profiling.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(EqualSamplingRenderer, Renderer);

EqualSamplingRenderer::EqualSamplingRenderer(
	std::unique_ptr<IRayEnergyEstimator>           estimator,
	Viewport                                       viewport,
	SampleFilter                                   filter,
	const uint32                                   numWorkers,
	const EScheduler                               scheduler,
	std::vector<SamplingFilmLayer<math::Spectrum>> filmLayers)

	: SamplingRenderer(
		std::move(estimator),
		std::move(viewport),
		std::move(filter),
		numWorkers)

	, m_scene(nullptr)
	, m_receiver(nullptr)
	, m_sampleGenerator(nullptr)
	
	, m_scheduler(nullptr)
	, m_schedulerType(scheduler)
	, m_mainFilmLayers(std::move(filmLayers))
	, m_blockSize(128, 128)
	, m_updatedRegionQueue()

	, m_renderWorks()
	, m_rayProcessors()
	, m_metaRecorders()

	, m_rendererMutex()
	, m_totalPaths()
	, m_totalElapsedMs()
	, m_suppliedFractionBits()
	, m_submittedFractionBits()
{
	PH_ASSERT(!m_mainFilmLayers.empty());
	for(const auto& filmLayer : m_mainFilmLayers)
	{
		PH_ASSERT(filmLayer.film);
	}
}

void EqualSamplingRenderer::doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	PH_LOG(EqualSamplingRenderer, Note, "rendering core: {}", m_estimator->toString());

	m_scene           = world.getScene();
	m_receiver        = cooked.getReceiver();
	m_sampleGenerator = cooked.getSampleGenerator();

	const Integrand integrand(m_scene, m_receiver);
	
	m_estimator->setEstimationIndex(EEstimatorAttribute::Energy, 0);
	m_estimator->update(integrand);

	for(auto& filmLayer : m_mainFilmLayers)
	{
		filmLayer.film->setActualResPx({getRenderWidthPx(), getRenderHeightPx()});
		filmLayer.film->setEffectiveWindowPx(getRenderRegionPx());
		filmLayer.film->clear();
	}

	m_rayProcessors.resize(numWorkers());
	m_renderWorks.resize(numWorkers());
	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
	{
		// One ray processor for each worker
		std::vector<std::shared_ptr<RayProcessor::FilmType>> workerFilms;
		workerFilms.reserve(m_mainFilmLayers.size());
		for(const auto& mainFilmLayer : m_mainFilmLayers)
		{
			auto copiedFilm = mainFilmLayer.film->makeCopy(false);
			workerFilms.push_back(std::shared_ptr<RayProcessor::FilmType>(std::move(copiedFilm)));
		}

		m_rayProcessors[workerId] = RayProcessor(
			1,
			integrand,
			std::move(workerFilms));
		m_rayProcessors[workerId].addEstimator(m_estimator->makeCopy());
		for(std::size_t filmIdx = 0; filmIdx < m_mainFilmLayers.size(); ++filmIdx)
		{
			m_rayProcessors[workerId].addFilmEstimation(filmIdx, 0);
		}

		m_renderWorks[workerId] = ReceiverSamplingWork(m_receiver);
		m_renderWorks[workerId].addProcessor(&m_rayProcessors[workerId]);
	}

	initScheduler(m_sampleGenerator->numSampleBatches());
	PH_ASSERT(m_scheduler);

	// Enough for 1024 workers, each with 16 updates queued
	m_updatedRegionQueue = TAtomicQuasiQueue<RenderRegionStatus>(1024 * 16);

	m_totalPaths            = 0;
	m_totalElapsedMs        = 0;
	m_suppliedFractionBits  = 0;
	m_submittedFractionBits = 0;
}

void EqualSamplingRenderer::doRender()
{
	PH_PROFILE_SCOPE();

	FixedSizeThreadPool workers(numWorkers());

	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
	{
		workers.queueWork([this, workerId]()
		{
			auto& renderWork         = m_renderWorks[workerId];
			auto& workerRayProcessor = m_rayProcessors[workerId];

			renderWork.onWorkReport([this, &workerRayProcessor]()
			{
				{
					std::lock_guard<std::mutex> lock(m_rendererMutex);

					for(std::size_t filmIdx = 0; filmIdx < m_mainFilmLayers.size(); ++filmIdx)
					{
						workerRayProcessor.mergeFilmTo(filmIdx, *m_mainFilmLayers[filmIdx].film);
					}
				}

				workerRayProcessor.clearFilms();
				asyncAddUpdatedRegion(workerRayProcessor.getFilmEffectiveWindowPx(), true);
			});

			float32 suppliedFraction = 0.0f;
			float32 submittedFraction = 0.0f;
			WorkUnit workUnit;
			while(true)
			{
				{
					std::lock_guard<std::mutex> lock(m_rendererMutex);
					
					if(m_scheduler->schedule(&workUnit))
					{
						suppliedFraction = m_scheduler->getScheduledFraction();
					}
					else
					{
						suppliedFraction = 1.0f;
						break;
					}
				}

				const std::size_t spp = workUnit.getDepth();
				auto sampleGenerator = m_sampleGenerator->makeNewborn(spp);

				m_suppliedFractionBits.store(
					bitwise_cast<std::uint32_t>(suppliedFraction),
					std::memory_order_relaxed);

				workerRayProcessor.setFilmDimensions(
					math::TVector2<int64>(getRenderWidthPx(), getRenderHeightPx()),
					workUnit.getRegion());

				const auto filmDimensions = workerRayProcessor.getFilmDimensions();
				renderWork.setSampleDimensions(
					filmDimensions.actualResPx, 
					filmDimensions.sampleWindowPx, 
					filmDimensions.effectiveWindowPx.getExtents());
				renderWork.setSampleGenerator(std::move(sampleGenerator));

				renderWork.work();

				{
					std::lock_guard<std::mutex> lock(m_rendererMutex);

					m_scheduler->submit(workUnit);
					submittedFraction = m_scheduler->getSubmittedFraction();
				}

				asyncAddUpdatedRegion(workerRayProcessor.getFilmEffectiveWindowPx(), false);

				m_submittedFractionBits.store(
					bitwise_cast<std::uint32_t>(submittedFraction),
					std::memory_order_relaxed);

				m_totalPaths.fetch_add(renderWork.asyncGetStatistics().numSamplesTaken, std::memory_order_relaxed);
				m_totalElapsedMs.fetch_add(renderWork.asyncGetProgress().getElapsedMs(), std::memory_order_relaxed);
			}
		});
	}

	workers.waitAllWorks();
}

std::size_t EqualSamplingRenderer::asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions)
{
	PH_PROFILE_SCOPE();

	return m_updatedRegionQueue.tryDequeueBulk(out_regions.begin(), out_regions.size());
}

// FIXME: Peeking does not need to ensure correctness of the frame.
// If correctness is not guaranteed, develop methods should be reimplemented. 
// (correctness is guaranteed currently)
void EqualSamplingRenderer::asyncPeekFrame(
	const uint32      layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame)
{
	PH_PROFILE_SCOPE();

	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(layerIndex < m_mainFilmLayers.size())
	{
		m_mainFilmLayers[layerIndex].film->develop(out_frame, region);
	}
	else
	{
		out_frame.fill(0, math::TAABB2D<uint32>(region));
	}
}

void EqualSamplingRenderer::retrieveFrame(const uint32 layerIndex, HdrRgbFrame& out_frame)
{
	PH_PROFILE_SCOPE();

	asyncPeekFrame(layerIndex, getRenderRegionPx(), out_frame);
}

void EqualSamplingRenderer::asyncAddUpdatedRegion(const Region& region, const bool isUpdating)
{
	if(isUpdating)
	{
		// Updating regions may keep accumulating, even with identical regions added. To not
		// exhaust all available memory, simply drop the region if no space is available.
		m_updatedRegionQueue.tryEnqueue(RenderRegionStatus(region, ERegionStatus::Updating));
	}
	else
	{
		// Always enqueue non-updating regions to meet the required ordering. A sane implementation
		// will not resubmitting non-updating regions for too many times, so no risk of using too
		// much memory.
		m_updatedRegionQueue.enqueue(RenderRegionStatus(region, ERegionStatus::Finished));
	}
}

RenderStats EqualSamplingRenderer::asyncQueryRenderStats()
{
	PH_PROFILE_SCOPE();

	// We want to calculate transient stats so averaging all current work stats is fine
	// (when a render work object is restarted its stats will be refreshed)
	uint64 summedElapsedMs = 0;
	uint64 summedNumSamples = 0;
	for(const auto& work : m_renderWorks)
	{
		const auto statistics = work.asyncGetStatistics();
		summedElapsedMs += work.asyncGetProgress().getElapsedMs();
		summedNumSamples += statistics.numSamplesTaken;
	}

	const float32 samplesPerMs = summedElapsedMs != 0 ?
		m_renderWorks.size() * (static_cast<float32>(summedNumSamples) / summedElapsedMs) : 0.0f;

	RenderStats stats;
	stats.setInteger(0, m_totalPaths.load(std::memory_order_relaxed) / static_cast<std::size_t>(getRenderRegionPx().getArea()));
	stats.setReal(0, samplesPerMs * 1000);
	return stats;
}

RenderProgress EqualSamplingRenderer::asyncQueryRenderProgress()
{
	PH_PROFILE_SCOPE();

	RenderProgress workerProgress;
	for(const auto& work : m_renderWorks)
	{
		workerProgress += work.asyncGetProgress();
	}

	// Should be plenty (equivalent to a 8k 60FPS 4-hour video with 8192spp)
	constexpr uint64 totalWork = 7680ull * 4320 * 8192 * 60 * 3600 * 4;

	const auto suppliedFraction = bitwise_cast<float32>(m_suppliedFractionBits.load(std::memory_order_relaxed));
	const auto submittedFraction = bitwise_cast<float32>(m_submittedFractionBits.load(std::memory_order_relaxed));
	const auto workingFraction = std::max(suppliedFraction - submittedFraction, 0.0f);
	const auto totalFraction = std::min(submittedFraction + workerProgress.getNormalizedProgress() * workingFraction, 1.0f);
	const auto workDone = static_cast<uint64>(totalWork * static_cast<float64>(totalFraction));
	
	// We want to obtain progress of the entire rendering process. Elapsed time from workers
	// should not be used directly as they are refreshed everytime they restart.
	const auto sumbittedTotalMs = m_totalElapsedMs.load(std::memory_order_relaxed);
	const auto totalMs = sumbittedTotalMs + workerProgress.getElapsedMs();
	const uint64 elapsedMsPerWorker = !m_renderWorks.empty() ? totalMs / m_renderWorks.size() : 0;

	return RenderProgress(
		totalWork,
		std::min(workDone, totalWork),
		elapsedMsPerWorker);
}

RenderObservableInfo EqualSamplingRenderer::getObservableInfo() const
{
	RenderObservableInfo info;
	info.setIntegerStat(0, "paths/pixel (avg.)");
	info.setRealStat(0, "paths/second");
	info.setProgressTimeMeasurement("wall clock time");
	return info;
}

void EqualSamplingRenderer::initScheduler(const std::size_t numSamplesPerPixel)
{
	const WorkUnit totalWorks(Region(getRenderRegionPx()), numSamplesPerPixel);

	switch(m_schedulerType)
	{
	case EScheduler::Bulk:
		m_scheduler = std::make_unique<PlateScheduler>(
			numWorkers(), totalWorks);
		break;

	case EScheduler::Stripe:
		m_scheduler = std::make_unique<StripeScheduler>(
			numWorkers(), totalWorks);
		break;

	case EScheduler::Grid:
		m_scheduler = std::make_unique<GridScheduler>(
			numWorkers(), totalWorks);
		break;

	case EScheduler::Tile:
		m_scheduler = std::make_unique<TileScheduler>(
			numWorkers(), totalWorks, m_blockSize);
		break;

	case EScheduler::Spiral:
		m_scheduler = std::make_unique<SpiralScheduler>(
			numWorkers(), totalWorks, m_blockSize);
		break;

	case EScheduler::SpiralGrid:
		m_scheduler = std::make_unique<SpiralGridScheduler>(
			numWorkers(), totalWorks, m_blockSize);
		break;

	default:
		PH_LOG(EqualSamplingRenderer, Warning, "unsupported scheduler ID: {}",
			static_cast<int>(m_schedulerType));

		// If this happends, we then assume other inputs might also be bad.
		// Uses a good default with munimum dependency on user inputs.
		m_scheduler = std::make_unique<GridScheduler>(
			numWorkers(), totalWorks);
		break;
	}
}

}// end namespace ph
