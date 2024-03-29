#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Utility/IMoveOnly.h"
#include "Core/Renderer/Sampling/SamplingStatistics.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Renderer/Sampling/IReceivedRayProcessor.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/Filmic/SamplingFilmDimensions.h"

#include <atomic>
#include <functional>

namespace ph
{

class Receiver;

class ReceiverSamplingWork : public RenderWork, private IMoveOnly
{
public:
	ReceiverSamplingWork();
	explicit ReceiverSamplingWork(const Receiver* receiver);
	ReceiverSamplingWork(ReceiverSamplingWork&& other) noexcept;

	SamplingStatistics asyncGetStatistics() const;

	void setSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator);

	void setSampleDimensions(
		const math::TVector2<int64>&  rasterActualResPx,
		const math::TAABB2D<float64>& rasterSampleWindowPx,
		const math::TVector2<int64>&  sampleRes);

	void addProcessor(IReceivedRayProcessor* processor);

	void onWorkStart(std::function<void()> func);
	void onWorkReport(std::function<void()> func);
	void onWorkFinish(std::function<void()> func);

	ReceiverSamplingWork& operator = (ReceiverSamplingWork&& other) noexcept;

private:
	void doWork() override;

	const Receiver*                     m_receiver;
	std::vector<IReceivedRayProcessor*> m_processors;
	std::unique_ptr<SampleGenerator>    m_sampleGenerator;
	math::Vector2D                      m_rasterActualResPx;
	math::TAABB2D<float64>              m_rasterSampleWindowPx;
	math::Vector2S                      m_sampleRes;

	std::atomic_uint32_t  m_numSamplesTaken;
	std::function<void()> m_onWorkStart;
	std::function<void()> m_onWorkReport;
	std::function<void()> m_onWorkFinish;
};

}// end namespace ph
