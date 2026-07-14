#pragma once

#include "Engine/Core/Renderer/PM/PMRendererBase.h"

#include <atomic>

namespace ph
{

class VanillaPMRenderer : public PMRendererBase
{
public:
	VanillaPMRenderer(
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
	void renderWithVanillaPM();

	std::atomic_uint64_t m_photonsPerSecond;
};

}// end namespace ph
