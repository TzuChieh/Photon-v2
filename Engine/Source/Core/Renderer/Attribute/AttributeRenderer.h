#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/Sampling/ReceiverSamplingWork.h"
#include "Core/Renderer/Sampling/TReceiverMeasurementEstimator.h"
#include "Core/Scheduler/WorkScheduler.h"
#include "Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Core/Quantity/Spectrum.h"
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

	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderState asyncQueryRenderState() override;
	RenderProgress asyncQueryRenderProgress() override;
	void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) override;

	ObservableRenderData getObservableData() const override;

private:
	const Scene*     m_scene;
	const Receiver*  m_receiver;
	SampleGenerator* m_sampleGenerator;

	std::string      m_attributeName;
	HdrRgbFilm       m_attributeFilm;
	
	std::mutex       m_rendererMutex;
};

}// end namespace ph
