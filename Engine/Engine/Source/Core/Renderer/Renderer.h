#pragma once

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
#include "Core/Renderer/RenderRegionStatus.h"
#include "Core/Renderer/RenderObservationInfo.h"
#include "Frame/Viewport.h"
#include "Utility/Timer.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
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

	/*! @brief Perform necessary updates for rendering. 
	No asynchronous operation is allowed during update.
	*/
	virtual void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) = 0;

	/*! @brief Perform rendering.
	*/
	virtual void doRender() = 0;

	/*! @brief Get the rendered result.
	*/
	virtual void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) = 0;

	/*! @brief Get the rendering regions that have been updated.
	Status of a region will always transition to `ERegionStatus::Finished`, and this ordering guarantee
	is valid for one or across multiple calls. Note that if the renderer has multiple workers working
	on the same region, you may observe a region transition from `ERegionStatus::Finished` to other
	status multiple times, but in the end it will eventually be `ERegionStatus::Finished`.
	@param out_regions Output buffer for storing the updated regions.
	@return Number of regions actually stored in the output buffer.
	*/
	virtual std::size_t asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions) = 0;
	
	/*! @brief Get general information of the ongoing rendering process.
	More information can be provided by the implementation. The meaning of each stat can be obtained
	via `getObservationInfo()`.
	*/
	virtual RenderStats asyncQueryRenderStats() = 0;

	/*! @brief Get progress of the ongoing rendering process.
	Implementation is advised to provide this information as it is vital for the caller to be able to
	observe the progress of a potentially long-running rendering process.
	*/
	virtual RenderProgress asyncQueryRenderProgress() = 0;

	/*! @brief Get the intermediate render result.
	This method is similar to `retrieveFrame()`, except that correctness is not guaranteed for the
	returned frame.
	*/
	virtual void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) = 0;

	/*! @brief Get information about available transient outputs of an ongoing render operation.
	This information will be determined after each update (constant throughout the following rendering 
	process). The actual data and can be retrieved via async<X>() methods.
	*/
	virtual RenderObservationInfo getObservationInfo() const = 0;

	/*! @brief Start updating.
	*/
	void update(const CoreCookedUnit& cooked, const VisualWorld& world);

	/*! @brief Start rendering.
	*/
	void render();

	/*! @brief Set number of workers for the renderer.
	*/
	void setNumWorkers(uint32 numWorkers);

	uint32 numWorkers() const;
	uint32 getRenderWidthPx() const;
	uint32 getRenderHeightPx() const;

	/*! @brief The region to work on.
	The user may specify a window to confine all operations to this region. Note that this
	is not necessary the region that will be rendered (see `getRenderRegionPx()`).
	*/
	math::TAABB2D<int64> getCropWindowPx() const;

	/*! @brief The region that is going to be rendered.
	The difference between render region and crop window is that render region will never exceed the
	area defined by the base render size ([0, 0] to [`getRenderWidthPx()`, `getRenderHeightPx()`]).
	*/
	math::TAABB2D<int64> getRenderRegionPx() const;

	/*! @brief Descriptions regarding dimensions for the rendered frame.
	Viewport is set on ctor and stayed constant during the lifetime of the renderer.
	*/
	const Viewport& getViewport() const;

	bool asyncIsUpdating() const;
	bool asyncIsRendering() const;

	/*! @brief Get the rendering region that has been updated.
	This variant polls for combined regions. Note that this variant does not guarantee any status
	ordering for a region. If a more fine-grained result is desired, use `asyncPollUpdatedRegions()`.
	@param out_regions Output buffer for storing the updated regions. The size of the buffer also
	determines the maximum number of regions that will participate in the merging process.
	@param mergeSize Number of regions to merge to a single unit. To obtain a single region, you can
	specify a number greater or equal to the size of the output buffer.
	*/
	std::size_t asyncPollMergedUpdatedRegions(
		TSpan<RenderRegionStatus> out_regions,
		std::size_t mergeSize);

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
