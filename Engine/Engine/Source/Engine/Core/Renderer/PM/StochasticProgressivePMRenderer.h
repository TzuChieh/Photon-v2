#pragma once

#include "Engine/Core/Renderer/PM/PMRendererBase.h"

#include <atomic>

namespace ph
{

/*!
Renderer based on the stochastic progressive photon mapping algorithm @cite Hachisuka:2009:Stochastic.
*/
class StochasticProgressivePMRenderer : public PMRendererBase
{
public:
	StochasticProgressivePMRenderer(
		PMCommonParams commonParams,
		Viewport       viewport,
		SampleFilter   filter,
		uint32         numWorkers);

	void doRender() override;
	void retrieveFrame(uint32 layerIndex, HdrRgbFrame& out_frame) override;

	RenderStats asyncQueryRenderStats() override;
	RenderProgress asyncQueryRenderProgress() override;

	void asyncPeekFrame(
		uint32 layerIndex,
		const Region& region,
		HdrRgbFrame& out_frame) override;

	RenderObservableInfo getObservableInfo() const override;

private:
	void renderWithStochasticProgressivePM();

	std::atomic_uint64_t m_photonsPerSecond;
};

}// end namespace ph
