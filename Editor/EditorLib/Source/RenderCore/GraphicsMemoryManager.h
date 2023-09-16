#pragma once

#include <Common/primitive_type.h>

#include <cstddef>
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
	Guaranteed to be pooled.
	@param numFramesToLive The expected lifespan the allocated memory block is guaranteed to be valid.
	Lifetime is in terms of GHI frames.
	E.g., 0 = May be invalidated after the end of current frame;
	      1 = May be invalidated after the end of next frame (current +1 frame), etc.
	@note Thread safe.
	*/
	virtual GraphicsMemoryBlock* allocHostBlock(uint32 numFramesToLive) = 0;

	/*!
	This variant of host block is generally not pooled, and much higher performance hit is expected.
	The block size can be determined by the user, which is useful in situations where large block is
	required or for one-off allocations.
	@param numFramesToLive See `allocHostBlock()`.
	@param blockSize Size of the memory block in bytes.
	@note Thread safe.
	*/
	virtual GraphicsMemoryBlock* allocCustomHostBlock(uint32 numFramesToLive, std::size_t blockSize) = 0;

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

public:
	static uint32 equivalentRenderThreadLifetime(uint32 numRenderThreadFramesToLive)
	{
		/* GHI frame may end before render frame ends but can never restart until next render frame
		begins, so we need to increase the number of frames to live to cover for the case that GHI
		frame may end earlier than render frame. Adding 1 frame will suffice for this case, as that
		means the lifetime won't end until next render frame.
		*/
		return numRenderThreadFramesToLive + 1;
	}
};

}// end namespace ph::editor
