#include "Utility/MemoryArena.h"
#include "Common/config.h"

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

}// end anonymous namespace

MemoryArena::MemoryArena()
{

}

MemoryArena::MemoryArena(const std::size_t blockSizeInBytes, const std::size_t numDefaultBlocks)
	: m_blocks()
	, m_blockSizeInBytes;
std::size_t m_currentBlockIdx;
std::byte* m_currentBlockPtr;
std::size_t m_currentByteIdxInBlock;
std::size_t m_numUsedBytes;

{

}

}// end namespace ph
