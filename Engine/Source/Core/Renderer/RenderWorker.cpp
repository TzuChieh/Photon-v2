#include "Core/Renderer/RenderWorker.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SampleGenerator/TSamplePhase.h"
#include "Core/SampleGenerator/sample_array.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Integrator/Utility/SenseEvent.h"
#include "Core/Ray.h"
#include "Core/Integrator/Integrator.h"
#include "Core/Camera/Camera.h"
#include "World/Scene.h"
#include "Core/Renderer/RenderWork.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

namespace ph
{

RenderWorker::RenderWorker(const RendererProxy& renderer, 
                           const uint32 id) :
	m_renderer(renderer),
	m_id(id),
	m_statistics()
{}

RenderWorker::RenderWorker(const RenderWorker& other) : 
	m_renderer(other.m_renderer),
	m_id(other.m_id),
	m_statistics(other.m_statistics)
{}

void RenderWorker::run()
{
	RenderWork work;
	while(m_renderer.getNewWork(m_id, &work))
	{
		doWork(work);
		m_renderer.submitWork(m_id, work, false);
	}
}

void RenderWorker::doWork(RenderWork& work)
{
	work.integrator->integrate(, work.progress);

	m_renderer.submitWork(m_id, work, true);
	m_statistics.incrementWorkDone();
}

Statistics::Record RenderWorker::getStatistics() const
{
	return m_statistics.record();
}

RenderWorker& RenderWorker::operator = (const RenderWorker& rhs)
{
	m_renderer   = rhs.m_renderer;
	m_id         = rhs.m_id;
	m_statistics = rhs.m_statistics;

	return *this;
}

}// end namespace ph