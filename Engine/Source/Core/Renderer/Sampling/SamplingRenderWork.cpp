#include "Core/Renderer/Sampling/SamplingRenderWork.h"
#include "Core/Renderer/SamplingRenderer.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Estimator/Utility/SenseEvent.h"
#include "Core/Estimator/Estimator.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/SampleFilterFactory.h"

namespace ph
{

SamplingRenderWork::SamplingRenderWork(SamplingRenderWork&& other) : 

	RenderWork(other),

	m_renderer(other.m_renderer),
	m_integrand(other.m_integrand),
	m_estimator(other.m_estimator),
	m_sampleGenerator(std::move(other.m_sampleGenerator)),
	m_film(std::move(other.m_film)),

	m_numSamplesTaken(other.m_numSamplesTaken.load()),
	m_numMsElapsed(other.m_numMsElapsed.load())
{}

void SamplingRenderWork::doWork()
{
	const uint64 filmWpx = m_film->getEffectiveResPx().x;
	const uint64 filmHpx = m_film->getEffectiveResPx().y;

	const Vector2D flooredSampleMinVertex = m_film->getSampleWindowPx().minVertex.floor();
	const Vector2D ceiledSampleMaxVertex  = m_film->getSampleWindowPx().maxVertex.ceil();
	const uint64 filmSampleWpx = static_cast<uint64>(ceiledSampleMaxVertex.x - flooredSampleMinVertex.x);
	const uint64 filmSampleHpx = static_cast<uint64>(ceiledSampleMaxVertex.y - flooredSampleMinVertex.y);
	const uint64 numCamPhaseSamples = filmSampleWpx * filmSampleHpx;

	TSamplePhase<SampleArray2D> camSamplePhase = m_sampleGenerator->declareArray2DPhase(numCamPhaseSamples);

	std::vector<SenseEvent> senseEvents;

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
			m_integrand.getCamera().genSensedRay(Vector2R(rasterPosPx), &ray);

			m_estimator->radianceAlongRay(ray, m_integrand, senseEvents);

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

		incrementWorkDone();
	
		t2 = std::chrono::system_clock::now();

		m_numSamplesTaken = static_cast<uint32>(camSamples.numElements());
		m_numMsElapsed    = static_cast<uint32>(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
	}
}

void SamplingRenderWork::setDomainPx(const TAABB2D<int64>& domainPx)
{
	PH_ASSERT(m_film && domainPx.isValid());

	m_domainPx = domainPx;

	m_film->setEffectiveWindowPx(domainPx);
}

}// end namespace ph