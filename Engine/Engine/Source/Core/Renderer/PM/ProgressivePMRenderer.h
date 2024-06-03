#pragma once

#include "Core/Renderer/PM/PMRendererBase.h"

#include <atomic>

namespace ph
{

/*!
Renderer based on the progressive photon mapping algorithm @cite Hachisuka:2008:Progressive.
*/
class ProgressivePMRenderer : public PMRendererBase
{
public:
	ProgressivePMRenderer(
		PMCommonParams commonParams,
		Viewport       viewport,
		SampleFilter   filter,
		uint32         numWorkers);

	void doRender() override;
	void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) override;

	RenderStats asyncQueryRenderStats() override;
	RenderProgress asyncQueryRenderProgress() override;

	void asyncPeekFrame(
		std::size_t layerIndex,
		const Region& region,
		HdrRgbFrame& out_frame) override;

	RenderObservationInfo getObservationInfo() const;

private:
	void renderWithProgressivePM();

	std::atomic_uint64_t m_photonsPerSecond;
};

}// end namespace ph
