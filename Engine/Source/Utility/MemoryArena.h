#pragma once

#include "Common/memory.h"
#include "Common/primitive_type.h"
#include "Utility/IMoveOnly.h"
#include "Utility/TFunction.h"
#include "Utility/TSpan.h"

#include <cstddef>
#include <vector>
#include <utility>
#include <memory>
#include <type_traits>

namespace ph
{

class MemoryArena final : private IMoveOnly
{
public:
	/*! @brief Empty arena without any allocation performed yet.
	*/
	MemoryArena();

	/*!
	@param blockSizeInBytes Size of each memory block in the arena. This imposed a limit 
	on the maximum size of a single allocation.
	@param numDefaultBlocks Number of pre-allocated blocks.
	*/
	MemoryArena(std::size_t blockSizeInBytes, std::size_t numDefaultBlocks);
	
	inline MemoryArena(MemoryArena&& other) = default;

	~MemoryArena();

	/*! @brief Allocate raw memory.
	The memory returned contains no object--placement new is required before any use of the
	memory content, otherwise it is UB by C++ standard.
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

	inline MemoryArena& operator = (MemoryArena&& rhs) = default;

	/*! @brief Allocate raw memory for type @p T.
	Convenient method for allocating raw memory for object of type @p T. Alignment is handled
	automatically. See allocRaw(std::size_t, std::size_t) for details.
	@tparam T Type for the raw memory allocated.
	*/
	template<typename T>
	inline T* alloc()
		requires std::is_trivially_destructible_v<T>
	{
		return reinterpret_cast<T*>(allocRaw(sizeof(T), alignof(T)));
	}

	/*! @brief Allocate raw memory for array of type @p T.
	Convenient method for allocating raw memory for array of type @p T. Alignment is handled
	automatically. See allocRaw(std::size_t, std::size_t) for details.
	@tparam T Type for the raw array memory allocated.
	*/
	template<typename T>
	inline TSpan<T> allocArray(const std::size_t arraySize)
		requires std::is_trivially_destructible_v<T>
	{
		return TSpan<T>(
			reinterpret_cast<T*>(allocRaw(sizeof(T) * arraySize, alignof(T))), 
			arraySize);
	}

	/*! @brief Make an object of type @p T.
	Convenient method for creating an object without needing a placement new later. Equivalent to
	allocate then placement new for an object of type @p T. Alignment is handled automatically.
	Additionally, destructors will be automatically called (if needed) when clearing the arena.
	@tparam T Type for the object created.
	@param args Arguments for calling the constructor of @p T.
	*/
	template<typename T, typename... Args>
	inline T* make(Args&&... args)
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

private:
	std::vector<TAlignedMemoryUniquePtr<std::byte>> m_blocks;

	std::size_t m_blockSizeInBytes;
	std::size_t m_currentBlockIdx;
	std::byte*  m_blockPtr;
	std::size_t m_remainingBytesInBlock;
	std::size_t m_numUsedBytes;

	// Destructors of non-trivially-copyable objects. Simply specify `MIN_SIZE_HINT = 0` for 
	// smallest possible `TFunction`; increase it if compilation failed (investigate first).
	std::vector<TFunction<void(void), 0>> m_dtorCallers;
};

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

}// end namespace ph
