#pragma once

#include <Utility/IMoveOnly.h>
#include <Common/memory.h>
#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor
{

class GraphicsMemoryBlock : private IMoveOnly
{
protected:
	GraphicsMemoryBlock();
	GraphicsMemoryBlock(GraphicsMemoryBlock&& other) noexcept;
	GraphicsMemoryBlock& operator = (GraphicsMemoryBlock&& rhs) noexcept;

	void setBlockSource(std::byte* source, std::size_t blockSizeInBytes);

	friend void swap(GraphicsMemoryBlock& first, GraphicsMemoryBlock& second) noexcept;

public:
	virtual ~GraphicsMemoryBlock();

	std::size_t numAllocatedBytes() const;
	std::size_t numUsedBytes() const;
	std::size_t numRemainingBytes() const;

	/*! @brief Reset the usage of the block.
	All memory handed out are effectively deallocated/deleted after this call. The block
	then transitions to its initial state and is ready for allocation again.
	*/
	void clear();

	/*! @brief Make an object of type `T` as if by calling its constructor with `Args`.
	@tparam T Type for the object created. Must be trivially destructible.
	@param args Arguments for calling the constructor of @p T.
	@return Pointer to the created object. Null if not enough storage is left.
	*/
	template<typename T, typename... Args>
	T* make(Args&&... args);

	/*! @brief Make an array of default constructed objects of type `T`.
	@tparam T Type for the objects created. Must be default constructible and trivially destructible.
	@return Span of the created objects. Empty if not enough storage is left.
	*/
	template<typename T>
	TSpan<T> makeArray(std::size_t arraySize);

	bool hasBlockSource() const;

private:
	std::byte* allocRaw(std::size_t numBytes, std::size_t alignmentInBytes);

	std::byte* m_blockSource;
	std::size_t m_blockSizeInBytes;
	std::byte* m_ptrInBlock;
	std::size_t m_remainingBytesInBlock;
};

}// end namespace ph::editor

#include "RenderCore/Memory/GraphicsMemoryBlock.ipp"
