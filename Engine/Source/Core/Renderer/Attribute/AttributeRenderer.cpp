#include "Core/Renderer/Attribute/AttributeRenderer.h"
#include "Common/primitive_type.h"
#include "World/VisualWorld.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/CoreDataGroup.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Common/assertion.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/Vector3Film.h"
#include "Core/Renderer/Region/PlateScheduler.h"
#include "Core/Renderer/Region/StripeScheduler.h"
#include "Core/Renderer/Region/GridScheduler.h"
#include "Utility/FixedSizeThreadPool.h"
#include "Utility/utility.h"
#include "Core/Renderer/Region/SpiralGridScheduler.h"
#include "Core/Renderer/Region/TileScheduler.h"
#include "Common/Logger.h"
#include "Core/Renderer/Region/WorkUnit.h"
#include "Core/Estimator/SurfaceAttributeEstimator.h"
#include "Math/Random/sample.h"
#include "Core/Receiver/Receiver.h"
#include "DataIO/SDL/InputPacket.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <utility>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Attribute Renderer"));
}

void AttributeRenderer::doUpdate(const CoreDataGroup& data)
{
	logger.log("target attribute: " + m_attributeName);

	m_scene           = data.getScene();
	m_receiver        = data.getReceiver();
	m_sampleGenerator = data.getSampleGenerator();

	PH_ASSERT(m_scene);
	PH_ASSERT(m_receiver);
	PH_ASSERT(m_sampleGenerator);

	m_attributeFilm = HdrRgbFilm(
		getRenderWidthPx(),
		getRenderHeightPx(),
		getCropWindowPx(),
		SampleFilters::createBoxFilter());
}

void AttributeRenderer::doRender()
{
	const Integrand integrand(m_scene, m_receiver);

	SurfaceAttributeEstimator estimator;
	estimator.update(integrand);

	TEstimationArray<math::Vector3R> estimation(1);

	const auto rasterSampleHandle = m_sampleGenerator->declareStageND(
		2,
		math::Vector2S(m_attributeFilm.getSampleResPx()).product(),
		math::Vector2S(m_attributeFilm.getSampleResPx()).toVector());

	const auto raySampleHandle = m_sampleGenerator->declareStageND(
		2,
		math::Vector2S(m_attributeFilm.getSampleResPx()).product(),
		math::Vector2S(m_attributeFilm.getSampleResPx()).toVector());

	const auto sampleWindow = m_attributeFilm.getSampleWindowPx();

	while(m_sampleGenerator->prepareSampleBatch())
	{
		const auto rasterSamples = m_sampleGenerator->getSamplesND(rasterSampleHandle);
		auto raySamples = m_sampleGenerator->getSamplesND(raySampleHandle);
		for(std::size_t si = 0; si < rasterSamples.numSamples(); ++si)
		{
			const auto rasterCoord = sampleWindow.sampleToSurface(math::sample_cast<float64>(rasterSamples.get<2>(si)));
			SampleFlow sampleFlow = raySamples.readSampleAsFlow();

			Ray ray;
			m_receiver->receiveRay(rasterCoord, &ray);

			estimator.estimate(ray, integrand, sampleFlow, estimation);

			{
				//std::lock_guard<std::mutex> lock(m_rendererMutex);

				m_attributeFilm.addSample(rasterCoord.x, rasterCoord.y, estimation[0].clamp(0, std::numeric_limits<real>::max()));
			}
		}
	}
}

ERegionStatus AttributeRenderer::asyncPollUpdatedRegion(Region* const out_region)
{
	PH_ASSERT(out_region);

	//std::lock_guard<std::mutex> lock(m_rendererMutex);

	*out_region = getCropWindowPx();
	return ERegionStatus::UPDATING;
}

// OPT: Peeking does not need to ensure correctness of the frame.
// If correctness is not guaranteed, develop methods should be reimplemented. 
// (correctness is guaranteed currently)
void AttributeRenderer::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame)
{
	//std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(layerIndex == 0)
	{
		m_attributeFilm.develop(out_frame, region);
	}
	else
	{
		out_frame.fill(0, TAABB2D<uint32>(region));
	}
}

void AttributeRenderer::retrieveFrame(const std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	asyncPeekFrame(layerIndex, getCropWindowPx(), out_frame);
}

RenderState AttributeRenderer::asyncQueryRenderState()
{
	return RenderState();
}

RenderProgress AttributeRenderer::asyncQueryRenderProgress()
{
	return RenderProgress();
}

ObservableRenderData AttributeRenderer::getObservableData() const
{
	return ObservableRenderData();
}

// command interface

AttributeRenderer::AttributeRenderer(const InputPacket& packet) :

	Renderer(packet),

	m_scene          (nullptr),
	m_receiver       (nullptr),
	m_sampleGenerator(nullptr),
	m_attributeFilm  (),

	m_attributeName(),

	m_rendererMutex()
{
	m_attributeName = packet.getString("attribute", m_attributeName, DataTreatment::REQUIRED());
}

SdlTypeInfo AttributeRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "attribute");
}

void AttributeRenderer::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<AttributeRenderer>(packet);
	}));
}

}// end namespace ph
