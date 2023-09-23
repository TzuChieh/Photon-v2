#pragma once

#include "RenderCore/GraphicsMemoryManager.h"

namespace ph::editor::ghi
{

class NullMemoryManager : public GraphicsMemoryManager
{
public:
	GraphicsMemoryBlock* allocHostBlock(uint32 numFramesToLive) override
	{
		return nullptr;
	}

	GraphicsMemoryBlock* allocCustomHostBlock(
		uint32 numFramesToLive,
		std::size_t blockSize,
		std::size_t blockAlignment) override
	{
		return nullptr;
	}

	void onGHILoad() override
	{}

	void onGHIUnload() override
	{}

	void beginFrameUpdate(const GHIThreadUpdateContext& ctx) override
	{}

	void endFrameUpdate(const GHIThreadUpdateContext& ctx) override
	{}
};

}// end namespace ph::editor::ghi
