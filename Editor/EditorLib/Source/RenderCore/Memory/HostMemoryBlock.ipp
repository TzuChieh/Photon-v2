#pragma once

#include "RenderCore/Memory/HostMemoryBlock.h"
#include "RenderCore/ghi_exceptions.h"

#include <Common/os.h>
#include <Common/math_basics.h>

#include <numeric>
#include <new>
#include <utility>

namespace ph::editor::ghi
{

inline HostMemoryBlock::HostMemoryBlock()
	: GraphicsMemoryBlock()
	, m_memoryBlock(nullptr)
{}

inline HostMemoryBlock::HostMemoryBlock(std::size_t blockSizeHintInBytes)
	: HostMemoryBlock(blockSizeHintInBytes, os::get_L1_cache_line_size_in_bytes())
{}

inline HostMemoryBlock::HostMemoryBlock(std::size_t blockSizeHintInBytes, std::size_t alignmentInBytes)
{
	const auto alignmentSize = std::lcm(alignof(std::max_align_t), alignmentInBytes);
	const auto blockSize = math::next_multiple(blockSizeHintInBytes, alignmentSize);

	m_memoryBlock = make_aligned_memory<std::byte>(blockSize, alignmentSize);
	if(!m_memoryBlock)
	{
		throw OutOfHostMemory{};
	}

	setBlockSource(m_memoryBlock.get(), blockSize);
}

inline HostMemoryBlock::HostMemoryBlock(HostMemoryBlock&& other) noexcept
	: HostMemoryBlock()
{
	swap(*this, other);
}

inline HostMemoryBlock& HostMemoryBlock::operator = (HostMemoryBlock&& rhs) noexcept
{
	swap(*this, rhs);

	return *this;
}

inline void swap(HostMemoryBlock& first, HostMemoryBlock& second) noexcept
{
	using std::swap;

	swap(static_cast<GraphicsMemoryBlock&>(first), static_cast<GraphicsMemoryBlock&>(second));
	swap(first.m_memoryBlock, second.m_memoryBlock);
}

}// end namespace ph::editor::ghi
