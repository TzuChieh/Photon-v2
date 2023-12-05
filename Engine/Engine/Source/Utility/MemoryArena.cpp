#include "Utility/MemoryArena.h"
#include "Common/math_basics.h"

#include <Common/assertion.h>
#include <Common/config.h>
#include <Common/os.h>

#include <new>
#include <bit>

namespace ph
{

namespace
{

inline std::size_t determine_block_size(const std::size_t blockSizeHintInBytes)
{
	return math::next_power_of_2_multiple(
		blockSizeHintInBytes,
		os::get_L1_cache_line_size_in_bytes());
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
	: MemoryArena(PH_MEMORY_ARENA_DEFAULT_BLOCK_SIZE_IN_BYTES, 0)
{}

MemoryArena::MemoryArena(const std::size_t blockSizeHintInBytes, const std::size_t numDefaultBlocks)
	: m_blocks               (numDefaultBlocks)
	, m_blockSizeInBytes     (determine_block_size(blockSizeHintInBytes))
	, m_currentBlockIdx      (static_cast<std::size_t>(-1))
	, m_blockPtr             (nullptr)
	, m_remainingBytesInBlock(0)
	, m_numUsedBytes         (0)
	, m_dtorCallers          ()
{
	PH_ASSERT_EQ(m_blocks.size(), numDefaultBlocks);
	for(TAlignedMemoryUniquePtr<std::byte>& block : m_blocks)
	{
		block = allocate_block(m_blockSizeInBytes);
	}

	if(!m_blocks.empty())
	{
		m_currentBlockIdx = 0;
		m_blockPtr = m_blocks[0].get();
		m_remainingBytesInBlock = m_blockSizeInBytes;
	}
}

MemoryArena::~MemoryArena()
{
	// Need to ensure `clear()` is called in case the user did not call it. Things like calling
	// non-trivial destructors are only done here.
	clear();
}

std::byte* MemoryArena::allocRaw(const std::size_t numBytes, const std::size_t alignmentInBytes)
{
	// Relying on the fact that unsigned integral wraps around, so that we do not need to check if this
	// is the first use of an empty arena.
	//
	// This is standard behavior, just to be safe:
	static_assert(static_cast<std::size_t>(-1) + 1 == 0);

	PH_ASSERT(math::is_power_of_2(alignmentInBytes));

	// It is impossible to allocate memory larger than the fixed block size
	if(numBytes > m_blockSizeInBytes)
	{
		throw std::bad_alloc();
	}

	// NOTE: A better strategy would be keeping a record of remaining space in each block, then pick the
	// most suitable one to allocate. Here we just use the next block to allocate, trading space for speed. 

	void* blockPtr = m_blockPtr;
	std::size_t bytesInBlock = m_remainingBytesInBlock;
	void* alignedPtr = std::align(alignmentInBytes, numBytes, blockPtr, bytesInBlock);

	// Use a new block if there is not enough space left
	if(!alignedPtr)
	{
		// Allocate a new one if there are no blocks left
		if(m_currentBlockIdx + 1 == m_blocks.size())
		{
			m_blocks.push_back(allocate_block(m_blockSizeInBytes));
		}

		// Try to align on the new block again before updating block states
		blockPtr = m_blocks[m_currentBlockIdx + 1].get();
		bytesInBlock = m_blockSizeInBytes;
		alignedPtr = std::align(alignmentInBytes, numBytes, blockPtr, bytesInBlock);

		// It is possible that the alignment requirement is impossible to met due to insufficient bytes
		// remaining in the block
		if(!alignedPtr)
		{
			throw std::bad_alloc();
		}

		++m_currentBlockIdx;
	}

	PH_ASSERT(alignedPtr);
	PH_ASSERT_GE(bytesInBlock, numBytes);

	// We have a successfully aligned allocation here, update block states
	
	// `std::align()` only adjusts `blockPtr` to the aligned memory location
	m_blockPtr = static_cast<std::byte*>(blockPtr) + numBytes;

	// `std::align()` only decreases `bytesInBlock` by the number of bytes used for alignment
	m_remainingBytesInBlock = bytesInBlock - numBytes;

	m_numUsedBytes += numBytes;

	return static_cast<std::byte*>(alignedPtr);
}

void MemoryArena::clear()
{
	if(!m_blocks.empty())
	{
		// Call all destructors in reverse order
		for(auto dtorCallerIt = m_dtorCallers.crbegin(); 
		    dtorCallerIt != m_dtorCallers.crend(); 
		    ++dtorCallerIt)
		{
			(*dtorCallerIt)();
		}
		m_dtorCallers.clear();

		m_currentBlockIdx       = 0;
		m_blockPtr              = m_blocks[0].get();
		m_remainingBytesInBlock = m_blockSizeInBytes;
		m_numUsedBytes          = 0;
	}
	else
	{
		// Nothing should already be added while there is no block
		PH_ASSERT(m_dtorCallers.empty());

		m_currentBlockIdx       = static_cast<std::size_t>(-1);
		m_blockPtr              = nullptr;
		m_remainingBytesInBlock = m_blockSizeInBytes;
		m_numUsedBytes          = 0;
	}
}

}// end namespace ph
