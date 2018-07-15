#include "Core/Renderer/Sampling/SamplingRenderWork.h"
#include "Core/Renderer/SamplingRenderer.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Estimator/Utility/SenseEvent.h"
#include "Core/Estimator/Estimator.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "Core/Estimator/Estimation.h"

namespace ph
{

SamplingRenderWork::SamplingRenderWork(SamplingRenderWork&& other) : 

	RenderWork(other),

	m_renderer(other.m_renderer),
	m_integrand(other.m_integrand),
	m_estimator(other.m_estimator),
	m_sampleGenerator(std::move(other.m_sampleGenerator)),
	m_films(std::move(other.m_films)),
	m_requestedAttributes(std::move(other.m_requestedAttributes)),

	m_numSamplesTaken(other.m_numSamplesTaken.load()),
	m_numMsElapsed(other.m_numMsElapsed.load())
{}

void SamplingRenderWork::doWork()
{
	const auto& lightFilm = m_films.get<EAttribute::LIGHT_ENERGY>();

	const Vector2D rasterSize(lightFilm->getEffectiveResPx());
	const Vector2D rasterSampleSize(lightFilm->getSampleResPx());

	const Vector2D sampleMinVertex = lightFilm->getSampleWindowPx().minVertex;
	const Vector2D sampleMaxVertex = lightFilm->getSampleWindowPx().maxVertex;
	
	const uint64 numCamPhaseSamples = static_cast<uint64>(rasterSampleSize.x * rasterSampleSize.y + 0.5);

	const Vector2D ndcScale(rasterSampleSize.div(rasterSize));
	const Vector2D ndcOffset(sampleMinVertex.div(rasterSize));

	TSamplePhase<SampleArray2D> camSamplePhase = m_sampleGenerator->declareArray2DPhase(numCamPhaseSamples);

	Estimation estimation;

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
			const Vector2D rasterPosPx(camSamples[si].x * rasterSampleSize.x + sampleMinVertex.x,
			                           camSamples[si].y * rasterSampleSize.y + sampleMinVertex.y);

			// TODO: this check is probably unnecessary now
			if(!lightFilm->getSampleWindowPx().isIntersectingArea(rasterPosPx))
			{
				continue;
			}

			const Vector2R filmNdcPos(Vector2D(camSamples[si]).mul(ndcScale).add(ndcOffset));

			Ray ray;
			m_integrand.getCamera().genSensedRay(filmNdcPos, &ray);

			m_estimator->estimate(ray, m_integrand, m_requestedAttributes, estimation);

			if(m_requestedAttributes.isTagged(EAttribute::LIGHT_ENERGY))
			{
				m_films.get<EAttribute::LIGHT_ENERGY>()->addSample(rasterPosPx.x, rasterPosPx.y, estimation.get<EAttribute::LIGHT_ENERGY>());
			}
			
			if(m_requestedAttributes.isTagged(EAttribute::NORMAL))
			{
				m_films.get<EAttribute::NORMAL>()->addSample(rasterPosPx.x, rasterPosPx.y, estimation.get<EAttribute::NORMAL>());
			}
		}// end for

		m_sampleGenerator->singleSampleEnd();

		incrementWorkDone();
	
		t2 = std::chrono::system_clock::now();

		m_numSamplesTaken = static_cast<uint32>(camSamples.numElements());
		m_numMsElapsed    = static_cast<uint32>(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

		m_renderer->asyncUpdateFilm(*this);
	}
}

void SamplingRenderWork::setDomainPx(const TAABB2D<int64>& domainPx)
{
	// HACK

	PH_ASSERT(domainPx.isValid());

	m_domainPx = domainPx;

	m_films.setEffectiveWindowPx(domainPx);
}

}// end namespace ph