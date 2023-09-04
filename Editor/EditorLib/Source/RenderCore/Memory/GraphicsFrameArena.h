#pragma once

#include <Utility/IMoveOnly.h>
#include <Common/memory.h>
#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor
{

class GraphicsFrameArena final : private IMoveOnly
{
public:
	GraphicsFrameArena();

	/*!
	@param blockSizeHintInBytes Size of the memory block in the arena. This imposed a limit
	on the maximum size of a single allocation. The parameter is only a hint, actual size may be
	larger for performance reasons.
	*/
	explicit GraphicsFrameArena(std::size_t blockSizeHintInBytes);

	GraphicsFrameArena(GraphicsFrameArena&& other) noexcept;
	GraphicsFrameArena& operator = (GraphicsFrameArena&& rhs) noexcept;

	std::size_t numAllocatedBytes() const;
	std::size_t numUsedBytes() const;
	std::size_t numRemainingBytes() const;

	/*! @brief Reset the usage of the arena.
	All memory handed out are effectively deallocated/deleted after this call. The arena
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

private:
	std::byte* allocRaw(std::size_t numBytes, std::size_t alignmentInBytes);

	TAlignedMemoryUniquePtr<std::byte> m_memoryBlock;
	std::byte* m_ptrInBlock;
	std::size_t m_blockSizeInBytes;
	std::size_t m_remainingBytesInBlock;
};

}// end namespace ph::editor

#include "RenderCore/Memory/GraphicsFrameArena.ipp"
