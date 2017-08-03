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
	m_totalWork(0), m_workDone(0)
{

}

RenderWorker::RenderWorker(const RenderWorker& other) : 
	m_renderer(other.m_renderer),
	m_id(other.m_id),
	m_totalWork(other.m_totalWork.load()), m_workDone(other.m_workDone.load())
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

void RenderWorker::doWork(const RenderWork& work)
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

	const std::size_t totalSamples = sg->numSamples();
	std::size_t currentSamples = 0;

	m_totalWork = static_cast<std::uint64_t>(totalSamples);

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

		currentSamples++;
		m_workDone = static_cast<std::uint64_t>(currentSamples);

		t2 = std::chrono::system_clock::now();

		//auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		//*workerSampleFreq = static_cast<float32>(filmWpx * filmHpx) / static_cast<float32>(msPassed.count()) * 1000.0f;

		sg->singleSampleEnd();

		m_renderer.submitWork(m_id, work, true);
	}
}

RenderWorker& RenderWorker::operator = (const RenderWorker& rhs)
{
	m_renderer  = rhs.m_renderer;
	m_id        = rhs.m_id;
	m_totalWork = rhs.m_totalWork.load();
	m_workDone  = rhs.m_workDone.load();

	return *this;
}

}// end namespace ph