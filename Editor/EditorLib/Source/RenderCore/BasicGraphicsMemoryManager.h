#pragma once

#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/Memory/HostMemoryBlock.h"

#include <Common/primitive_type.h>
#include <Utility/Concurrent/TAtomicQuasiQueue.h>

#include <cstddef>
#include <memory>

namespace ph::editor
{

class GraphicsMemoryBlock;

/*! @brief Memory manager with some basic functionalities.
*/
class BasicGraphicsMemoryManager : public GraphicsMemoryManager
{
public:
	BasicGraphicsMemoryManager();

	BasicGraphicsMemoryManager(
		std::size_t hostBlockSize,
		std::size_t maxHostBlocks);

	GraphicsMemoryBlock* allocHostBlock(uint32 numFramesToLive) override;
	void beginFrameUpdate(const GHIThreadUpdateContext& ctx) override;
	void endFrameUpdate(const GHIThreadUpdateContext& ctx) override;

private:
	struct HostBlock
	{
		HostMemoryBlock block;
		uint32 numFramesLeft;
	};

	std::unique_ptr<HostBlock[]> m_hostBlocks;
	TAtomicQuasiQueue<HostBlock*> m_freeHostBlocks;
	TAtomicQuasiQueue<HostBlock*> m_activeHostBlocks;
	std::unique_ptr<HostBlock*[]> m_hostBlockCache;
	std::size_t m_maxHostBlocks;
	std::size_t m_hostBlockSize;
};

}// end namespace ph::editor
