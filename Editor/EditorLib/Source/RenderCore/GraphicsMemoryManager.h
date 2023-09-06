#pragma once

#include <Common/primitive_type.h>

#include <limits>

namespace ph::editor
{

class GraphicsMemoryBlock;
class GHIThreadUpdateContext;
class GraphicsArena;

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
