#pragma once

#include "Common/memory.h"
#include "Common/primitive_type.h"
#include "Utility/IMoveOnly.h"

#include <cstddef>
#include <vector>
#include <span>
#include <utility>
#include <memory>
#include <type_traits>

namespace ph
{

class MemoryArena final : private IMoveOnly
{
public:
	/*! @brief Empty arena without any allocation performed yet.
	*/
	MemoryArena();

	MemoryArena(std::size_t blockSizeInBytes, std::size_t numDefaultBlocks);
	inline MemoryArena(MemoryArena&& other) = default;

	std::byte* allocRaw(std::size_t numBytes, std::size_t alignmentInBytes = alignof(std::max_align_t));

	std::size_t numUsedBytes() const;
	std::size_t numAllocatedBytes() const;
	std::size_t getBlockSizeInBytes() const;
	std::size_t numAllocatedBlocks() const;

	inline MemoryArena& operator = (MemoryArena&& rhs) = default;

	template<typename T>
	inline T* alloc()
		requires std::is_trivially_destructible_v<T>
	{
		return reinterpret_cast<T*>(allocRaw(sizeof(T), alignof(T)));
	}

	template<typename T>
	inline std::span<T> allocArray(const std::size_t arraySize)
		requires std::is_trivially_destructible_v<T>
	{
		return std::span<T>(allocRaw(sizeof(T) * arraySize, alignof(T)), arraySize);
	}

	template<typename T, typename... Args>
	inline T* make(Args&&... args)
		requires std::is_trivially_destructible_v<T>
	{
		return std::construct_at<T, Args>(alloc<T>(), std::forward<Args>(args));
	}

private:
	std::vector<TAlignedMemoryUniquePtr<std::byte>> m_blocks;

	std::size_t m_blockSizeInBytes;
	std::size_t m_currentBlockIdx;
	std::byte*  m_blockPtr;
	std::size_t m_remainingBytesInBlock;
	std::size_t m_numUsedBytes;
};

inline std::size_t MemoryArena::numUsedBytes() const
{
	return m_numUsedBytes;
}

inline std::size_t MemoryArena::numAllocatedBytes() const
{
	return getBlockSizeInBytes() * numAllocatedBlocks();
}

inline std::size_t MemoryArena::getBlockSizeInBytes() const
{
	return m_blockSizeInBytes;
}

inline std::size_t MemoryArena::numAllocatedBlocks() const
{
	return m_blocks.size();
}

}// end namespace ph
