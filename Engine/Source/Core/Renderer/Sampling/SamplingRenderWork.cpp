#include "Core/Renderer/Sampling/SamplingRenderWork.h"
#include "Core/Renderer/SamplingRenderer.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Estimator/Utility/SenseEvent.h"
#include "Core/Estimator/Estimator.h"
#include "Core/Estimator/Integrand.h"

namespace ph
{

void SamplingRenderWork::doWork()
{
	const uint64 filmWpx = m_film->getEffectiveResPx().x;
	const uint64 filmHpx = m_film->getEffectiveResPx().y;

	const Vector2D flooredSampleMinVertex = m_film->getSampleWindowPx().minVertex.floor();
	const Vector2D ceiledSampleMaxVertex  = m_film->getSampleWindowPx().maxVertex.ceil();
	const uint64 filmSampleWpx = static_cast<uint64>(ceiledSampleMaxVertex.x - flooredSampleMinVertex.x);
	const uint64 filmSampleHpx = static_cast<uint64>(ceiledSampleMaxVertex.y - flooredSampleMinVertex.y);
	const uint64 numCamPhaseSamples = filmSampleWpx * filmSampleHpx;

	TSamplePhase<SampleArray2D> camSamplePhase = sg->declareArray2DPhase(numCamPhaseSamples);

	std::vector<SenseEvent> senseEvents;

	const Integrand integrand(m_scene, m_camera);

	m_numSamplesTaken = 0;
	m_numMsElapsed    = 0;
	setTotalWork(static_cast<uint32>(m_sampleGenerator->numSamples()));
	setWorkDone(0);

	std::chrono::time_point<std::chrono::system_clock> t1;
	std::chrono::time_point<std::chrono::system_clock> t2;

	while(m_sampleGenerator->singleSampleStart())
	{
		t1 = std::chrono::system_clock::now();

		const SampleArray2D& camSamples = m_sampleGenerator->getNextArray2D(camSamplePhase);

		for(std::size_t si = 0; si < camSamples.numElements(); si++)
		{
			const Vector2D rasterPosPx(camSamples[si].x * filmSampleWpx + flooredSampleMinVertex.x,
			                           camSamples[si].y * filmSampleHpx + flooredSampleMinVertex.y);

			if(!m_film->getSampleWindowPx().isIntersectingArea(rasterPosPx))
			{
				continue;
			}

			Ray ray;
			m_camera->genSensedRay(Vector2R(rasterPosPx), &ray);

			m_estimator->radianceAlongRay(ray, integrand, senseEvents);

			// HACK: sense event
			for(const auto& senseEvent : senseEvents)
			{
				m_film->addSample(rasterPosPx.x, rasterPosPx.y, senseEvent.radiance);
			}

			if(senseEvents.size() != 1)
			{
				std::cerr << "unexpected event occured" << std::endl;
			}

			senseEvents.clear();
		}// end for

		m_sampleGenerator->singleSampleEnd();

		m_renderer.submitWork(m_id, work, true);
		incrementWorkDone();
	
		t2 = std::chrono::system_clock::now();

		m_numSamplesTaken = static_cast<uint32>(camSamples.numElements());
		m_numMsElapsed    = static_cast<uint32>(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
	}
}

}// end namespace ph