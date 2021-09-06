#include "Core/Renderer/Sampling/AdaptiveSamplingRenderer.h"
#include "Common/assertion.h"
#include "Core/Ray.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "World/VisualWorld.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Common/assertion.h"
#include "Core/Estimator/Integrand.h"
#include "Utility/FixedSizeThreadPool.h"
#include "Utility/utility.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <utility>

namespace ph
{

void AdaptiveSamplingRenderer::doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	m_updatedRegions.clear();
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
		getCropWindowPx(),
		m_filter);
	m_halfEffortFilm = HdrRgbFilm(
		getRenderWidthPx(),
		getRenderHeightPx(),
		getCropWindowPx(),
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
		getCropWindowPx(),
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
		auto& renderWork    = m_renderWorks[workerId];
		auto& filmEstimator = m_filmEstimators[workerId];
		auto  analyzer      = m_dispatcher.createAnalyzer<REFINE_MODE>();

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
				bitwise_cast<float, std::uint32_t>(suppliedFraction),
				std::memory_order_relaxed);

			filmEstimator.setFilmDimensions(
				math::TVector2<int64>(getRenderWidthPx(), getRenderHeightPx()),
				workUnit.getRegion());

			const auto filmDimensions = filmEstimator.getFilmDimensions();
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

			renderWork.onWorkReport([this, workerId]()
			{
				// No synchronization needed, since no other worker can have an 
				// overlapping region with the current one.
				m_filmEstimators[workerId].mergeFilmTo(0, m_allEffortFilm);
				m_filmEstimators[workerId].clearFilm(0);

				std::lock_guard<std::mutex> lock(m_rendererMutex);

				addUpdatedRegion(m_filmEstimators[workerId].getFilmEffectiveWindowPx(), true);
			});

			renderWork.work();

			// No synchronization needed, since no other worker can have an 
			// overlapping region with the current one.
			m_filmEstimators[workerId].mergeFilmTo(1, m_halfEffortFilm);
			m_filmEstimators[workerId].clearFilm(1);
			m_allEffortFilm.develop(m_allEffortFrame, workUnit.getRegion());
			m_halfEffortFilm.develop(m_halfEffortFrame, workUnit.getRegion());
			analyzer.analyzeFinishedRegion(workUnit.getRegion(), m_allEffortFrame, m_halfEffortFrame);

			m_metaRecorders[workerId].getRecord(&m_metaFrame, {0, 0});

			{
				std::lock_guard<std::mutex> lock(m_rendererMutex);

				m_dispatcher.addAnalyzedData(analyzer);
				m_numNoisyRegions.store(static_cast<uint32>(m_dispatcher.numPendingRegions()), std::memory_order_relaxed);

				addUpdatedRegion(workUnit.getRegion(), false);
			}

			m_submittedFractionBits.store(
				bitwise_cast<float, std::uint32_t>(submittedFraction),
				std::memory_order_relaxed);

			m_totalPaths.fetch_add(renderWork.asyncGetStatistics().numSamplesTaken, std::memory_order_relaxed);
		}
	};
}

ERegionStatus AdaptiveSamplingRenderer::asyncPollUpdatedRegion(Region* const out_region)
{
	PH_ASSERT(out_region != nullptr);

	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_updatedRegions.empty())
	{
		return ERegionStatus::INVALID;
	}

	const UpdatedRegion updatedRegion = m_updatedRegions.front();
	m_updatedRegions.pop_front();

	*out_region = updatedRegion.region;
	if(updatedRegion.isFinished)
	{
		return ERegionStatus::FINISHED;
	}
	else
	{
		return ERegionStatus::UPDATING;
	}
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
			[&out_frame](const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& pixel)
			{
				HdrRgbFrame::Pixel mappedPixel;
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
	asyncPeekFrame(layerIndex, getCropWindowPx(), out_frame);
}

void AdaptiveSamplingRenderer::addUpdatedRegion(const Region& region, const bool isUpdating)
{
	for(UpdatedRegion& pendingRegion : m_updatedRegions)
	{
		// later added region takes the precedence
		if(pendingRegion.region.isEqual(region))
		{
			pendingRegion.isFinished = !isUpdating;
			return;
		}
	}

	m_updatedRegions.push_back(UpdatedRegion{region, !isUpdating});
}

RenderState AdaptiveSamplingRenderer::asyncQueryRenderState()
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

	RenderState state;
	state.setIntegerState(0, m_totalPaths.load(std::memory_order_relaxed) / static_cast<std::size_t>(getCropWindowPx().getArea()));
	state.setIntegerState(1, m_numNoisyRegions.load(std::memory_order_relaxed));
	state.setRealState(0, samplesPerMs * 1000);
	return state;
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
	const float suppliedFraction = bitwise_cast<std::uint32_t, float>(m_suppliedFractionBits.load(std::memory_order_relaxed));
	const float submittedFraction = std::max(bitwise_cast<std::uint32_t, float>(m_submittedFractionBits.load(std::memory_order_relaxed)), suppliedFraction);
	const float workingFraction = submittedFraction - suppliedFraction;
	const std::size_t workDone = static_cast<std::size_t>(totalWork * (suppliedFraction + workerProgress.getNormalizedProgress() * workingFraction));
	RenderProgress totalProgress(totalWork, std::min(workDone, totalWork), workerProgress.getElapsedMs());

	return totalProgress;
}

ObservableRenderData AdaptiveSamplingRenderer::getObservableData() const
{
	ObservableRenderData data;

	data.setIntegerState(0, "paths/pixel (avg.)");
	data.setIntegerState(1, "noisy regions");
	data.setRealState   (0, "paths/second");

	return data;
}

}// end namespace ph
