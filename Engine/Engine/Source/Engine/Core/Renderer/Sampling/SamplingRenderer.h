#pragma once

#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Core/Filmic/SampleFilter.h"
#include "Engine/Core/Estimator/IRayEnergyEstimator.h"

#include <memory>

namespace ph
{

class SamplingRenderer : public Renderer
{
public:
	SamplingRenderer(
		std::unique_ptr<IRayEnergyEstimator> estimator,
		Viewport                             viewport, 
		SampleFilter                         filter,
		uint32                               numWorkers);

	void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) override = 0;
	void doRender() override = 0;
	void retrieveFrame(int32 layerIndex, HdrRgbFrame& out_frame) override = 0;

	std::size_t asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions) override = 0;
	RenderStats asyncQueryRenderStats() override = 0;
	RenderProgress asyncQueryRenderProgress() override = 0;

	void asyncPeekFrame(
		int32         layerIndex, 
		const Region& region,
		HdrRgbFrame&  out_frame) override = 0;

	RenderObservationInfo getObservationInfo() const override = 0;

protected:
	SampleFilter                         m_filter;
	std::unique_ptr<IRayEnergyEstimator> m_estimator;
};

}// end namespace ph
