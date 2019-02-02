#include "Core/Renderer/Sampling/SamplingRenderWork.h"
#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Estimator/Estimator.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "Core/Estimator/Estimation.h"
#include "Utility/Timer.h"
#include "Core/Ray.h"

namespace ph
{

SamplingRenderWork::SamplingRenderWork(SamplingRenderWork&& other) : 

	RenderWork(other),

	m_estimator          (other.m_estimator),
	m_integrand          (other.m_integrand),
	m_renderer           (other.m_renderer),
	m_films              (std::move(other.m_films)),
	m_sampleGenerator    (std::move(other.m_sampleGenerator)),
	m_requestedAttributes(std::move(other.m_requestedAttributes)),
	m_numSamplesTaken    (other.m_numSamplesTaken.load())
{}

void SamplingRenderWork::doWork()
{
	const auto& lightFilm = m_films.get<EAttribute::LIGHT_ENERGY>();

	const Vector2D rasterSize(lightFilm->getEffectiveResPx());
	const Vector2D rasterSampleSize(lightFilm->getSampleResPx());

	const Vector2D sampleMinVertex = lightFilm->getSampleWindowPx().minVertex;
	const Vector2D sampleMaxVertex = lightFilm->getSampleWindowPx().maxVertex;
	
	const uint64 numCamStageSamples = static_cast<uint64>(lightFilm->getEffectiveResPx().product());

	const Vector2D ndcScale(rasterSampleSize.div(Vector2D(lightFilm->getActualResPx())));
	const Vector2D ndcOffset(sampleMinVertex.div(Vector2D(lightFilm->getActualResPx())));

	Samples2DStage camSampleStage = m_sampleGenerator->declare2DStage(
		numCamStageSamples,
		Vector2S(lightFilm->getEffectiveResPx()));

	/*Samples2DStage camSampleStage = m_sampleGenerator->declare2DStage(
		numCamStageSamples,
		{1, 1});*/

	m_numSamplesTaken = 0;
	setTotalWork(m_sampleGenerator->numSampleBatches());
	setWorkDone(0);
	setElapsedMs(0);

	Estimation estimation;
	Timer sampleTimer;

	std::uint32_t totalMs = 0;
	std::size_t batchNumber = 0;
	while(m_sampleGenerator->prepareSampleBatch())
	{
		sampleTimer.start();

		const Samples2D& camSamples = m_sampleGenerator->getSamples2D(camSampleStage);

		for(std::size_t si = 0; si < camSamples.numSamples(); si++)
		{
			const Vector2D rasterPosPx(camSamples[si].x * rasterSampleSize.x + sampleMinVertex.x,
			                           camSamples[si].y * rasterSampleSize.y + sampleMinVertex.y);

			// TODO: check rare, this check is probably unnecessary now
			if(!lightFilm->getSampleWindowPx().isIntersectingArea(rasterPosPx))
			{
				continue;
			}

			const Vector2R filmNdcPos(Vector2D(camSamples[si]).mul(ndcScale).add(ndcOffset));

			Ray ray;
			m_integrand.getCamera().genSensedRay(filmNdcPos, &ray);

			m_estimator->estimate(ray, m_integrand, m_requestedAttributes, estimation);

			if(m_films.get<EAttribute::LIGHT_ENERGY>())
			{
				m_films.get<EAttribute::LIGHT_ENERGY>()->addSample(rasterPosPx.x, rasterPosPx.y, estimation.get<EAttribute::LIGHT_ENERGY>());
			}
			
			if(m_films.get<EAttribute::NORMAL>())
			{
				m_films.get<EAttribute::NORMAL>()->addSample(rasterPosPx.x, rasterPosPx.y, estimation.get<EAttribute::NORMAL>());
			}

			if(m_films.get<EAttribute::LIGHT_ENERGY_HALF_EFFORT>())
			{
				if(batchNumber % 2 == 0)
				{
					m_films.get<EAttribute::LIGHT_ENERGY_HALF_EFFORT>()->addSample(rasterPosPx.x, rasterPosPx.y, estimation.get<EAttribute::LIGHT_ENERGY>());
				}
			}
			
		}// end for

		const bool isUpdating = m_sampleGenerator->hasMoreBatches();
		m_renderer->asyncUpdateFilm(m_films, isUpdating);
		incrementWorkDone();	

		sampleTimer.finish();
		totalMs += static_cast<std::uint32_t>(sampleTimer.getDeltaMs());
		setElapsedMs(totalMs);

		m_numSamplesTaken += static_cast<uint32>(camSamples.numSamples());

		++batchNumber;
	}
}

void SamplingRenderWork::setDomainPx(const TAABB2D<int64>& domainPx)
{
	PH_ASSERT(domainPx.isValid());

	m_films.setEffectiveWindowPx(domainPx);
}

}// end namespace ph