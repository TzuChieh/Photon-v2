#pragma once

#include "Utility/IMoveOnly.h"
#include "Utility/TFunction.h"
#include "Utility/TSpan.h"

#include <Common/primitive_type.h>
#include <Common/memory.h>

#include <cstddef>
#include <vector>
#include <utility>
#include <memory>

namespace ph
{

/*! @brief A general purpose bump allocator.
*/
class MemoryArena final : private IMoveOnly
{
public:
	/*! @brief Empty arena without any allocation performed yet.
	*/
	MemoryArena();

	/*!
	@param blockSizeHintInBytes Size of each memory block in the arena. This imposed a limit
	on the maximum size of a single allocation. The parameter is only a hint, actual size may be
	larger for performance reasons.
	@param numDefaultBlocks Number of pre-allocated blocks.
	*/
	MemoryArena(std::size_t blockSizeHintInBytes, std::size_t numDefaultBlocks);
	
	/*! @brief Move another arena into this one.
	The moved-from arena cannot be used unless:
	1. yet another arena has moved into it
	2. `clear()` has been called.
	*/
	///@{
	inline MemoryArena(MemoryArena&& other) = default;
	inline MemoryArena& operator = (MemoryArena&& rhs) = default;
	///@}

	~MemoryArena();

	/*! @brief Allocate raw memory.
	Generally speaking, the memory returned contains no object--placement new is required before any
	use of the memory content for most object types, otherwise it is UB by C++ standard. An exception
	is implicit-lifetime types. This method implicitly create objects of implicit-lifetime type in the
	memory returned.
	@param numBytes Number of bytes to allocate.
	@param alignmentInBytes Alignment requirement of the allocation, in bytes.
	@return Allocated memory, never `nullptr`.
	@exception std::bad_alloc If the allocation failed.
	*/
	std::byte* allocRaw(std::size_t numBytes, std::size_t alignmentInBytes = alignof(std::max_align_t));

	/*! @brief Reset the usage of the arena.
	All memory handed out are effectively deallocated/deleted after this call. The arena
	then transitions to its initial state and is ready for allocation again. Destructors are
	called if required.
	*/
	void clear();

	std::size_t numUsedBytes() const;
	std::size_t numAllocatedBytes() const;
	std::size_t getBlockSizeInBytes() const;
	std::size_t numAllocatedBlocks() const;

	/*! @brief Allocate raw memory for type @p T.
	Convenient method for allocating raw memory for object of type @p T. Alignment is handled
	automatically. See allocRaw(std::size_t, std::size_t) for details.
	@tparam T Type for the raw memory allocated, must be trivially destructible.
	@note Provides storage only. Placement new is required before any use of the memory content.
	For const types, laundering is needed to access the created object via the returned pointer
	([basic.life] Section 8.3).
	*/
	template<typename T>
	T* alloc();

	/*! @brief Allocate raw memory for array of type @p T.
	Convenient method for allocating raw memory for array of type @p T. Alignment is handled
	automatically. See allocRaw(std::size_t, std::size_t) for details.
	@tparam T Type for the raw array memory allocated, must be trivially destructible.
	@note Provides storage only. Placement new is required before any use of the memory content.
	For const types, laundering is needed to access the created object via the returned pointer
	([basic.life] Section 8.3).
	*/
	template<typename T>
	TSpan<T> allocArray(const std::size_t arraySize);

	/*! @brief Make an object of type @p T.
	Convenient method for creating an object without needing a placement new later. Equivalent to
	allocate then placement new for an object of type @p T. Alignment is handled automatically.
	Additionally, destructors will be automatically called (if needed) when clearing the arena.
	@tparam T Type for the object created.
	@param args Arguments for calling the constructor of @p T.
	*/
	template<typename T, typename... Args>
	T* make(Args&&... args);

private:
	std::vector<TAlignedMemoryUniquePtr<std::byte>> m_blocks;

	std::size_t m_blockSizeInBytes;
	std::size_t m_currentBlockIdx;
	std::byte*  m_blockPtr;
	std::size_t m_remainingBytesInBlock;
	std::size_t m_numUsedBytes;

	// Destructors of non-trivially-copyable objects. Simply specify `MIN_SIZE_HINT = 0` for 
	// smallest possible `TFunction`; increase it if compilation failed (this is unlikely, 
	// should investigate first).
	std::vector<TFunction<void(void), 0>> m_dtorCallers;
};

}// end namespace ph

#include "Utility/MemoryArena.ipp"
