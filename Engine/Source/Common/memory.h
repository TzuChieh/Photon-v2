#pragma once

#include "Common/os.h"
#include "Common/assertion.h"

#if defined(PH_OPERATING_SYSTEM_IS_WINDOWS)

#include <malloc.h>
#include <crtdbg.h>

#else

#include <cstdlib>

#endif

#include <cstddef>
#include <bit>
#include <memory>

namespace ph
{

namespace detail
{

/*!
@param numBytes Number of bytes to allocate. Must be an integer multiple of @p alignmentInBytes.
@param alignmentInBytes How many bytes to align (so the returned pointer is an integer multiple
of @p alignmentInBytes). Must be an integer power of 2 and a multiple of `sizeof(void*)`.
@return Pointer to the beginning of newly allocated memory. `nullptr` on failure.
@note Call free_aligned_memory(void*) to deallocate the memory.
*/
inline void* allocate_aligned_memory(const std::size_t numBytes, const std::size_t alignmentInBytes)
{
	// Alignment must be an integer power of 2.
	PH_ASSERT(std::has_single_bit(alignmentInBytes));

	// Alignment must be a multiple of `sizeof(void*)`.
	PH_ASSERT_EQ(alignmentInBytes % sizeof(void*), 0);

	// Allocation size must be an integer multiple pf alignment.
	// (here `alignmentInBytes` is guaranteed to be > 0 by the previous assertion)
	PH_ASSERT(numBytes % alignmentInBytes == 0);

	PH_ASSERT_GT(numBytes, 0);

#if defined(PH_OPERATING_SYSTEM_IS_WINDOWS)

	// Reduced to a call to `_aligned_malloc` when `_DEBUG` is not defined.
	// See https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/aligned-malloc-dbg?view=msvc-170
	return _aligned_malloc_dbg(numBytes, alignmentInBytes, __FILE__, __LINE__);

#else

	return std::aligned_alloc(alignmentInBytes, numBytes);

#endif
}

/*!
@param ptr The memory to be deallocated. @p ptr must be allocated by allocate_aligned_memory(std::size_t, std::size_t).
If @p ptr is `nullptr`, no action is performed.
*/
inline void free_aligned_memory(void* const ptr)
{
#if defined(PH_OPERATING_SYSTEM_IS_WINDOWS)

	// Reduced to a call to `_aligned_free` when `_DEBUG` is not defined.
	// See https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/aligned-free-dbg?view=msvc-170
	_aligned_free_dbg(ptr);

#else

	std::free(ptr);

#endif
}

// For empty base optimization on `std::unique_ptr`, see https://stackoverflow.com/questions/42715492/stdunique-ptr-and-custom-deleters.
// This would reduce the size of the resulting `unique_ptr` to the size of a single pointer.
// Reference: https://stackoverflow.com/questions/45341371/memory-efficient-custom-deleter-for-stdunique-ptr
struct AlignedMemoryDeleter final
{
	inline void operator () (void* const ptr) const
	{
		free_aligned_memory(ptr);
	}
};

}// end namespace detail

template<typename T>
using TAlignedMemoryUniquePtr = std::unique_ptr<T, detail::AlignedMemoryDeleter>;

/*! @brief Create an aligned memory resource.

The returned memory resource will follow the life time of `std::unique_ptr`.

@param numBytes Number of bytes to allocate. Must be an integer multiple of @p alignmentInBytes.
@param alignmentInBytes How many bytes to align (so the returned pointer is an integer multiple
of @p alignmentInBytes). Must be an integer power of 2 and a multiple of `sizeof(void*)`.
@return Pointer to the beginning of newly allocated memory. `nullptr` on failure.
*/
template<typename T = void>
inline auto make_aligned_memory(const std::size_t numBytes, const std::size_t alignmentInBytes)
	-> TAlignedMemoryUniquePtr<T>
{
	void* const ptr = detail::allocate_aligned_memory(numBytes, alignmentInBytes);

	return ptr != nullptr
		? TAlignedMemoryUniquePtr<T>(static_cast<T*>(ptr))
		: nullptr;
}

}// end namespace ph
