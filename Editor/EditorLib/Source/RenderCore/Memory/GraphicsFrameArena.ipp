#pragma once

#include "RenderCore/Memory/GraphicsFrameArena.h"

#include <Common/os.h>
#include <Common/math_basics.h>
#include <Common/assertion.h>

#include <numeric>
#include <new>
#include <utility>
#include <memory>
#include <type_traits>

namespace ph::editor
{

inline GraphicsFrameArena::GraphicsFrameArena()
	: m_memoryBlock()
	, m_ptrInBlock(nullptr)
	, m_blockSizeInBytes(0)
	, m_remainingBytesInBlock(0)
{}

inline GraphicsFrameArena::GraphicsFrameArena(const std::size_t blockSizeHintInBytes)
	: GraphicsFrameArena()
{
	const auto alignmentSize = std::lcm(alignof(std::max_align_t), os::get_L1_cache_line_size_in_bytes());
	const auto blockSize = math::next_multiple(blockSizeHintInBytes, alignmentSize);

	m_memoryBlock = make_aligned_memory<std::byte>(blockSize, alignmentSize);
	if(!m_memoryBlock)
	{
		throw std::bad_alloc();
	}

	m_ptrInBlock = m_memoryBlock.get();
	m_blockSizeInBytes = blockSize;
	m_remainingBytesInBlock = blockSize;
}

inline GraphicsFrameArena::GraphicsFrameArena(GraphicsFrameArena&& other) noexcept
	: GraphicsFrameArena()
{
	*this = std::move(other);
}

inline GraphicsFrameArena& GraphicsFrameArena::operator = (GraphicsFrameArena&& rhs) noexcept
{
	using std::swap;

	swap(m_memoryBlock, rhs.m_memoryBlock);
	swap(m_ptrInBlock, rhs.m_ptrInBlock);
	swap(m_blockSizeInBytes, rhs.m_blockSizeInBytes);
	swap(m_remainingBytesInBlock, rhs.m_remainingBytesInBlock);

	return *this;
}


inline std::size_t GraphicsFrameArena::numAllocatedBytes() const
{
	return m_blockSizeInBytes;
}

inline std::size_t GraphicsFrameArena::numUsedBytes() const
{
	PH_ASSERT_GE(m_blockSizeInBytes, m_remainingBytesInBlock);
	return m_blockSizeInBytes - m_remainingBytesInBlock;
}

inline std::size_t GraphicsFrameArena::numRemainingBytes() const
{
	return m_remainingBytesInBlock;
}

inline void GraphicsFrameArena::clear()
{
	m_ptrInBlock = m_memoryBlock.get();
	m_remainingBytesInBlock = m_blockSizeInBytes;
}

template<typename T, typename... Args>
inline T* GraphicsFrameArena::make(Args&&... args)
{
	static_assert(std::is_trivially_destructible_v<T>);

	T* const storage = reinterpret_cast<T*>(allocRaw(sizeof(T), alignof(T)));
	if(!storage)
	{
		return nullptr;
	}

	return std::construct_at(storage, std::forward<Args>(args)...);
}

template<typename T>
inline TSpan<T> GraphicsFrameArena::makeArray(const std::size_t arraySize)
{
	static_assert(std::is_default_constructible_v<T>);
	static_assert(std::is_trivially_destructible_v<T>);

	T* const storage = reinterpret_cast<T*>(allocRaw(sizeof(T) * arraySize, alignof(T)));
	if(!storage)
	{
		return {};
	}

	for(std::size_t i = 0; i < arraySize; ++i)
	{
		std::construct_at(storage + i, T{});
	}
	return TSpan<T>(storage, arraySize);
}

inline std::byte* GraphicsFrameArena::allocRaw(
	const std::size_t numBytes, 
	const std::size_t alignmentInBytes)
{
	PH_ASSERT(m_ptrInBlock);
	PH_ASSERT(math::is_power_of_2(alignmentInBytes));

	void* ptr = m_ptrInBlock;
	std::size_t availableBytes = m_remainingBytesInBlock;
	void* alignedPtr = std::align(alignmentInBytes, numBytes, ptr, availableBytes);

	// Return null if there is not enough space left
	if(!alignedPtr)
	{
		return nullptr;
	}

	PH_ASSERT(alignedPtr);
	PH_ASSERT_GE(availableBytes, numBytes);

	// We have a successfully aligned allocation here, update block states
	
	// `std::align()` only adjusts `ptr` to the aligned memory location
	m_ptrInBlock = static_cast<std::byte*>(ptr) + numBytes;

	// `std::align()` only decreases `availableBytes` by the number of bytes used for alignment
	m_remainingBytesInBlock = availableBytes - numBytes;

	return static_cast<std::byte*>(alignedPtr);
}

}// end namespace ph::editor
