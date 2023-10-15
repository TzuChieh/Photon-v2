#include "RenderCore/BasicGraphicsMemoryManager.h"
#include "RenderCore/GHIThreadUpdateContext.h"
#include "RenderCore/ghi_exceptions.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/profiling.h>
#include <Math/constant.h>

#include <utility>
#include <new>
#include <algorithm>
#include <iterator>

namespace ph::editor::ghi
{

BasicGraphicsMemoryManager::BasicGraphicsMemoryManager()
	: BasicGraphicsMemoryManager(math::constant::MiB * 16, 2048)// 32 GiB in total
{}

BasicGraphicsMemoryManager::BasicGraphicsMemoryManager(
	std::size_t pooledHostBlockSize,
	std::size_t maxPooledHostBlocks)

	: GraphicsMemoryManager()

	, m_hostBlocks()
	, m_freeHostBlocks()
	, m_activeHostBlocks()
	, m_hostBlockCache()
	, m_numUsedHostBlocks(0)
	, m_maxPooledHostBlocks(maxPooledHostBlocks)
	, m_pooledHostBlockSize(pooledHostBlockSize)

	, m_freeCustomHostBlocks()
	, m_activeCustomHostBlocks()
	, m_customHostBlockCache(16)
{
	constexpr std::size_t minBlocks = 8;
	m_maxPooledHostBlocks = m_maxPooledHostBlocks < minBlocks ? minBlocks : m_maxPooledHostBlocks;

	m_hostBlocks = std::make_unique<PooledHostBlock[]>(m_maxPooledHostBlocks);
	m_hostBlockCache = std::make_unique<PooledHostBlock*[]>(m_maxPooledHostBlocks);
}

GraphicsMemoryBlock* BasicGraphicsMemoryManager::allocHostBlock(uint32 numFramesToLive)
{
	PH_PROFILE_SCOPE();

	// Note: this method can be called concurrently

	// Try to use recycled block, otherwise make a new block
	PooledHostBlock* hostBlock = nullptr;
	if(!m_freeHostBlocks.tryDequeue(&hostBlock))
	{
		const auto newBlockIdx = m_numUsedHostBlocks.fetch_add(1, std::memory_order_relaxed);
		if(newBlockIdx < m_maxPooledHostBlocks)
		{
			hostBlock = &(m_hostBlocks[newBlockIdx]);
			PH_ASSERT(!hostBlock->block.hasBlockSource());
			hostBlock->block = HostMemoryBlock(m_pooledHostBlockSize);
		}
		else
		{
			// We can safely log here--we are not strictly out of memory, just the amount reserved for
			// GHI is not enough:
			PH_DEFAULT_LOG_ERROR(
				"Out of pooled host memory block (max={} blocks, {} MiB each)",
				m_maxPooledHostBlocks, m_pooledHostBlockSize / math::constant::MiB);

			throw OutOfHostMemory{};
		}
	}

	PH_ASSERT(hostBlock);
	PH_ASSERT(hostBlock->block.hasBlockSource());

	hostBlock->numFramesLeft = numFramesToLive;
	m_activeHostBlocks.enqueue(hostBlock);
	return &hostBlock->block;
}

GraphicsMemoryBlock* BasicGraphicsMemoryManager::allocCustomHostBlock(
	uint32 numFramesToLive,
	std::size_t blockSize,
	std::size_t blockAlignment)
{
	PH_PROFILE_SCOPE();

	// Note: this method can be called concurrently

	CustomHostBlock hostBlock;
	if(!m_freeCustomHostBlocks.tryDequeue(&hostBlock))
	{
		hostBlock.block = std::make_unique<HostMemoryBlock>();
	}

	HostMemoryBlock* block = hostBlock.block.get();
	PH_ASSERT(block);
	PH_ASSERT(!block->hasBlockSource());

	*block = HostMemoryBlock(blockSize, blockAlignment);
	hostBlock.numFramesLeft = numFramesToLive;

	m_activeCustomHostBlocks.enqueue(std::move(hostBlock));
	return block;
}

void BasicGraphicsMemoryManager::onGHILoad()
{}

void BasicGraphicsMemoryManager::onGHIUnload()
{}

void BasicGraphicsMemoryManager::beginFrameUpdate(const GHIThreadUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();
}

void BasicGraphicsMemoryManager::endFrameUpdate(const GHIThreadUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	// Update pooled blocks
	{
		// A failed dequeue is fine--just those blocks get lucky and get to live an extra frame
		auto numDequeued = m_activeHostBlocks.tryDequeueBulk(m_hostBlockCache.get(), m_maxPooledHostBlocks);

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

	// Update custom blocks
	{
		PH_ASSERT_NE(m_customHostBlockCache.size(), 0);

		// A failed dequeue is fine--just those blocks get lucky and get to live an extra frame
		auto numDequeued = m_activeCustomHostBlocks.tryDequeueBulk(
			m_customHostBlockCache.begin(),
			m_customHostBlockCache.size());

		// Effective growth rate k = 1.5
		if(numDequeued == m_customHostBlockCache.size())
		{
			m_customHostBlockCache.resize(numDequeued + numDequeued / 2);
		}

		std::size_t numStillActive = 0;
		for(std::size_t bi = 0; bi < numDequeued; ++bi)
		{
			if(m_customHostBlockCache[bi].numFramesLeft == 0)
			{
				// Free underlying memory by moving in an empty block
				*(m_customHostBlockCache[bi].block) = HostMemoryBlock{};

				// Enqueue free blocks back one by one, so allocators can obtain a recycled one sooner
				m_freeCustomHostBlocks.enqueue(std::move(m_customHostBlockCache[bi]));
			}
			else
			{
				--(m_customHostBlockCache[bi].numFramesLeft);
			
				// Move active blocks forward, so we can bulk enqueue them back later
				m_customHostBlockCache[numStillActive] = std::move(m_customHostBlockCache[bi]);
				++numStillActive;
			}
		}

		m_activeCustomHostBlocks.enqueueBulk(
			std::make_move_iterator(m_customHostBlockCache.begin()),
			numStillActive);
	}
}

}// end namespace ph::editor::ghi
