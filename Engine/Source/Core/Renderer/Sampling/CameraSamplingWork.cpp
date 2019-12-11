#include "Core/Renderer/Sampling/CameraSamplingWork.h"
#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Estimator/Integrand.h"
#include "Utility/Timer.h"
#include "Core/Ray.h"

namespace ph
{

CameraSamplingWork::CameraSamplingWork() :
	CameraSamplingWork(nullptr)
{}

CameraSamplingWork::CameraSamplingWork(const Camera* const camera) :

	RenderWork(),

	m_camera         (camera),
	m_processors     (),
	m_sampleGenerator(nullptr),
	m_filmResPx      (1, 1),
	m_filmWindowPx   ({0, 0}, {1, 1}),
	m_sampleResPx    (1, 1),

	m_numSamplesTaken(0),
	m_onWorkStart    (nullptr),
	m_onWorkReport   (nullptr),
	m_onWorkFinish   (nullptr)
{}

CameraSamplingWork::CameraSamplingWork(CameraSamplingWork&& other) :

	RenderWork(other),

	m_camera         (other.m_camera),
	m_processors     (std::move(other.m_processors)),
	m_sampleGenerator(std::move(other.m_sampleGenerator)),
	m_filmResPx      (std::move(other.m_filmResPx)),
	m_filmWindowPx   (std::move(other.m_filmWindowPx)),
	m_sampleResPx    (std::move(other.m_sampleResPx)),

	m_numSamplesTaken(other.m_numSamplesTaken.load()),
	m_onWorkStart    (std::move(other.m_onWorkStart)),
	m_onWorkReport   (std::move(other.m_onWorkReport)),
	m_onWorkFinish   (std::move(other.m_onWorkFinish))
{}

SamplingStatistics CameraSamplingWork::asyncGetStatistics()
{
	SamplingStatistics statistics;
	statistics.numSamplesTaken = m_numSamplesTaken.load(std::memory_order_relaxed);
	return statistics;
}

void CameraSamplingWork::doWork()
{
	PH_ASSERT(m_camera);

	if(m_onWorkStart)
	{
		m_onWorkStart();
	}

	m_numSamplesTaken.store(0, std::memory_order_relaxed);
	setTotalWork(m_sampleGenerator->numSampleBatches());
	setWorkDone(0);
	setElapsedMs(0);

	const auto camSampleHandle = m_sampleGenerator->declareStageND(
		2,
		m_sampleResPx.product(),
		m_sampleResPx.toVector());

	const auto raySampleHandle = m_sampleGenerator->declareStageND(
		5,
		m_sampleResPx.product());

	const math::Vector2D ndcScale  = m_filmWindowPx.getExtents().div(m_filmResPx);
	const math::Vector2D ndcOffset = m_filmWindowPx.minVertex.div(m_filmResPx);

	Timer sampleTimer;

	std::uint32_t totalMs     = 0;
	std::size_t   batchNumber = 1;
	while(m_sampleGenerator->prepareSampleBatch())
	{
		sampleTimer.start();

		for(ISensedRayProcessor* processor : m_processors)
		{
			processor->onBatchStart(batchNumber);
		}

		const auto camSamples = m_sampleGenerator->getSamplesND(camSampleHandle);
		auto raySamples = m_sampleGenerator->getSamplesND(raySampleHandle);
		for(std::size_t si = 0; si < camSamples.numSamples(); si++)
		{
			const auto filmNdc    = math::Vector2D(camSamples[si]).mul(ndcScale).add(ndcOffset);
			SampleFlow sampleFlow = raySamples.readSampleAsFlow();

			Ray ray;
			m_camera->genSensedRay(math::Vector2R(filmNdc), &ray);

			// FIXME: this loop uses correlated samples, also some processors
			for(ISensedRayProcessor* processor : m_processors)
			{
				processor->process(filmNdc, ray, sampleFlow);
			}
		}
		m_numSamplesTaken.fetch_add(static_cast<uint32>(camSamples.numSamples()), std::memory_order_relaxed);

		if(m_onWorkReport)
		{
			m_onWorkReport();
		}

		for(ISensedRayProcessor* processor : m_processors)
		{
			processor->onBatchFinish(batchNumber);
		}
		++batchNumber;
		incrementWorkDone();

		sampleTimer.finish();
		totalMs += static_cast<std::uint32_t>(sampleTimer.getDeltaMs());
		setElapsedMs(totalMs);
	}

	if(m_onWorkFinish)
	{
		m_onWorkFinish();
	}
}

void CameraSamplingWork::setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator)
{
	m_sampleGenerator = std::move(sampleGenerator);
}

void CameraSamplingWork::setSampleDimensions(
	const math::TVector2<int64>&  filmResPx,
	const math::TAABB2D<float64>& filmWindowPx,
	const math::TVector2<int64>&  sampleResPx)
{
	PH_ASSERT_MSG(filmWindowPx.isValid(), filmWindowPx.toString());

	m_filmResPx    = math::Vector2D(filmResPx);
	m_filmWindowPx = filmWindowPx;
	m_sampleResPx  = math::Vector2S(sampleResPx);
}

void CameraSamplingWork::addProcessor(ISensedRayProcessor* const processor)
{
	PH_ASSERT(processor);

	m_processors.push_back(processor);
}

void CameraSamplingWork::onWorkStart(std::function<void()> func)
{
	m_onWorkStart = std::move(func);
}

void CameraSamplingWork::onWorkReport(std::function<void()> func)
{
	m_onWorkReport = std::move(func);
}

void CameraSamplingWork::onWorkFinish(std::function<void()> func)
{
	m_onWorkFinish = std::move(func);
}

CameraSamplingWork& CameraSamplingWork::operator = (CameraSamplingWork&& other)
{
	RenderWork::operator = (std::move(other));

	m_camera          = other.m_camera;
	m_processors      = std::move(other.m_processors);
	m_sampleGenerator = std::move(other.m_sampleGenerator);
	m_filmResPx       = std::move(other.m_filmResPx);
	m_filmWindowPx    = std::move(other.m_filmWindowPx);
	m_sampleResPx     = std::move(other.m_sampleResPx);

	m_numSamplesTaken = other.m_numSamplesTaken.load();
	m_onWorkStart     = std::move(other.m_onWorkStart);
	m_onWorkReport    = std::move(other.m_onWorkReport);
	m_onWorkFinish    = std::move(other.m_onWorkFinish);

	return *this;
}

}// end namespace ph
