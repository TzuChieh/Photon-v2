#pragma once

#include "Common/memory.h"
#include "Common/primitive_type.h"
#include "Utility/IMoveOnly.h"

#include <cstddef>
#include <vector>

namespace ph
{

class MemoryArena final : private IMoveOnly
{
public:
	MemoryArena();
	MemoryArena(std::size_t blockSizeInBytes, std::size_t numDefaultBlocks);
	MemoryArena(MemoryArena&& other);

	std::size_t numUsedBytes() const;
	std::size_t numAllocatedBytes() const;
	std::size_t getBlockSizeInBytes() const;
	std::size_t numAllocatedBlocks() const;

	MemoryArena& operator = (MemoryArena&& rhs);

private:
	std::vector<TAlignedMemoryUniquePtr<std::byte>> m_blocks;
	std::size_t m_blockSizeInBytes;
	std::size_t m_currentBlockIdx;
	std::byte* m_currentBlockPtr;
	std::size_t m_currentByteIdxInBlock;
	std::size_t m_numUsedBytes;
};



}// end namespace ph
