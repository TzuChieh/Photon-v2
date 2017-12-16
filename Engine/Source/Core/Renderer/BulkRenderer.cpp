#include "Core/Renderer/BulkRenderer.h"
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

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <utility>

namespace ph
{

BulkRenderer::BulkRenderer() :
	Renderer()
{

}

BulkRenderer::~BulkRenderer() = default;

void BulkRenderer::init(const Description& description)
{
	const uint32 numWorks = m_numThreads;

	clearWorkData();
	m_scene      = &description.visualWorld.getScene();
	m_sg         = description.getSampleGenerator().get();
	m_integrator = description.getIntegrator().get();
	m_film       = description.getFilm().get();
	m_camera     = description.getCamera().get();
	m_numRemainingWorks = numWorks;
	m_numFinishedWorks  = 0;

	m_sg->genSplitted(numWorks, m_workSgs);

	for(uint32 i = 0; i < numWorks; i++)
	{
		m_workFilms.push_back(m_film->genChild(m_film->getEffectiveWindowPx()));
	}
}

bool BulkRenderer::getNewWork(const uint32 workerId, RenderWork* out_work)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_numRemainingWorks == 0)
	{
		return false;
	}

	const uint32 workIndex = m_numRemainingWorks - 1;
	*out_work = RenderWork(m_scene,
	                       m_camera,
	                       m_integrator,
	                       m_workSgs[workIndex].get(),
	                       m_workFilms[workIndex].get());
	m_numRemainingWorks--;

	return true;
}

void BulkRenderer::submitWork(const uint32 workerId, const RenderWork& work, const bool isUpdating)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	work.film->mergeToParent();
	work.film->clear();

	addUpdatedRegion(work.film->getEffectiveWindowPx(), isUpdating);
}

void BulkRenderer::clearWorkData()
{
	m_workSgs.clear();
	m_workFilms.clear();
	m_updatedRegions.clear();
}

ERegionStatus BulkRenderer::asyncPollUpdatedRegion(Region* const out_region)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_updatedRegions.empty())
	{
		return ERegionStatus::INVALID;
	}

	*out_region = m_updatedRegions.front().first;
	m_updatedRegions.pop_front();

	if(m_numFinishedWorks != m_workFilms.size())
	{
		return ERegionStatus::UPDATING;
	}
	else
	{
		return ERegionStatus::FINISHED;
	}
}

void BulkRenderer::asyncDevelopFilmRegion(TFrame<real>& out_frame, const Region& region)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	m_film->develop(out_frame, region);
}

void BulkRenderer::addUpdatedRegion(const Region& region, const bool isUpdating)
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

}// end namespace ph