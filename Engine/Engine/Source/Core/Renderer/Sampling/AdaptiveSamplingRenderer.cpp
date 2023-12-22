#include "Core/Renderer/Sampling/AdaptiveSamplingRenderer.h"
#include "Core/Ray.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "World/VisualWorld.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Core/Estimator/Integrand.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"
#include "Utility/utility.h"

#include <Common/assertion.h>

#include <cmath>
#include <thread>
#include <chrono>
#include <utility>

namespace ph
{

void AdaptiveSamplingRenderer::doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	m_updatedRegionQueue    = TAtomicQuasiQueue<RenderRegionStatus>{};
	m_totalPaths            = 0;
	m_suppliedFractionBits  = 0;
	m_submittedFractionBits = 0;
	m_numNoisyRegions       = 0;

	m_scene           = world.getScene();
	m_receiver        = cooked.getReceiver();
	m_sampleGenerator = cooked.getSampleGenerator();

	const Integrand integrand(m_scene, m_receiver);

	m_estimator->mapAttributeToEstimation(0, 0);
	m_estimator->update(integrand);

	m_allEffortFilm = HdrRgbFilm(
		getRenderWidthPx(),
		getRenderHeightPx(),
		getRenderRegionPx(),
		m_filter);
	m_halfEffortFilm = HdrRgbFilm(
		getRenderWidthPx(),
		getRenderHeightPx(),
		getRenderRegionPx(),
		m_filter);

	m_metaRecorders.resize(numWorkers());
	m_filmEstimators.resize(numWorkers());
	m_renderWorks.resize(numWorkers());
	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
	{
		m_filmEstimators[workerId] = FilmEstimator(2, 1, integrand, m_filter);
		m_filmEstimators[workerId].addEstimator(m_estimator.get());
		m_filmEstimators[workerId].addFilmEstimation(0, 0);
		m_filmEstimators[workerId].addFilmEstimation(1, 0);
		m_filmEstimators[workerId].setFilmStepSize(1, 2);

		m_metaRecorders[workerId] = MetaRecordingProcessor(&m_filmEstimators[workerId]);

		m_renderWorks[workerId] = ReceiverSamplingWork(
			m_receiver);
		//m_renderWorks[workerId].addProcessor(&m_filmEstimators[workerId]);
		// DEBUG
		m_renderWorks[workerId].addProcessor(&m_metaRecorders[workerId]);
	}

	m_dispatcher = DammertzDispatcher(
		numWorkers(),
		getRenderRegionPx(),
		m_precisionStandard,
		m_numInitialSamples);

	m_freeWorkerIds.clear();
	m_freeWorkerIds.reserve(numWorkers());

	m_allEffortFrame = HdrRgbFrame(getRenderWidthPx(), getRenderHeightPx());
	m_halfEffortFrame = HdrRgbFrame(getRenderWidthPx(), getRenderHeightPx());

	m_metaFrame = HdrRgbFrame(getRenderWidthPx(), getRenderHeightPx());
}

void AdaptiveSamplingRenderer::doRender()
{
	FixedSizeThreadPool workers(numWorkers());

	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
	{
		workers.queueWork(createWork(workers, workerId));
	}

	workers.waitAllWorks();
}

std::function<void()> AdaptiveSamplingRenderer::createWork(FixedSizeThreadPool& workers, uint32 workerId)
{
	return [this, workerId, &workers]()
	{
		auto& renderWork          = m_renderWorks[workerId];
		auto& workerFilmEstimator = m_filmEstimators[workerId];
		auto  workerAnalyzer      = m_dispatcher.createAnalyzer<REFINE_MODE>();

		// DEBUG
		auto& metaRecorder = m_metaRecorders[workerId];

		float suppliedFraction = 0.0f;
		float submittedFraction = 0.0f;
		while(true)
		{
			WorkUnit workUnit;
			std::unique_ptr<SampleGenerator> sampleGenerator;
			{
				std::lock_guard<std::mutex> lock(m_rendererMutex);

				if(!m_dispatcher.dispatch(&workUnit))
				{
					m_freeWorkerIds.push_back(workerId);
					break;
				}

				const std::size_t spp = workUnit.getDepth();
				sampleGenerator = m_sampleGenerator->genCopied(spp);

				std::size_t numPendingRegions = m_dispatcher.numPendingRegions();
				while(!m_freeWorkerIds.empty() && numPendingRegions > 0)
				{
					const uint32 freeWorkerId = m_freeWorkerIds.back();
					m_freeWorkerIds.pop_back();
					--numPendingRegions;

					workers.queueWork(createWork(workers, freeWorkerId));
				}
			}

			m_suppliedFractionBits.store(
				bitwise_cast<std::uint32_t>(suppliedFraction),
				std::memory_order_relaxed);

			workerFilmEstimator.setFilmDimensions(
				math::TVector2<int64>(getRenderWidthPx(), getRenderHeightPx()),
				workUnit.getRegion());

			const auto filmDimensions = workerFilmEstimator.getFilmDimensions();
			renderWork.setSampleDimensions(
				filmDimensions.actualResPx,
				filmDimensions.sampleWindowPx,
				filmDimensions.effectiveWindowPx.getExtents());
			renderWork.setSampleGenerator(std::move(sampleGenerator));

			// DEBUG
			metaRecorder.setDimensions(
				math::TVector2<int64>(getRenderWidthPx(), getRenderHeightPx()),
				workUnit.getRegion());
			metaRecorder.clearRecords();

			renderWork.onWorkReport([this, &workerFilmEstimator]()
			{
				// No synchronization needed, since no other worker can have an 
				// overlapping region with the current one.
				workerFilmEstimator.mergeFilmTo(0, m_allEffortFilm);
				workerFilmEstimator.clearFilm(0);

				asyncAddUpdatedRegion(workerFilmEstimator.getFilmEffectiveWindowPx(), true);
			});

			renderWork.work();

			// No synchronization needed, since no other worker can have an 
			// overlapping region with the current one.
			workerFilmEstimator.mergeFilmTo(1, m_halfEffortFilm);
			workerFilmEstimator.clearFilm(1);
			m_allEffortFilm.develop(m_allEffortFrame, workUnit.getRegion());
			m_halfEffortFilm.develop(m_halfEffortFrame, workUnit.getRegion());
			workerAnalyzer.analyzeFinishedRegion(workUnit.getRegion(), m_allEffortFrame, m_halfEffortFrame);

			m_metaRecorders[workerId].getRecord(&m_metaFrame, {0, 0});

			{
				std::lock_guard<std::mutex> lock(m_rendererMutex);

				m_dispatcher.addAnalyzedData(workerAnalyzer);
				m_numNoisyRegions.store(static_cast<uint32>(m_dispatcher.numPendingRegions()), std::memory_order_relaxed);
			}

			asyncAddUpdatedRegion(workUnit.getRegion(), false);

			m_submittedFractionBits.store(
				bitwise_cast<std::uint32_t>(submittedFraction),
				std::memory_order_relaxed);

			m_totalPaths.fetch_add(renderWork.asyncGetStatistics().numSamplesTaken, std::memory_order_relaxed);
		}
	};
}

std::size_t AdaptiveSamplingRenderer::asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions)
{
	return m_updatedRegionQueue.tryDequeueBulk(out_regions.begin(), out_regions.size());
}

// FIXME: Peeking does not need to ensure correctness of the frame.
// If correctness is not guaranteed, develop methods should be reimplemented. 
// (correctness is guaranteed currently)
void AdaptiveSamplingRenderer::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(layerIndex == 0)
	{
		m_allEffortFilm.develop(out_frame, region);
	}
	else if(layerIndex == 1)
	{
		m_metaFrame.forEachPixel(
			math::TAABB2D<uint32>(region),
			[&out_frame](const uint32 x, const uint32 y, const HdrRgbFrame::PixelType& pixel)
			{
				HdrRgbFrame::PixelType mappedPixel;
				mappedPixel[0] = std::pow(std::log2(pixel[0] + 1) / 14.0_r, 16.0_r);
				mappedPixel[1] = std::pow(std::log2(pixel[1] + 1) / 14.0_r, 16.0_r);
				mappedPixel[2] = std::pow(std::log2(pixel[2] + 1) / 14.0_r, 16.0_r);
				out_frame.setPixel({x, y}, mappedPixel);
			});
	}
	else
	{
		out_frame.fill(0, math::TAABB2D<uint32>(region));
	}
}

void AdaptiveSamplingRenderer::retrieveFrame(const std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	asyncPeekFrame(layerIndex, getRenderRegionPx(), out_frame);
}

void AdaptiveSamplingRenderer::asyncAddUpdatedRegion(const Region& region, const bool isUpdating)
{
	m_updatedRegionQueue.enqueue(
		RenderRegionStatus(region, isUpdating ? ERegionStatus::Updating : ERegionStatus::Finished));
}

RenderStats AdaptiveSamplingRenderer::asyncQueryRenderStats()
{
	uint64 totalElapsedMs = 0;
	uint64 totalNumSamples = 0;
	for(auto&& work : m_renderWorks)
	{
		const auto statistics = work.asyncGetStatistics();
		totalElapsedMs += work.asyncGetProgress().getElapsedMs();
		totalNumSamples += statistics.numSamplesTaken;
	}

	const float32 samplesPerMs = totalElapsedMs != 0 ?
		static_cast<float32>(m_renderWorks.size() * totalNumSamples) / static_cast<float32>(totalElapsedMs) : 0.0f;

	RenderStats stats;
	stats.setInteger(0, m_totalPaths.load(std::memory_order_relaxed) / static_cast<std::size_t>(getRenderRegionPx().getArea()));
	stats.setInteger(1, m_numNoisyRegions.load(std::memory_order_relaxed));
	stats.setReal(0, samplesPerMs * 1000);
	return stats;
}

RenderProgress AdaptiveSamplingRenderer::asyncQueryRenderProgress()
{
	RenderProgress workerProgress(0, 0, 0);
	{
		for(auto&& work : m_renderWorks)
		{
			workerProgress += work.asyncGetProgress();
		}
	}

	// HACK
	const std::size_t totalWork = 100000000;
	const float suppliedFraction = bitwise_cast<float>(m_suppliedFractionBits.load(std::memory_order_relaxed));
	const float submittedFraction = std::max(bitwise_cast<float>(m_submittedFractionBits.load(std::memory_order_relaxed)), suppliedFraction);
	const float workingFraction = submittedFraction - suppliedFraction;
	const std::size_t workDone = static_cast<std::size_t>(totalWork * (suppliedFraction + workerProgress.getNormalizedProgress() * workingFraction));
	RenderProgress totalProgress(totalWork, std::min(workDone, totalWork), workerProgress.getElapsedMs());

	return totalProgress;
}

RenderObservationInfo AdaptiveSamplingRenderer::getObservationInfo() const
{
	RenderObservationInfo info;
	info.setIntegerStat(0, "paths/pixel (avg.)");
	info.setIntegerStat(1, "noisy regions");
	info.setRealStat   (0, "paths/second");

	info.setProgressTimeMeasurement("CPU time");

	return info;
}

}// end namespace ph
