#pragma once

#include "RenderCore/Memory/GraphicsMemoryBlock.h"

#include <Common/assertion.h>
#include <Common/math_basics.h>

#include <utility>
#include <memory>
#include <type_traits>

namespace ph::editor::ghi
{

inline std::size_t GraphicsMemoryBlock::numAllocatedBytes() const
{
	return m_blockSizeInBytes;
}

inline std::size_t GraphicsMemoryBlock::numUsedBytes() const
{
	PH_ASSERT_GE(m_blockSizeInBytes, m_remainingBytesInBlock);
	return m_blockSizeInBytes - m_remainingBytesInBlock;
}

inline std::size_t GraphicsMemoryBlock::numRemainingBytes() const
{
	return m_remainingBytesInBlock;
}

inline void GraphicsMemoryBlock::clear()
{
	m_ptrInBlock = m_blockSource;
	m_remainingBytesInBlock = m_blockSizeInBytes;
}

template<typename T, typename... Args>
inline T* GraphicsMemoryBlock::make(Args&&... args)
{
	static_assert(std::is_trivially_destructible_v<T>);

	// IOC of array of size 1
	T* const storage = reinterpret_cast<T*>(allocRaw(sizeof(T), alignof(T)));
	if(!storage)
	{
		return nullptr;
	}

	return std::construct_at(storage, std::forward<Args>(args)...);
}

template<typename T>
inline TSpan<T> GraphicsMemoryBlock::makeArray(const std::size_t arraySize)
{
	// To not violate [basic.life] Section 8.3 later
	// (https://timsong-cpp.github.io/cppwp/basic.life#8.3)
	using NonConstT = std::remove_const_t<T>;

	static_assert(std::is_default_constructible_v<NonConstT>);
	static_assert(std::is_trivially_destructible_v<NonConstT>);

	// IOC of array of size `arraySize`
	NonConstT* const storage = reinterpret_cast<NonConstT*>(
		allocRaw(sizeof(NonConstT) * arraySize, alignof(NonConstT)));
	if(!storage)
	{
		return {};
	}

	for(std::size_t i = 0; i < arraySize; ++i)
	{
		std::construct_at(storage + i, NonConstT{});
	}

	// Potentially do implicit non-const -> const conversion
	return TSpan<NonConstT>(storage, arraySize);
}

inline std::byte* GraphicsMemoryBlock::allocRaw(
	const std::size_t numBytes, 
	const std::size_t alignmentInBytes)
{
	PH_ASSERT(m_blockSource);
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

	return start_implicit_lifetime_as_array<std::byte>(alignedPtr, numBytes);
}

}// end namespace ph::editor::ghi
