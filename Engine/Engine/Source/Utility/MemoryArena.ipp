#pragma once

#include "Utility/MemoryArena.h"

#include <type_traits>

namespace ph
{

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

template<typename T>
inline T* MemoryArena::alloc()
{
	// User is responsible for managing lifetime. This restriction helps to reduce the risk of
	// forgetting to end the object's lifetime.
	static_assert(std::is_trivially_destructible_v<T>);

	// IOC of array of size 1
	return reinterpret_cast<T*>(allocRaw(sizeof(T), alignof(T)));
}

template<typename T>
inline TSpan<T> MemoryArena::allocArray(const std::size_t arraySize)
{
	// User is responsible for managing lifetime. This restriction helps to reduce the risk of
	// forgetting to end the object's lifetime.
	static_assert(std::is_trivially_destructible_v<T>);

	// IOC of array of size `arraySize`
	T* const storage = reinterpret_cast<T*>(allocRaw(sizeof(T) * arraySize, alignof(T)));

	return TSpan<T>(storage, arraySize);
}

template<typename T, typename... Args>
inline T* MemoryArena::make(Args&&... args)
{
	if constexpr(std::is_trivially_destructible_v<T>)
	{
		return std::construct_at(alloc<T>(), std::forward<Args>(args)...);
	}
	else
	{
		T* const objPtr = std::construct_at(
			reinterpret_cast<T*>(allocRaw(sizeof(T), alignof(T))), 
			std::forward<Args>(args)...);

		// Record the dtor so we can call it later (on clear)
		m_dtorCallers.push_back(
			[objPtr]()
			{
				objPtr->~T();
			});

		return objPtr;
	}
}

}// end namespace ph
