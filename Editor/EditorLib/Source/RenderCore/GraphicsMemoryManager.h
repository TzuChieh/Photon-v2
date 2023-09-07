#pragma once

#include <Common/primitive_type.h>

#include <limits>

namespace ph::editor
{

class GraphicsMemoryBlock;
class GHIThreadUpdateContext;
class GraphicsArena;

/*! @brief Manages memory for graphics.

A lower level graphics abstraction that can act as one of the foundations of other higher level
abstractions (such as `GraphicsObjectManger`).

During the lifetime of the current graphics context, the `GHI` abstraction may be loaded/unloaded
and thread safe methods are allowed to fail gently (not crash or causing any data corruption)
during the unloaded period.
*/
class GraphicsMemoryManager
{
public:
	virtual ~GraphicsMemoryManager();

	/*!
	@param numFramesToLive The expected lifespan the allocated memory block is guaranteed to be valid.
	E.g., 0 = May be invalidated after the end of current frame;
	      1 = May be invalidated after the end of next frame (current +1 frame), etc.
	@note Thread safe.
	*/
	virtual GraphicsMemoryBlock* allocHostBlock(uint32 numFramesToLive) = 0;

	/*! @brief Called by GHI thread after GHI is loaded.
	*/
	virtual void onGHILoad() = 0;

	/*! @brief Called by GHI thread before GHI is unloaded.
	*/
	virtual void onGHIUnload() = 0;

	/*! @brief Called by GHI thread when a frame begins.
	*/
	virtual void beginFrameUpdate(const GHIThreadUpdateContext& ctx) = 0;

	/*! @brief Called by GHI thread when a frame ends.
	*/
	virtual void endFrameUpdate(const GHIThreadUpdateContext& ctx) = 0;

	GraphicsArena getHostArena();
	GraphicsArena getRendererHostArena();
};

}// end namespace ph::editor
