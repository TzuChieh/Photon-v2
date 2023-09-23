#pragma once

#include "RenderCore/Memory/GraphicsMemoryBlock.h"

#include <Common/memory.h>
#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor::ghi
{

/*! @brief A chunk of cache aligned main memory.
*/
class HostMemoryBlock : public GraphicsMemoryBlock
{
public:
	/*! @brief Empty block without any allocation.
	*/
	HostMemoryBlock();

	/*!
	See `HostMemoryBlock(2)`. Alignment is automatically determined in this variant.
	*/
	explicit HostMemoryBlock(std::size_t blockSizeHintInBytes);

	/*!
	@param blockSizeHintInBytes Size of the memory block. The size provided is only a hint and
	imposed a lower limit on the maximum size of a single allocation. Actual size may be larger
	for performance reasons.
	@param alignmentInBytes Alignment of the memory block.
	*/
	HostMemoryBlock(std::size_t blockSizeHintInBytes, std::size_t alignmentInBytes);

	HostMemoryBlock(HostMemoryBlock&& other) noexcept;
	HostMemoryBlock& operator = (HostMemoryBlock&& rhs) noexcept;

	friend void swap(HostMemoryBlock& first, HostMemoryBlock& second) noexcept;

private:
	TAlignedMemoryUniquePtr<std::byte> m_memoryBlock;
};

}// end namespace ph::editor::ghi

#include "RenderCore/Memory/HostMemoryBlock.ipp"
