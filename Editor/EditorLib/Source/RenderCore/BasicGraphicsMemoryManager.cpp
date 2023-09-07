#include "RenderCore/BasicGraphicsMemoryManager.h"
#include "RenderCore/GHIThreadUpdateContext.h"
#include "RenderCore/ghi_exceptions.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Math/constant.h>

#include <utility>
#include <new>
#include <algorithm>

namespace ph::editor
{

BasicGraphicsMemoryManager::BasicGraphicsMemoryManager()
	: BasicGraphicsMemoryManager(math::constant::MiB * 16, 128)// 2 GiB in total
{}

BasicGraphicsMemoryManager::BasicGraphicsMemoryManager(
	std::size_t hostBlockSize,
	std::size_t maxHostBlocks)

	: GraphicsMemoryManager()

	, m_hostBlocks()
	, m_freeHostBlocks()
	, m_activeHostBlocks()
	, m_hostBlockCache()
	, m_maxHostBlocks(maxHostBlocks)
	, m_hostBlockSize(hostBlockSize)
{
	constexpr std::size_t minBlocks = 8;
	m_maxHostBlocks = m_maxHostBlocks < minBlocks ? minBlocks : m_maxHostBlocks;

	m_hostBlocks = std::make_unique<HostBlock[]>(m_maxHostBlocks);
	for(std::size_t bi = 0; bi < m_maxHostBlocks; ++bi)
	{
		m_freeHostBlocks.enqueue(&m_hostBlocks[bi]);
	}
	
	m_hostBlockCache = std::make_unique<HostBlock*[]>(m_maxHostBlocks);
}

GraphicsMemoryBlock* BasicGraphicsMemoryManager::allocHostBlock(uint32 numFramesToLive)
{
	// Note: this method can be called concurrently

	HostBlock* hostBlock;
	if(m_freeHostBlocks.tryDequeue(&hostBlock))
	{
		PH_ASSERT(hostBlock);

		if(!hostBlock->block.hasBlockSource())
		{
			hostBlock->block = HostMemoryBlock(m_hostBlockSize);
			hostBlock->numFramesLeft = numFramesToLive;
		}

		m_activeHostBlocks.enqueue(hostBlock);
		return &hostBlock->block;
	}
	else
	{
		// We can safely log here--we are not strictly out of memory, just the amount reserved for
		// GHI is not enough:
		PH_DEFAULT_LOG_ERROR(
			"Out of host memory block (max={} blocks, {} MiB each)",
			m_maxHostBlocks, m_hostBlockSize / math::constant::MiB);

		throw GHIOutOfHostMemory{};
	}
}

void BasicGraphicsMemoryManager::onGHILoad()
{
	// TODO
}

void BasicGraphicsMemoryManager::onGHIUnload()
{
	// TODO
}

void BasicGraphicsMemoryManager::beginFrameUpdate(const GHIThreadUpdateContext& ctx)
{}

void BasicGraphicsMemoryManager::endFrameUpdate(const GHIThreadUpdateContext& ctx)
{
	// A failed dequeue is fine--just those blocks get lucky and get to live an extra frame
	//auto numDequeued = m_activeHostBlocks.tryDequeueBulk(m_hostBlockCache.get(), m_maxHostBlocks);
	std::size_t numDequeued = 0;

	std::size_t numStillActive = 0;
	for(std::size_t bi = 0; bi < numDequeued; ++bi)
	{
		if(m_hostBlockCache[bi]->numFramesLeft == 0)
		{
			m_hostBlockCache[bi]->block.clear();

			// Enqueue free blocks back one by one, so allocators can obtain a recycled one sooner
			m_freeHostBlocks.enqueue(m_hostBlockCache[bi]);
		}
		else
		{
			--(m_hostBlockCache[bi]->numFramesLeft);
			
			// Move active blocks forward, so we can bulk enqueue them back later
			m_hostBlockCache[numStillActive] = m_hostBlockCache[bi];
			++numStillActive;
		}
	}

	m_activeHostBlocks.enqueueBulk(m_hostBlockCache.get(), numStillActive);
}

}// end namespace ph::editor
