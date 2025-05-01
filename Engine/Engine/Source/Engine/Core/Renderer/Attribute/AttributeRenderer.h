#pragma once

#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Core/Filmic/Vector3Film.h"
#include "Engine/Core/Renderer/Sampling/ReceiverSamplingWork.h"
#include "Engine/Core/Renderer/Sampling/TReceiverMeasurementProcessor.h"
#include "Engine/Core/Scheduler/WorkScheduler.h"
#include "Engine/Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Engine/Math/TVector2.h"

#include <mutex>
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
