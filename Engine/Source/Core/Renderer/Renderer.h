#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/Renderer/ERegionStatus.h"
#include "Core/Renderer/Statistics.h"
#include "Frame/frame_fwd.h"
#include "Core/Renderer/AttributeTags.h"
#include "Core/Renderer/RenderStats.h"
#include "Core/Scheduler/Region.h"
#include "Common/assertion.h"
#include "Utility/Timer.h"
#include "Core/Renderer/ObservableRenderData.h"
#include "Frame/Viewport.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <deque>
#include <string>

namespace ph { class CoreCookedUnit; }
namespace ph { class VisualWorld; }

namespace ph
{

class RenderWorker;

class Renderer
{
public:
	Renderer(Viewport viewport, uint32 numWorkers);

	virtual ~Renderer();

	// Perform necessary updates for rendering. 
	// No asynchronous operation is allowed during update.
	virtual void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) = 0;

	// Start rendering.
	virtual void doRender() = 0;

	// Get the rendered result.
	virtual void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) = 0;

	// Get the rendering region that has been updated.
	// TODO: allow polling unioned regions seems like a good idea
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) = 0;
	
	// Returns information regarding the ongoing rendering process.
	virtual RenderStats asyncQueryRenderStats() = 0;

	// TODO: remove this method
	virtual RenderProgress asyncQueryRenderProgress() = 0;

	// Similar to retrieveFrame(2), except that correctness is not guaranteed 
	// for the returned frame.
	virtual void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) = 0;

	// Get information about available outputs of the renderer, which will be
	// determined after each update. The actual data and can be retrieved via
	// async<X>() methods.
	virtual ObservableRenderData getObservableData() const = 0;

	void update(const CoreCookedUnit& cooked, const VisualWorld& world);
	void render();

	/*! @brief Set number of workers for the renderer.
	*/
	void setNumWorkers(uint32 numWorkers);

	uint32 numWorkers() const;
	uint32 getRenderWidthPx() const;
	uint32 getRenderHeightPx() const;
	math::TAABB2D<int64> getCropWindowPx() const;

	/*! @brief The frame region that is going to be rendered.
	*/
	math::TAABB2D<int64> getRenderRegionPx() const;

	/*! @brief Descriptions regarding dimensions for the rendered frame.
	*/
	const Viewport& getViewport() const;

	bool asyncIsUpdating() const;
	bool asyncIsRendering() const;

private:
	Viewport m_viewport;
	uint32   m_numWorkers;

	std::vector<RenderWorker> m_workers;

	std::atomic_bool m_isUpdating;
	std::atomic_bool m_isRendering;
};

// In-header Implementations:

inline uint32 Renderer::numWorkers() const
{
	return m_numWorkers;
}

inline uint32 Renderer::getRenderWidthPx() const
{
	return m_viewport.getBaseSizePx().x();
}

inline uint32 Renderer::getRenderHeightPx() const
{
	return m_viewport.getBaseSizePx().y();
}

inline math::TAABB2D<int64> Renderer::getCropWindowPx() const
{
	return m_viewport.getWindowPx();
}

inline math::TAABB2D<int64> Renderer::getRenderRegionPx() const
{
	return m_viewport.getCroppedRegionPx();
}

inline const Viewport& Renderer::getViewport() const
{
	return m_viewport;
}

inline bool Renderer::asyncIsUpdating() const
{
	return m_isUpdating.load(std::memory_order_relaxed);
}

inline bool Renderer::asyncIsRendering() const
{
	return m_isRendering.load(std::memory_order_relaxed);
}

}// end namespace ph
