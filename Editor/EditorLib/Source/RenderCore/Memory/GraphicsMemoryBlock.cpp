#include "RenderCore/Memory/GraphicsMemoryBlock.h"
#include "RenderCore/ghi_exceptions.h"

#include <Common/assertion.h>

#include <new>
#include <utility>

namespace ph::editor::ghi
{

GraphicsMemoryBlock::GraphicsMemoryBlock()
	: m_blockSource(nullptr)
	, m_blockSizeInBytes(0)
	, m_ptrInBlock(nullptr)
	, m_remainingBytesInBlock(0)
{}

GraphicsMemoryBlock::GraphicsMemoryBlock(GraphicsMemoryBlock&& other) noexcept
	: GraphicsMemoryBlock()
{
	swap(*this, other);
}

GraphicsMemoryBlock& GraphicsMemoryBlock::operator = (GraphicsMemoryBlock&& rhs) noexcept
{
	swap(*this, rhs);

	return *this;
}

GraphicsMemoryBlock::~GraphicsMemoryBlock() = default;

void GraphicsMemoryBlock::setBlockSource(std::byte* const source, const std::size_t blockSizeInBytes)
{
	if(!source || blockSizeInBytes == 0)
	{
		throw BadAllocation{};
	}

	m_blockSource = source;
	m_blockSizeInBytes = blockSizeInBytes;

	clear();
}

bool GraphicsMemoryBlock::hasBlockSource() const
{
	return m_blockSource != nullptr;
}

void swap(GraphicsMemoryBlock& first, GraphicsMemoryBlock& second) noexcept
{
	using std::swap;

	swap(first.m_blockSource, second.m_blockSource);
	swap(first.m_blockSizeInBytes, second.m_blockSizeInBytes);
	swap(first.m_ptrInBlock, second.m_ptrInBlock);
	swap(first.m_remainingBytesInBlock, second.m_remainingBytesInBlock);
}

}// end namespace ph::editor::ghi
