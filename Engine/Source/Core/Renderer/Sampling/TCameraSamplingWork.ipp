#pragma once

#include "Core/Renderer/Sampling/TCameraSamplingWork.h"
#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Estimator/EnergyEstimation.h"
#include "Utility/Timer.h"
#include "Core/Ray.h"

namespace ph
{

template<typename Processor>
inline TCameraSamplingWork<Processor>::TCameraSamplingWork() :
	TCameraSamplingWork(nullptr, nullptr)
{}

template<typename Processor>
inline TCameraSamplingWork<Processor>::TCameraSamplingWork(
	const Camera* const camera,
	Processor*    const processor) :

	RenderWork(),

	m_camera(camera),
	m_processor(processor),
	m_sampleGenerator(nullptr),
	m_filmActualResPx(1, 1),
	m_filmEffectiveWindowPx({0, 0}, {1, 1}),
	m_filmSampleWindowPx({0, 0}, {1, 1}),

	m_numSamplesTaken(0),
	m_onWorkStart    (nullptr),
	m_onWorkReport   (nullptr),
	m_onWorkFinish   (nullptr)
{}

template<typename Processor>
inline TCameraSamplingWork<Processor>::TCameraSamplingWork(
	TCameraSamplingWork&& other) :

	RenderWork(other),

	m_camera(other.m_camera),
	m_processor(other.m_processor),
	m_sampleGenerator(std::move(other.m_sampleGenerator)),
	m_filmActualResPx(std::move(other.m_filmActualResPx)),
	m_filmEffectiveWindowPx(std::move(other.m_filmEffectiveWindowPx)),
	m_filmSampleWindowPx(std::move(other.m_filmSampleWindowPx)),

	m_numSamplesTaken(other.m_numSamplesTaken.load()),
	m_onWorkStart(std::move(other.m_onWorkStart)),
	m_onWorkReport(std::move(other.m_onWorkReport)),
	m_onWorkFinish(std::move(other.m_onWorkFinish))
{}

template<typename Processor>
inline SamplingStatistics TCameraSamplingWork<Processor>::asyncGetStatistics()
{
	SamplingStatistics statistics;
	statistics.numSamplesTaken = m_numSamplesTaken;

	return statistics;
}

template<typename Processor>
inline void TCameraSamplingWork<Processor>::doWork()
{
	if(m_onWorkStart)
	{
		m_onWorkStart();
	}

	const Vector2S effectiveResPx = Vector2S(m_filmEffectiveWindowPx.getExtents());
	const Vector2D sampleResPx = m_filmSampleWindowPx.getExtents();

	const Vector2D ndcScale = sampleResPx.div(m_filmActualResPx);
	const Vector2D ndcOffset = m_filmSampleWindowPx.minVertex.div(m_filmActualResPx);

	Samples2DStage camSampleStage = m_sampleGenerator->declare2DStage(
		effectiveResPx.product(),
		effectiveResPx);

	m_numSamplesTaken = 0;
	setTotalWork(m_sampleGenerator->numSampleBatches());
	setWorkDone(0);
	setElapsedMs(0);

	EnergyEstimation estimation(1);

	Timer sampleTimer;

	std::uint32_t totalMs = 0;
	std::size_t batchNumber = 0;
	while(m_sampleGenerator->prepareSampleBatch())
	{
		sampleTimer.start();

		const Samples2D& camSamples = m_sampleGenerator->getSamples2D(camSampleStage);

		for(std::size_t si = 0; si < camSamples.numSamples(); si++)
		{
			/*const Vector2D rasterPosPx(camSamples[si].x * sampleResPx.x + m_filmSampleWindowPx.minVertex.x,
			                           camSamples[si].y * sampleResPx.y + m_filmSampleWindowPx.minVertex.y);*/

			// TODO: check rare, this check is probably unnecessary now
			/*if(!lightFilm->getSampleWindowPx().isIntersectingArea(rasterPosPx))
			{
				continue;
			}*/

			const Vector2D filmNdc = Vector2D(camSamples[si]).mul(ndcScale).add(ndcOffset);

			Ray ray;
			m_camera->genSensedRay(Vector2R(filmNdc), &ray);
			m_processor->process(filmNdc, ray);
		}// end for

		if(m_onWorkReport)
		{
			m_onWorkReport();
		}

		incrementWorkDone();	

		sampleTimer.finish();
		totalMs += static_cast<std::uint32_t>(sampleTimer.getDeltaMs());
		setElapsedMs(totalMs);

		m_numSamplesTaken += static_cast<uint32>(camSamples.numSamples());

		++batchNumber;
	}

	if(m_onWorkFinish)
	{
		m_onWorkFinish();
	}
}

template<typename Processor>
inline void TCameraSamplingWork<Processor>::setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator)
{
	m_sampleGenerator = std::move(sampleGenerator);
}

template<typename Processor>
inline void TCameraSamplingWork<Processor>::setFilmDimensions(
	const Vector2S&         actualResPx,
	const TAABB2D<int64>&   effectiveWindowPx,
	const TAABB2D<float64>& sampleWindowPx)
{
	PH_ASSERT_MSG(effectiveWindowPx.isValid(), effectiveWindowPx.toString());
	PH_ASSERT_MSG(sampleWindowPx.isValid(),    sampleWindowPx.toString());

	m_filmActualResPx       = Vector2D(actualResPx);
	m_filmEffectiveWindowPx = effectiveWindowPx;
	m_filmSampleWindowPx    = sampleWindowPx;

	std::cerr << "csw " << m_filmEffectiveWindowPx.toString() << std::endl;
}

template<typename Processor>
inline void TCameraSamplingWork<Processor>::onWorkStart(std::function<void()> func)
{
	m_onWorkStart = std::move(func);
}

template<typename Processor>
inline void TCameraSamplingWork<Processor>::onWorkReport(std::function<void()> func)
{
	m_onWorkReport = std::move(func);
}

template<typename Processor>
inline void TCameraSamplingWork<Processor>::onWorkFinish(std::function<void()> func)
{
	m_onWorkFinish = std::move(func);
}

template<typename Processor>
inline TCameraSamplingWork<Processor>& TCameraSamplingWork<Processor>::operator = (TCameraSamplingWork&& other)
{
	RenderWork::operator = (std::move(other));

	m_camera = other.m_camera;
	m_processor = other.m_processor;
	m_sampleGenerator = std::move(other.m_sampleGenerator);
	m_filmActualResPx = std::move(other.m_filmActualResPx);
	m_filmEffectiveWindowPx = std::move(other.m_filmEffectiveWindowPx);
	m_filmSampleWindowPx = std::move(other.m_filmSampleWindowPx);

	m_numSamplesTaken = other.m_numSamplesTaken.load();
	m_onWorkStart = std::move(other.m_onWorkStart);
	m_onWorkReport = std::move(other.m_onWorkReport);
	m_onWorkFinish = std::move(other.m_onWorkFinish);

	return *this;
}

}// end namespace ph