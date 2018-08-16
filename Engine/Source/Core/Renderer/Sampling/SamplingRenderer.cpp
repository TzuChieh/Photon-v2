#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Common/primitive_type.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Core/Camera/Camera.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/Description.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Common/assertion.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "Core/Estimator/BVPTEstimator.h"
#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/Vec3Film.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <utility>

namespace ph
{

SamplingRenderer::~SamplingRenderer() = default;

AttributeTags SamplingRenderer::supportedAttributes() const
{
	PH_ASSERT(m_estimator);
	return m_estimator->supportedAttributes();
}

void SamplingRenderer::init(const Description& description)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	clearWorkData();

	m_scene  = &description.visualWorld.getScene();
	m_camera = description.getCamera().get();
	m_sg     = description.getSampleGenerator().get();

	m_estimator->update(*m_scene);

	m_films.set<EAttribute::LIGHT_ENERGY>(std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter));

	// HACK
	m_films.set<EAttribute::NORMAL>(std::make_unique<Vec3Film>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter));

	m_works.resize(getNumRenderThreads());

	RegionScheduler* scheduler = getRegionScheduler();
	scheduler->setNumWorkers(getNumRenderThreads());
	scheduler->setFullRegion(getRenderWindowPx());
	scheduler->setSppBudget(m_sg->numSampleBatches());
	scheduler->init();
}

bool SamplingRenderer::asyncSupplyWork(RenderWorker& worker)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	RegionScheduler* scheduler = getRegionScheduler();
	
	Region region;
	uint64 spp;
	if(!scheduler->scheduleRegion(&region, &spp))
	{
		return false;
	}

	SamplingRenderWork work(
		this,
		m_estimator.get(),
		Integrand(m_scene, m_camera),
		m_films.genChild(getRenderWindowPx()),
		m_sg->genCopied(spp),
		m_requestedAttributes);
	work.setDomainPx(getRenderWindowPx());
	m_works[worker.getId()] = std::move(work);

	worker.setWork(&(m_works[worker.getId()]));

	float bestProgress, worstProgress;
	scheduler->percentageProgress(&bestProgress, &worstProgress);
	m_percentageProgress = static_cast<unsigned int>(worstProgress);

	return true;
}

void SamplingRenderer::asyncUpdateFilm(SamplingRenderWork& work)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	mergeWorkFilms(work);

	// HACK
	addUpdatedRegion(work.m_films.get<EAttribute::LIGHT_ENERGY>()->getEffectiveWindowPx(), false);
}

void SamplingRenderer::asyncSubmitWork(RenderWorker& worker)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	mergeWorkFilms(m_works[worker.getId()]);

	// HACK
	addUpdatedRegion(m_works[worker.getId()].m_films.get<EAttribute::LIGHT_ENERGY>()->getEffectiveWindowPx(), true);
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

	const auto regionInfo = m_updatedRegions.front();
	m_updatedRegions.pop_front();

	*out_region = regionInfo.first;
	if(regionInfo.second)
	{
		return ERegionStatus::UPDATING;
	}
	else
	{
		return ERegionStatus::FINISHED;
	}
}

void SamplingRenderer::asyncDevelopFilmRegion(
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
	const SamplingFilmBase* film = m_films.get(attribute);
	if(film)
	{
		film->develop(out_frame);
	}
	else
	{
		out_frame.fill(0);
	}
}

void SamplingRenderer::mergeWorkFilms(SamplingRenderWork& work)
{
	const auto& lightFilm = work.m_films.get<EAttribute::LIGHT_ENERGY>();
	lightFilm->mergeToParent();
	lightFilm->clear();

	// HACK
	const auto& normalFilm = work.m_films.get<EAttribute::NORMAL>();
	normalFilm->mergeToParent();
	normalFilm->clear();
}

void SamplingRenderer::addUpdatedRegion(const Region& region, const bool isUpdating)
{
	for(auto& pendingRegion : m_updatedRegions)
	{
		if(pendingRegion.first.equals(region))
		{
			pendingRegion.second = isUpdating;
			return;
		}
	}

	m_updatedRegions.push_back(std::make_pair(region, isUpdating));
}

RenderStates SamplingRenderer::asyncQueryRenderStates()
{
	uint64 totalElapsedMs  = 0;
	uint64 totalNumSamples = 0;
	for(auto& work : m_works)
	{
		const auto statistics = work.asyncGetStatistics();
		totalElapsedMs  += statistics.numMsElapsed;
		totalNumSamples += statistics.numSamplesTaken;
	}

	float32 samplesPerMs = totalElapsedMs != 0 ? 
		static_cast<float32>(m_works.size() * totalNumSamples) / static_cast<float32>(totalElapsedMs) : 0.0f;

	uint64 totalWork     = 0;
	uint64 totalWorkDone = 0;
	for(uint32 workerId = 0; workerId < getNumRenderThreads(); workerId++)
	{
		const auto progress = asyncQueryWorkerProgress(workerId);

		totalWork     += progress.totalWork;
		totalWorkDone += progress.workDone;
	}

	float32 workerProgress = totalWork != 0 ?
		static_cast<float32>(totalWorkDone) / static_cast<float32>(totalWork) : 0.0f;


	RenderStates states;
	states.fltStates[0] = samplesPerMs;

	return states;
}

// command interface

SamplingRenderer::SamplingRenderer(const InputPacket& packet) :

	Renderer(packet),

	m_films(),
	m_scene(nullptr),
	m_sg(nullptr),
	m_estimator(nullptr),
	m_camera(nullptr),
	m_updatedRegions(),
	m_rendererMutex(),
	m_filter(SampleFilterFactory::createGaussianFilter()),
	m_requestedAttributes(),
	m_percentageProgress(0)
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

	PH_ASSERT(m_estimator);
}

SdlTypeInfo SamplingRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "sampling");
}

void SamplingRenderer::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<SamplingRenderer>([](const InputPacket& packet)
	{
		return std::make_unique<SamplingRenderer>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph