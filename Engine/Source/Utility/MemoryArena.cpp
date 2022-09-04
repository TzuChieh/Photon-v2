#include "Utility/MemoryArena.h"
#include "Common/config.h"
#include "Common/assertion.h"
#include "Common/os.h"

#include <new>

namespace ph
{

namespace
{

inline std::size_t determine_block_size(const std::size_t blockSizeInBytes)
{
	return blockSizeInBytes > PH_MEMORY_ARENA_DEFAULT_BLOCK_SIZE_IN_BYTES
		? blockSizeInBytes
		: PH_MEMORY_ARENA_DEFAULT_BLOCK_SIZE_IN_BYTES;
}

inline auto allocate_block(const std::size_t blockSizeInBytes)
	-> TAlignedMemoryUniquePtr<std::byte>
{
	constexpr auto MAX_ALIGN_SIZE = alignof(std::max_align_t);
	const     auto cacheSize      = os::get_L1_cache_line_size_in_bytes();

	PH_ASSERT_MSG(cacheSize >= MAX_ALIGN_SIZE && cacheSize % MAX_ALIGN_SIZE == 0,
		"Current allocation scheme will result in degraded performance due to cache miss if the above "
		"condition is not met.");

	TAlignedMemoryUniquePtr<std::byte> memory = make_aligned_memory<std::byte>(blockSizeInBytes, cacheSize);
	if(!memory)
	{
		throw std::bad_alloc();
	}
	return memory;
}

}// end anonymous namespace

MemoryArena::MemoryArena()
	: MemoryArena(determine_block_size(0), 0)
{}

MemoryArena::MemoryArena(const std::size_t blockSizeInBytes, const std::size_t numDefaultBlocks)
	: m_blocks               (numDefaultBlocks)
	, m_blockSizeInBytes     (determine_block_size(blockSizeInBytes))
	, m_currentBlockIdx      (0)
	, m_blockPtr             (nullptr)
	, m_remainingBytesInBlock(0)
	, m_numUsedBytes         (0)
{
	PH_ASSERT_EQ(m_blocks.size(), numDefaultBlocks);
	for(TAlignedMemoryUniquePtr<std::byte>& block : m_blocks)
	{
		block = allocate_block(m_blockSizeInBytes);
	}

	if(!m_blocks.empty())
	{
		m_blockPtr = m_blocks[0].get();
		m_remainingBytesInBlock = m_blockSizeInBytes;
	}
}

std::byte* MemoryArena::allocRaw(const std::size_t numBytes, const std::size_t alignmentInBytes)
{
	// It is impossible to allocate memory larger than the fixed block size
	if(numBytes > m_blockSizeInBytes) [[unlikely]]
	{
		throw std::bad_alloc();
	}

	// NOTE: A better strategy would be keeping a record of remaining space in each block, then pick the
	// most suitable one to allocate. Here we just use the next block to allocate, trading space for speed. 

	// Use a new block if there is not enough space left
	if(m_remainingBytesInBlock < numBytes)
	{
		// Allocate a new one if there are no blocks left
		if(m_currentBlockIdx + 1 == m_blocks.size())
		{
			m_blocks.push_back(allocate_block(m_blockSizeInBytes));
		}

		PH_ASSERT_LT(m_currentBlockIdx + 1, m_blocks.size());
		m_blockPtr = m_blocks[++m_currentBlockIdx].get();
		m_remainingBytesInBlock = m_blockSizeInBytes;
	}

	PH_ASSERT(m_blockPtr);
	PH_ASSERT_GE(m_remainingBytesInBlock, numBytes);

	void* newBlockPtr = m_blockPtr;
	void* const alignedPtr = std::align(alignmentInBytes, numBytes, newBlockPtr, m_remainingBytesInBlock);
	
	// It is possible that the alignment requirement is impossible to met due to insufficient bytes
	// remaining in the block
	if(!alignedPtr) [[unlikely]]
	{
		throw std::bad_alloc();
	}

	// `std::align()` only adjusts `newBlockPtr` to the aligned memory location
	m_blockPtr = static_cast<std::byte*>(newBlockPtr) + numBytes;

	// `std::align()` only decreases `m_remainingBytesInBlock` by the number of bytes used for alignment
	m_remainingBytesInBlock -= numBytes;

	return static_cast<std::byte*>(alignedPtr);
}

}// end namespace ph
