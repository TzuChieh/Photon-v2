#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Common/primitive_type.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Core/Camera/Camera.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/SDL/SdlResourcePack.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Common/assertion.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "Core/Estimator/BVPTEstimator.h"
#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/Vector3Film.h"
#include "Utility/FixedSizeThreadPool.h"
#include "Core/Renderer/Region/PlateScheduler.h"
#include "Core/Renderer/Region/StripeScheduler.h"
#include "Core/Renderer/Region/GridScheduler.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <utility>

namespace ph
{

void SamplingRenderer::doUpdate(const SdlResourcePack& data)
{
	clearWorkData();

	m_scene  = &data.visualWorld.getScene();
	m_camera = data.getCamera().get();
	m_sampleGenerator = data.getSampleGenerator().get();

	m_estimator->update(*m_scene);

	m_films.set<EAttribute::LIGHT_ENERGY>(std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter));

	// HACK
	m_films.set<EAttribute::NORMAL>(std::make_unique<Vector3Film>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter));

	m_renderWorks.resize(numWorkers());
	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
	{
		m_renderWorks[workerId] = SamplingRenderWork(
			m_estimator.get(),
			Integrand(m_scene, m_camera),
			this);
	}
}

void SamplingRenderer::doRender()
{
	FixedSizeThreadPool workers(numWorkers());

	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
	{
		workers.queueWork([this, workerId]()
		{
			SamplingRenderWork& renderWork = m_renderWorks[workerId];

			while(true)
			{
				{
					std::lock_guard<std::mutex> lock(m_rendererMutex);

					if(!supplyWork(workerId, renderWork))
					{
						break;
					}
				}

				renderWork.work();

				{
					std::lock_guard<std::mutex> lock(m_rendererMutex);

					submitWork(workerId, renderWork);
				}
			}
		});
	}

	workers.waitAllWorks();
}

void SamplingRenderer::asyncUpdateFilm(SamplingFilmSet& workerFilms, bool isUpdating)
{
	{
		std::lock_guard<std::mutex> lock(m_rendererMutex);

		mergeWorkFilms(workerFilms);

		// HACK
		addUpdatedRegion(workerFilms.get<EAttribute::LIGHT_ENERGY>()->getEffectiveWindowPx(), isUpdating);
	}

	// FIXME: this is broken under non-bulk rendering
	m_averageSpp.fetch_add(1, std::memory_order_relaxed);
}

void SamplingRenderer::clearWorkData()
{
	m_updatedRegions.clear();

	// TODO: other data
}

ERegionStatus SamplingRenderer::asyncPollUpdatedRegion(Region* const out_region)
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

void SamplingRenderer::asyncPeekRegion(
	HdrRgbFrame&     out_frame, 
	const Region&    region, 
	const EAttribute attribute)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	const SamplingFilmBase* film = m_films.get(attribute);
	if(film)
	{
		film->develop(out_frame, region);
	}
	else
	{
		out_frame.fill(0);
	}
}

void SamplingRenderer::develop(HdrRgbFrame& out_frame, const EAttribute attribute)
{
	asyncPeekRegion(out_frame, getRenderWindowPx(), attribute);
}

void SamplingRenderer::mergeWorkFilms(SamplingFilmSet& workerFilms)
{
	const auto& lightFilm = workerFilms.get<EAttribute::LIGHT_ENERGY>();
	lightFilm->mergeToParent();
	lightFilm->clear();

	// HACK
	const auto& normalFilm = workerFilms.get<EAttribute::NORMAL>();
	normalFilm->mergeToParent();
	normalFilm->clear();
}

void SamplingRenderer::addUpdatedRegion(const Region& region, const bool isUpdating)
{
	for(UpdatedRegion& pendingRegion : m_updatedRegions)
	{
		// later added region takes the precedence
		if(pendingRegion.region.equals(region))
		{
			pendingRegion.isFinished = !isUpdating;
			return;
		}
	}

	m_updatedRegions.push_back(UpdatedRegion{region, !isUpdating});
}

RenderState SamplingRenderer::asyncQueryRenderState()
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
	state.setIntegerState(0, static_cast<int64>(m_averageSpp.load(std::memory_order_relaxed)));
	state.setRealState(0, samplesPerMs * 1000);
	return state;
}

RenderProgress SamplingRenderer::asyncQueryRenderProgress()
{
	RenderProgress totalProgress(0, 0, 0);
	{
		for(auto&& work : m_renderWorks)
		{
			totalProgress += work.asyncGetProgress();
		}
	}

	return totalProgress;
}

AttributeTags SamplingRenderer::supportedAttributes() const
{
	PH_ASSERT(m_estimator);
	return m_estimator->supportedAttributes();
}

std::string SamplingRenderer::renderStateName(const RenderState::EType type, const std::size_t index) const
{
	PH_ASSERT_LT(index, RenderState::numStates(type));

	if(type == RenderState::EType::INTEGER)
	{
		switch(index)
		{
		case 0:  return "samples/pixel";
		default: return "";
		}
	}
	else if(type == RenderState::EType::REAL)
	{
		switch(index)
		{
		case 0:  return "samples/second";
		default: return "";
		}
	}
	else
	{
		return "";
	}
}

std::size_t SamplingRenderer::numAvailableSampleBatches() const
{
	return m_sampleGenerator ? m_sampleGenerator->numSampleBatches() : 0;
}

// command interface

SamplingRenderer::SamplingRenderer(const InputPacket& packet) :

	Renderer(packet),

	m_films(),
	m_scene(nullptr),
	m_sampleGenerator(nullptr),
	m_estimator(nullptr),
	m_camera(nullptr),
	m_updatedRegions(),
	m_rendererMutex(),
	m_filter(SampleFilterFactory::createGaussianFilter()),
	m_requestedAttributes(),
	m_averageSpp(0)
{
	const std::string filterName = packet.getString("filter-name");
	m_filter = SampleFilterFactory::create(filterName);

	const std::string estimatorName = packet.getString("estimator", "bneept");
	if(estimatorName == "bvpt")
	{
		m_estimator = std::make_unique<BVPTEstimator>();
	}
	else if(estimatorName == "bneept")
	{
		m_estimator = std::make_unique<BNEEPTEstimator>();
	}

	const std::string lightEnergyTag = packet.getString("light-energy-tag", "true");
	const std::string normalTag      = packet.getString("normal-tag",       "false");
	if(lightEnergyTag == "true")
	{
		m_requestedAttributes.tag(EAttribute::LIGHT_ENERGY);
	}
	if(normalTag == "true")
	{
		m_requestedAttributes.tag(EAttribute::NORMAL);
	}

	/*const std::string regionSchedulerName = packet.getString("region-scheduler", "bulk");
	if(regionSchedulerName == "bulk")
	{
		m_workScheduler = std::make_unique<PlateScheduler>();
	}*/

	PH_ASSERT(m_estimator);
}

SdlTypeInfo SamplingRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "sampling");
}

void SamplingRenderer::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph