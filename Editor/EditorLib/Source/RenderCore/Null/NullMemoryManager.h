#pragma once

#include "RenderCore/GraphicsMemoryManager.h"

namespace ph::editor
{

class NullMemoryManager : public GraphicsMemoryManager
{
public:
	inline GraphicsMemoryBlock* allocHostBlock(uint32 numFramesToLive) override
	{
		return nullptr;
	}

	inline void beginFrameUpdate(const GHIThreadUpdateContext& ctx) override
	{}

	inline void endFrameUpdate(const GHIThreadUpdateContext& ctx) override
	{}
};

}// end namespace ph::editor
