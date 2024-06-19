#include "Core/Renderer/Attribute/AttributeRenderer.h"
#include "World/VisualWorld.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "World/VisualWorld.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Filmic/Vector3Film.h"
#include "Core/Scheduler/PlateScheduler.h"
#include "Core/Scheduler/StripeScheduler.h"
#include "Core/Scheduler/GridScheduler.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"
#include "Utility/utility.h"
#include "Core/Scheduler/SpiralGridScheduler.h"
#include "Core/Scheduler/TileScheduler.h"
#include "Core/Scheduler/WorkUnit.h"
#include "Core/Estimator/SurfaceAttributeEstimator.h"
#include "Math/Random/sample.h"
#include "Core/Receiver/Receiver.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(AttributeRenderer, Renderer);

void AttributeRenderer::doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	PH_LOG(AttributeRenderer, Note, "target attribute: {}", m_attributeName);

	m_scene           = world.getScene();
	m_receiver        = cooked.getReceiver();
	m_sampleGenerator = cooked.getSampleGenerator();

	PH_ASSERT(m_scene);
	PH_ASSERT(m_receiver);
	PH_ASSERT(m_sampleGenerator);

	m_attributeFilm = Vector3Film(
		getRenderWidthPx(),
		getRenderHeightPx(),
		getRenderRegionPx(),
		SampleFilter::makeBox());
}

void AttributeRenderer::doRender()
{
	// TODO: threading

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
				std::lock_guard<std::mutex> lock(m_rendererMutex);

				m_attributeFilm.addSample(rasterCoord.x(), rasterCoord.y(), estimation[0]);
			}
		}
	}
}

std::size_t AttributeRenderer::asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions)
{
	if(asyncIsRendering())
	{
		out_regions[0] = RenderRegionStatus(getRenderRegionPx(), ERegionStatus::Updating);
		return 1;
	}
	else
	{
		// Report region finished when not rendering, so we meet the required ordering
		out_regions[0] = RenderRegionStatus(getRenderRegionPx(), ERegionStatus::Finished);
		return 1;
	}
}

// OPT: Peeking does not need to ensure correctness of the frame.
// If correctness is not guaranteed, develop methods should be reimplemented. 
// (correctness is guaranteed currently)
void AttributeRenderer::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(layerIndex == 0)
	{
		m_attributeFilm.develop(out_frame, region);
	}
	else
	{
		out_frame.fill(0, math::TAABB2D<uint32>(region));
	}
}

void AttributeRenderer::retrieveFrame(const std::size_t layerIndex, HdrRgbFrame& out_frame)
{
	asyncPeekFrame(layerIndex, getRenderRegionPx(), out_frame);
}

RenderStats AttributeRenderer::asyncQueryRenderStats()
{
	return RenderStats{};
}

RenderProgress AttributeRenderer::asyncQueryRenderProgress()
{
	return RenderProgress{};
}

RenderObservationInfo AttributeRenderer::getObservationInfo() const
{
	return RenderObservationInfo{};
}

}// end namespace ph
