#include "Core/Renderer/RenderWorker.h"
#include "Core/Filmic/Film.h"
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
{

}

RenderWorker::RenderWorker(const RenderWorker& other) : 
	m_renderer(other.m_renderer),
	m_id(other.m_id),
	m_statistics(other.m_statistics)
{

}

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
	const Camera* const     camera     = work.camera;
	const Integrator* const integrator = work.integrator;
	const Scene* const      scene      = work.scene;
	SampleGenerator* const  sg         = work.sampleGenerator;
	Film* const             film       = work.film;

	const uint64 filmWpx = film->getEffectiveResPx().x;
	const uint64 filmHpx = film->getEffectiveResPx().y;

	const Vector2D flooredSampleMinVertex = film->getSampleWindowPx().minVertex.floor();
	const Vector2D ceiledSampleMaxVertex  = film->getSampleWindowPx().maxVertex.ceil();
	const uint64 filmSampleWpx = static_cast<uint64>(ceiledSampleMaxVertex.x - flooredSampleMinVertex.x);
	const uint64 filmSampleHpx = static_cast<uint64>(ceiledSampleMaxVertex.y - flooredSampleMinVertex.y);
	const uint64 numCamPhaseSamples = filmSampleWpx * filmSampleHpx;

	TSamplePhase<SampleArray2D> camSamplePhase = sg->declareArray2DPhase(numCamPhaseSamples);

	std::vector<SenseEvent> senseEvents;

	m_statistics.setTotalWork(static_cast<uint32>(sg->numSamples()));
	m_statistics.setWorkDone(0);

	std::chrono::time_point<std::chrono::system_clock> t1;
	std::chrono::time_point<std::chrono::system_clock> t2;

	while(sg->singleSampleStart())
	{
		t1 = std::chrono::system_clock::now();

		const SampleArray2D& camSamples = sg->getNextArray2D(camSamplePhase);

		for(std::size_t si = 0; si < camSamples.numElements(); si++)
		{
			const Vector2D rasterPosPx(camSamples[si].x * filmSampleWpx + flooredSampleMinVertex.x,
			                           camSamples[si].y * filmSampleHpx + flooredSampleMinVertex.y);

			if(!film->getSampleWindowPx().isIntersectingArea(rasterPosPx))
			{
				continue;
			}

			Ray ray;
			camera->genSensingRay(Vector2R(rasterPosPx), &ray);

			integrator->radianceAlongRay(ray, work, senseEvents);

			// HACK: sense event
			for(const auto& senseEvent : senseEvents)
			{
				film->addSample(rasterPosPx.x, rasterPosPx.y, senseEvent.radiance);
			}

			if(senseEvents.size() != 1)
			{
				std::cerr << "unexpected event occured" << std::endl;
			}

			senseEvents.clear();
		}// end for

		sg->singleSampleEnd();

		m_renderer.submitWork(m_id, work, true);
		m_statistics.incrementWorkDone();
	
		t2 = std::chrono::system_clock::now();

		m_statistics.setNumSamplesTaken(static_cast<uint32>(camSamples.numElements()));
		m_statistics.setNumMsElapsed(static_cast<uint32>(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()));
	}

	// FIXME: sort of hacking
	m_statistics.setNumSamplesTaken(0);
	m_statistics.setNumMsElapsed(0);
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