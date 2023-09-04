#pragma once

#include "RenderCore/Memory/GraphicsMemoryBlock.h"

#include <Common/memory.h>
#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor
{

class HostMemoryBlock : public GraphicsMemoryBlock
{
public:
	HostMemoryBlock();

	/*!
	@param blockSizeHintInBytes Size of the memory block. The size provided is only a hint and
	imposed a lower limit on the maximum size of a single allocation. Actual size may be larger
	for performance reasons.
	*/
	explicit HostMemoryBlock(std::size_t blockSizeHintInBytes);

	HostMemoryBlock(HostMemoryBlock&& other) noexcept;
	HostMemoryBlock& operator = (HostMemoryBlock&& rhs) noexcept;

	friend void swap(HostMemoryBlock& first, HostMemoryBlock& second) noexcept;

private:
	TAlignedMemoryUniquePtr<std::byte> m_memoryBlock;
};

}// end namespace ph::editor

#include "RenderCore/Memory/HostMemoryBlock.ipp"
