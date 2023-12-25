#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/Vector3Film.h"
#include "Core/Renderer/Sampling/ReceiverSamplingWork.h"
#include "Core/Renderer/Sampling/TReceiverMeasurementEstimator.h"
#include "Core/Scheduler/WorkScheduler.h"
#include "Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Math/TVector2.h"

#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <string>

namespace ph
{

class Scene;
class Receiver;
class SampleGenerator;

class AttributeRenderer : public Renderer
{
public:
	void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) override;
	void doRender() override;
	void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) override;

	std::size_t asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions) override;
	RenderStats asyncQueryRenderStats() override;
	RenderProgress asyncQueryRenderProgress() override;

	void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) override;

	RenderObservationInfo getObservationInfo() const override;

private:
	const Scene*     m_scene;
	const Receiver*  m_receiver;
	SampleGenerator* m_sampleGenerator;

	std::string      m_attributeName;
	Vector3Film      m_attributeFilm;
	
	std::mutex       m_rendererMutex;
};

}// end namespace ph
