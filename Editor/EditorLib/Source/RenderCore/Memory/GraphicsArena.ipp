#pragma once

#include "RenderCore/Memory/GraphicsArena.h"
#include "RenderCore/Memory/GraphicsMemoryBlock.h"
#include "RenderCore/ghi_exceptions.h"

#include <Common/assertion.h>

#include <utility>

namespace ph::editor::ghi
{

template<typename T, typename... Args>
inline T* GraphicsArena::make(Args&&... args)
{
	if(!m_memoryBlock)
	{
		m_memoryBlock = allocNextBlock();
	}

	// Get next block if failed
	T* ptr = m_memoryBlock->make<T>(std::forward<Args>(args)...);
	if(!ptr)
	{
		m_memoryBlock = allocNextBlock();
		ptr = m_memoryBlock->make<T>(std::forward<Args>(args)...);
	}

	// Generally should not be null. If so, consider to increase the size of default memory block.
	PH_ASSERT(ptr);
	return ptr;
}

template<typename T>
inline TSpan<T> GraphicsArena::makeArray(std::size_t arraySize)
{
	PH_ASSERT_GT(arraySize, 0);

	if(!m_memoryBlock)
	{
		m_memoryBlock = allocNextBlock();
	}

	// Alignment not checked--the allocation generally will align to `std::max_align_t` at least.
	// If the user specifies custom alignment, there may be extra cost to satisfy the request
	// (e.g., wasting a new default block then allocating a custom block).
	bool mayFitDefaultBlock = sizeof(T) * arraySize <= m_memoryBlock->numAllocatedBytes();

	TSpan<T> arr;
	if(mayFitDefaultBlock)
	{
		// Only retry if current block is not a new one
		arr = m_memoryBlock->makeArray<T>(arraySize);
		if(!arr.data() && m_memoryBlock->numUsedBytes() > 0)
		{
			m_memoryBlock = allocNextBlock();
			arr = m_memoryBlock->makeArray<T>(arraySize);
		}
	}

	// If we cannot make the array with default block, then use a custom block with performance hit
	if(!arr.data())
	{
		GraphicsMemoryBlock* customBlock = allocCustomBlock(sizeof(T) * arraySize, alignof(T));
		arr = customBlock->makeArray<T>(arraySize);
	}

	// Cannot be null as we can fallback to custom block.
	PH_ASSERT(arr.data());
	return arr;
}

}// end namespace ph::editor::ghi
