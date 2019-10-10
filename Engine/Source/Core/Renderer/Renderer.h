#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/Renderer/ERegionStatus.h"
#include "Core/Renderer/Statistics.h"
#include "Frame/frame_fwd.h"
#include "Core/Renderer/AttributeTags.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Renderer/RenderState.h"
#include "Core/Renderer/Region/Region.h"
#include "Common/assertion.h"
#include "Utility/Timer.h"
#include "Core/Renderer/ObservableRenderData.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <deque>
#include <string>

namespace ph
{

class SdlResourcePack;
class InputPacket;
class RenderWorker;

class Renderer: public TCommandInterface<Renderer>
{
public:
	template<typename T>
	using TAABB2D = math::TAABB2D<T>;

	virtual ~Renderer();

	// Perform necessary updates for rendering. 
	// No asynchronous operation is allowed during update.
	virtual void doUpdate(const SdlResourcePack& data) = 0;

	// Start rendering.
	virtual void doRender() = 0;

	// Get the rendered result.
	virtual void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) = 0;

	// Get the rendering region that has been updated.
	// TODO: allow polling unioned regions seems like a good idea
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) = 0;
	
	// Returns information regarding the rendering process.
	virtual RenderState asyncQueryRenderState() = 0;

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

	void update(const SdlResourcePack& data);
	void render();
	void setNumWorkers(uint32 numWorkers);

	uint32         numWorkers()        const;
	uint32         getRenderWidthPx()  const;
	uint32         getRenderHeightPx() const;
	TAABB2D<int64> getCropWindowPx()   const;

	bool asyncIsUpdating() const;
	bool asyncIsRendering() const;

private:
	uint32         m_numWorkers;
	uint32         m_widthPx;
	uint32         m_heightPx;
	TAABB2D<int64> m_cropWindowPx;

	std::vector<RenderWorker> m_workers;

	std::atomic_bool m_isUpdating;
	std::atomic_bool m_isRendering;


// command interface
public:
	explicit Renderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline uint32 Renderer::numWorkers() const
{
	return m_numWorkers;
}

inline uint32 Renderer::getRenderWidthPx() const
{
	return m_widthPx;
}

inline uint32 Renderer::getRenderHeightPx() const
{
	return m_heightPx;
}

inline auto Renderer::getCropWindowPx() const
	-> TAABB2D<int64>
{
	return m_cropWindowPx;
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

/*
	<SDL_interface>

	<category>  renderer </category>
	<type_name> renderer </type_name>

	<name> Renderer </name>
	<description>
		The main engine component for producing images.
	</description>

	<command type="creator" intent="blueprint">
		<input name="width" type="integer">
			<description>Width of the film in pixels.</description>
		</input>
		<input name="height" type="integer">
			<description>Height of the film in pixels.</description>
		</input>
		<input name="rect-x" type="integer">
			<description>X coordinate of the lower-left corner of the film cropping window.</description>
		</input>
		<input name="rect-y" type="integer">
			<description>Y coordinate of the lower-left corner of the film cropping window.</description>
		</input>
		<input name="rect-w" type="integer">
			<description>Width of the film cropping window.</description>
		</input>
		<input name="rect-h" type="integer">
			<description>Height of the film cropping window.</description>
		</input>
	</command>

	</SDL_interface>
*/
