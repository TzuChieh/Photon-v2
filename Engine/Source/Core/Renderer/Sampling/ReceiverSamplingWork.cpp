#include "Core/Renderer/Sampling/ReceiverSamplingWork.h"
#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Receiver/Receiver.h"
#include "Core/Estimator/Integrand.h"
#include "Utility/Timer.h"
#include "Core/Ray.h"
#include "Math/Random/sample.h"

namespace ph
{

ReceiverSamplingWork::ReceiverSamplingWork() :
	ReceiverSamplingWork(nullptr)
{}

ReceiverSamplingWork::ReceiverSamplingWork(const Receiver* const receiver) :

	RenderWork(),

	m_receiver       (receiver),
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

ReceiverSamplingWork::ReceiverSamplingWork(ReceiverSamplingWork&& other) :

	RenderWork(other),

	m_receiver       (other.m_receiver),
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

SamplingStatistics ReceiverSamplingWork::asyncGetStatistics()
{
	SamplingStatistics statistics;
	statistics.numSamplesTaken = m_numSamplesTaken.load(std::memory_order_relaxed);
	return statistics;
}

void ReceiverSamplingWork::doWork()
{
	PH_ASSERT(m_receiver);

	if(m_onWorkStart)
	{
		m_onWorkStart();
	}

	m_numSamplesTaken.store(0, std::memory_order_relaxed);
	setTotalWork(m_sampleGenerator->numSampleBatches());
	setWorkDone(0);
	setElapsedMs(0);

	const auto rasterSampleHandle = m_sampleGenerator->declareStageND(
		2,
		m_sampleResPx.product(),
		m_sampleResPx.toVector());

	const auto raySampleHandle = m_sampleGenerator->declareStageND(
		5,
		m_sampleResPx.product());

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

		const auto rasterSamples = m_sampleGenerator->getSamplesND(rasterSampleHandle);
		auto raySamples = m_sampleGenerator->getSamplesND(raySampleHandle);
		for(std::size_t si = 0; si < rasterSamples.numSamples(); ++si)
		{
			const auto rasterCoord = m_filmWindowPx.sampleToSurface(math::sample_cast<float64>(rasterSamples.get<2>(si)));
			SampleFlow sampleFlow = raySamples.readSampleAsFlow();

			Ray ray;
			m_receiver->receiveRay(rasterCoord, &ray);

			// FIXME: this loop uses correlated samples, also some processors
			for(ISensedRayProcessor* processor : m_processors)
			{
				processor->process(rasterCoord, ray, sampleFlow);
			}
		}
		m_numSamplesTaken.fetch_add(static_cast<uint32>(rasterSamples.numSamples()), std::memory_order_relaxed);

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

void ReceiverSamplingWork::setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator)
{
	m_sampleGenerator = std::move(sampleGenerator);
}

void ReceiverSamplingWork::setSampleDimensions(
	const math::TVector2<int64>&  filmResPx,
	const math::TAABB2D<float64>& filmWindowPx,
	const math::TVector2<int64>&  sampleResPx)
{
	PH_ASSERT_MSG(filmWindowPx.isValid(), filmWindowPx.toString());

	m_filmResPx    = math::Vector2D(filmResPx);
	m_filmWindowPx = filmWindowPx;
	m_sampleResPx  = math::Vector2S(sampleResPx);
}

void ReceiverSamplingWork::addProcessor(ISensedRayProcessor* const processor)
{
	PH_ASSERT(processor);

	m_processors.push_back(processor);
}

void ReceiverSamplingWork::onWorkStart(std::function<void()> func)
{
	m_onWorkStart = std::move(func);
}

void ReceiverSamplingWork::onWorkReport(std::function<void()> func)
{
	m_onWorkReport = std::move(func);
}

void ReceiverSamplingWork::onWorkFinish(std::function<void()> func)
{
	m_onWorkFinish = std::move(func);
}

ReceiverSamplingWork& ReceiverSamplingWork::operator = (ReceiverSamplingWork&& other)
{
	RenderWork::operator = (std::move(other));

	m_receiver        = other.m_receiver;
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
