#include "Core/Renderer/SamplingRenderer.h"
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
	AttributeTags supports;
	supports.tag(EAttribute::LIGHT_ENERGY);
	return supports;
}

void SamplingRenderer::init(const Description& description)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	const uint32 numWorks = getNumRenderThreads();

	clearWorkData();
	m_scene           = &description.visualWorld.getScene();
	m_sg              = description.getSampleGenerator().get();

	m_films.set<EAttribute::LIGHT_ENERGY>(std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter));

	m_camera     = description.getCamera().get();
	m_numRemainingWorks = numWorks;
	m_numFinishedWorks  = 0;

	m_sg->genSplitted(numWorks, m_workSgs);

	std::vector<SamplingFilmSet> workFilms;
	for(uint32 i = 0; i < numWorks; i++)
	{
		workFilms.push_back(m_films.genChild(getRenderWindowPx()));
	}

	const Integrand integrand(m_scene, m_camera);
	for(uint32 i = 0; i < numWorks; i++)
	{
		SamplingRenderWork work(this, m_estimator.get(), integrand, std::move(workFilms[i]), std::move(m_workSgs[i]));
		work.setDomainPx(getRenderWindowPx());
		m_works.push_back(std::move(work));
	}

	m_estimator->update(*m_scene);
}

bool SamplingRenderer::asyncSupplyWork(RenderWorker& worker)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_numRemainingWorks == 0)
	{
		return false;
	}

	worker.setWork(&(m_works[worker.getId()]));
	m_numRemainingWorks--;

	return true;
}

void SamplingRenderer::asyncMergeFilm(RenderWorker& worker)
{
	// TODO
}

void SamplingRenderer::asyncSubmitWork(RenderWorker& worker)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	SamplingRenderWork* work = &(m_works[worker.getId()]);

	const auto& lightFilm = work->m_films.get<EAttribute::LIGHT_ENERGY>();
	lightFilm->mergeToParent();
	lightFilm->clear();

	addUpdatedRegion(lightFilm->getEffectiveWindowPx(), false);
}

// TODO: check this
void SamplingRenderer::clearWorkData()
{
	m_workSgs.clear();
	m_updatedRegions.clear();
}

ERegionStatus SamplingRenderer::asyncPollUpdatedRegion(Region* const out_region)
{
	PH_ASSERT(out_region != nullptr);

	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_updatedRegions.empty())
	{
		return ERegionStatus::INVALID;
	}

	*out_region = m_updatedRegions.front().first;
	m_updatedRegions.pop_front();

	if(m_numFinishedWorks != m_works.size())
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
	// TODO: attribute

	std::lock_guard<std::mutex> lock(m_rendererMutex);

	const SamplingFilmBase* film = m_films.get(attribute);
	if(film)
	{
		film->develop(out_frame, region);
	}
}

void SamplingRenderer::develop(HdrRgbFrame& out_frame, const EAttribute attribute)
{
	const SamplingFilmBase* film = m_films.get(attribute);
	if(film)
	{
		film->develop(out_frame);
	}
}

void SamplingRenderer::addUpdatedRegion(const Region& region, const bool isUpdating)
{
	if(!isUpdating)
	{
		m_numFinishedWorks++;
	}

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

	float32 samplesPerMs = 0.0f;
	if(totalElapsedMs != 0)
	{
		samplesPerMs = static_cast<float32>(m_works.size() * totalNumSamples) / static_cast<float32>(totalElapsedMs);
	}

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
	m_numRemainingWorks(0),
	m_numFinishedWorks(0),
	m_workSgs(),
	m_updatedRegions(),
	m_rendererMutex(),
	m_filter(SampleFilterFactory::createGaussianFilter())
{
	const std::string filterName = packet.getString("filter-name");
	if(filterName == "box")
	{
		m_filter = SampleFilterFactory::createBoxFilter();
	}
	else if(filterName == "gaussian")
	{
		m_filter = SampleFilterFactory::createGaussianFilter();
	}
	else if(filterName == "mn")
	{
		m_filter = SampleFilterFactory::createMNFilter();
	}

	const std::string estimatorName = packet.getString("estimator", "bneept");
	if(estimatorName == "bvpt")
	{
		m_estimator = std::make_unique<BVPTEstimator>();
	}
	else if(estimatorName == "bneept")
	{
		m_estimator = std::make_unique<BNEEPTEstimator>();
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