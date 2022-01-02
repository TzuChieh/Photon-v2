#include "Core/Renderer/Sampling/EqualSamplingRenderer.h"
#include "Common/primitive_type.h"
#include "World/VisualWorld.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "World/VisualWorld.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Common/assertion.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/Vector3Film.h"
#include "Core/Scheduler/PlateScheduler.h"
#include "Core/Scheduler/StripeScheduler.h"
#include "Core/Scheduler/GridScheduler.h"
#include "Core/Scheduler/SpiralGridScheduler.h"
#include "Core/Scheduler/TileScheduler.h"
#include "Core/Scheduler/WorkUnit.h"
#include "Utility/Concurrent/FixedSizeBlockingThreadPool.h"
#include "Utility/utility.h"
#include "Common/logging.h"

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
	std::unique_ptr<IRayEnergyEstimator> estimator,
	Viewport                             viewport,
	SampleFilter                         filter,
	const uint32                         numWorkers,
	const EScheduler                     scheduler) : 

	SamplingRenderer(
		std::move(estimator),
		std::move(viewport),
		std::move(filter),
		numWorkers),

	m_scene(nullptr),
	m_receiver(nullptr),
	m_sampleGenerator(nullptr),
	m_mainFilm(),

	m_scheduler(nullptr),
	m_schedulerType(scheduler),
	m_blockSize(128, 128),

	m_renderWorks(),
	m_filmEstimators(),

	m_updatedRegions(),
	m_rendererMutex(),
	m_totalPaths(),
	m_suppliedFractionBits(),
	m_submittedFractionBits()
{}

void EqualSamplingRenderer::doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	PH_LOG(EqualSamplingRenderer, "rendering core: {}", m_estimator->toString());

	m_updatedRegions.clear();
	m_totalPaths            = 0;
	m_suppliedFractionBits  = 0;
	m_submittedFractionBits = 0;
	
	m_scene           = world.getScene();
	m_receiver        = cooked.getReceiver();
	m_sampleGenerator = cooked.getSampleGenerator();

	const Integrand integrand(m_scene, m_receiver);
	
	m_estimator->mapAttributeToEstimation(0, 0);
	m_estimator->update(integrand);

	m_mainFilm = HdrRgbFilm(
		getRenderWidthPx(), 
		getRenderHeightPx(), 
		getCropWindowPx(),
		m_filter);

	m_filmEstimators.resize(numWorkers());
	m_renderWorks.resize(numWorkers());
	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
	{
		m_filmEstimators[workerId] = FilmEstimator(1, 1, integrand, m_filter);
		m_filmEstimators[workerId].addEstimator(m_estimator.get());
		m_filmEstimators[workerId].addFilmEstimation(0, 0);

		m_renderWorks[workerId] = ReceiverSamplingWork(
			m_receiver);
		m_renderWorks[workerId].addProcessor(&m_filmEstimators[workerId]);
	}

	initScheduler(m_sampleGenerator->numSampleBatches());
	PH_ASSERT(m_scheduler);
}

void EqualSamplingRenderer::doRender()
{
	FixedSizeBlockingThreadPool workers(numWorkers());

	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
	{
		workers.queueWork([this, workerId]()
		{
			auto& renderWork = m_renderWorks[workerId];
			auto& filmEstimator = m_filmEstimators[workerId];

			float suppliedFraction = 0.0f;
			float submittedFraction = 0.0f;
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
				auto sampleGenerator = m_sampleGenerator->genCopied(spp);

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

				renderWork.onWorkReport([this, workerId]()
				{
					std::lock_guard<std::mutex> lock(m_rendererMutex);

					m_filmEstimators[workerId].mergeFilmTo(0, m_mainFilm);
					m_filmEstimators[workerId].clearFilm(0);

					addUpdatedRegion(m_filmEstimators[workerId].getFilmEffectiveWindowPx(), true);
				});

				renderWork.work();

				{
					std::lock_guard<std::mutex> lock(m_rendererMutex);

					m_scheduler->submit(workUnit);
					submittedFraction = m_scheduler->getSubmittedFraction();

					addUpdatedRegion(filmEstimator.getFilmEffectiveWindowPx(), false);
				}

				m_submittedFractionBits.store(
					bitwise_cast<float, std::uint32_t>(submittedFraction),
					std::memory_order_relaxed);

				m_totalPaths.fetch_add(renderWork.asyncGetStatistics().numSamplesTaken, std::memory_order_relaxed);
			}
		});
	}

	workers.waitAllWorks();
}

ERegionStatus EqualSamplingRenderer::asyncPollUpdatedRegion(Region* const out_region)
{
	PH_ASSERT(out_region);

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
void EqualSamplingRenderer::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(layerIndex == 0)
	{
		m_mainFilm.develop(out_frame, region);
	}
	else
	{
		out_frame.fill(0, math::TAABB2D<uint32>(region));
	}
}

void EqualSamplingRenderer::retrieveFrame(const std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	asyncPeekFrame(layerIndex, getCropWindowPx(), out_frame);
}

void EqualSamplingRenderer::addUpdatedRegion(const Region& region, const bool isUpdating)
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

RenderState EqualSamplingRenderer::asyncQueryRenderState()
{
	uint64 totalElapsedMs  = 0;
	uint64 totalNumSamples = 0;
	for(auto&& work : m_renderWorks)
	{
		const auto statistics = work.asyncGetStatistics();
		totalElapsedMs  += work.asyncGetProgress().getElapsedMs();
		totalNumSamples += statistics.numSamplesTaken;
	}

	const float32 samplesPerMs = totalElapsedMs != 0 ?
		static_cast<float32>(m_renderWorks.size() * totalNumSamples) / static_cast<float32>(totalElapsedMs) : 0.0f;

	RenderState state;
	state.setIntegerState(0, m_totalPaths.load(std::memory_order_relaxed) / static_cast<std::size_t>(getCropWindowPx().getArea()));
	state.setRealState(0, samplesPerMs * 1000);
	return state;
}

RenderProgress EqualSamplingRenderer::asyncQueryRenderProgress()
{
	RenderProgress workerProgress(0, 0, 0);
	{
		for(auto&& work : m_renderWorks)
		{
			workerProgress += work.asyncGetProgress();
		}
	}

	const std::size_t totalWork = 100000000;
	const float suppliedFraction = bitwise_cast<std::uint32_t, float>(m_suppliedFractionBits.load(std::memory_order_relaxed));
	const float submittedFraction = std::max(bitwise_cast<std::uint32_t, float>(m_submittedFractionBits.load(std::memory_order_relaxed)), suppliedFraction);
	const float workingFraction = submittedFraction - suppliedFraction;
	const std::size_t workDone = static_cast<std::size_t>(totalWork * (suppliedFraction + workerProgress.getNormalizedProgress() * workingFraction));
	RenderProgress totalProgress(totalWork, std::min(workDone, totalWork), workerProgress.getElapsedMs());

	return totalProgress;
}

ObservableRenderData EqualSamplingRenderer::getObservableData() const
{
	ObservableRenderData data;

	data.setIntegerState(0, "paths/pixel (avg.)");
	data.setRealState   (0, "paths/second");

	return data;
}

void EqualSamplingRenderer::initScheduler(const std::size_t numSamplesPerPixel)
{
	const WorkUnit totalWorks(Region(getCropWindowPx()), numSamplesPerPixel);

	switch(m_schedulerType)
	{
	case EScheduler::BULK:
		m_scheduler = std::make_unique<PlateScheduler>(
			numWorkers(), totalWorks);
		break;

	case EScheduler::STRIPE:
		m_scheduler = std::make_unique<StripeScheduler>(
			numWorkers(), totalWorks, math::constant::X_AXIS);
		break;

	case EScheduler::GRID:
		m_scheduler = std::make_unique<GridScheduler>(
			numWorkers(), totalWorks);
		break;

	case EScheduler::TILE:
		m_scheduler = std::make_unique<TileScheduler>(
			numWorkers(), totalWorks, m_blockSize);
		break;

	case EScheduler::SPIRAL:
		m_scheduler = std::make_unique<SpiralScheduler>(
			numWorkers(), totalWorks, m_blockSize);
		break;

	case EScheduler::SPIRAL_GRID:
		m_scheduler = std::make_unique<SpiralGridScheduler>(
			numWorkers(), totalWorks, m_blockSize);
		break;

	default:
		PH_LOG_WARNING(EqualSamplingRenderer, "unsupported scheduler ID: {}", 
			static_cast<int>(m_schedulerType));

		// If this happends, we then assume other inputs might also be bad.
		// Uses a good default with munimum dependency on user inputs.
		m_scheduler = std::make_unique<GridScheduler>(
			numWorkers(), totalWorks);
		break;
	}
}

}// end namespace ph
