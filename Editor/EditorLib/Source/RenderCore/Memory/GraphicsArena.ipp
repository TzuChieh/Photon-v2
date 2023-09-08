#pragma once

#include "RenderCore/Memory/GraphicsArena.h"
#include "RenderCore/Memory/GraphicsMemoryBlock.h"
#include "RenderCore/ghi_exceptions.h"

#include <Common/assertion.h>

#include <utility>

namespace ph::editor
{

template<typename T, typename... Args>
inline T* GraphicsArena::make(Args&&... args)
{
	T* ptr = m_memoryBlock->make<T>(std::forward<Args>(args)...);
	if(!ptr)
	{
		m_memoryBlock = allocNextBlock();
		ptr = m_memoryBlock->make<T>(std::forward<Args>(args)...);
	}

	// Generally should not be null. If so, consider to increase the capacity of memory block.
	PH_ASSERT(ptr);

	return ptr;
}

template<typename T>
inline TSpan<T> GraphicsArena::makeArray(const std::size_t arraySize)
{
	TSpan<T> arr = m_memoryBlock->makeArray<T>(arraySize);

	// It is important to also check whether the array can possibly fit in a block--otherwise we
	// are just wasting what's left of the current block
	if(!arr.data() &&
	   m_memoryBlock->numUsedBytes() > 0 &&
	   arraySize > 0 &&
	   sizeof(T) * arraySize <= m_memoryBlock->numAllocatedBytes())
	{
		m_memoryBlock = allocNextBlock();
		arr = m_memoryBlock->makeArray<T>(arraySize);
	}

	return arr;
}

}// end namespace ph::editor
