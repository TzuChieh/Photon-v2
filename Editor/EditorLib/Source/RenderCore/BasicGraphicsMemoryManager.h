#pragma once

#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/Memory/HostMemoryBlock.h"

#include <Common/primitive_type.h>
#include <Utility/Concurrent/TAtomicQuasiQueue.h>

#include <cstddef>
#include <memory>
#include <vector>

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
		std::size_t pooledHostBlockSize,
		std::size_t maxPooledHostBlocks);

	GraphicsMemoryBlock* allocHostBlock(uint32 numFramesToLive) override;

	GraphicsMemoryBlock* allocCustomHostBlock(
		uint32 numFramesToLive,
		std::size_t blockSize,
		std::size_t blockAlignment) override;

	void onGHILoad() override;
	void onGHIUnload() override;
	void beginFrameUpdate(const GHIThreadUpdateContext& ctx) override;
	void endFrameUpdate(const GHIThreadUpdateContext& ctx) override;

private:
	struct PooledHostBlock
	{
		HostMemoryBlock block;
		uint32 numFramesLeft = 0;
	};

	struct CustomHostBlock
	{
		std::unique_ptr<HostMemoryBlock> block;
		uint32 numFramesLeft = 0;
	};

	std::unique_ptr<PooledHostBlock[]> m_hostBlocks;
	TAtomicQuasiQueue<PooledHostBlock*> m_freeHostBlocks;
	TAtomicQuasiQueue<PooledHostBlock*> m_activeHostBlocks;
	std::unique_ptr<PooledHostBlock*[]> m_hostBlockCache;
	std::size_t m_maxPooledHostBlocks;
	std::size_t m_pooledHostBlockSize;

	TAtomicQuasiQueue<CustomHostBlock> m_freeCustomHostBlocks;
	TAtomicQuasiQueue<CustomHostBlock> m_activeCustomHostBlocks;
	std::vector<CustomHostBlock> m_customHostBlockCache;
};

}// end namespace ph::editor
