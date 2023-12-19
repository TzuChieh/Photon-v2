#pragma once

/*! @file
@brief Low-level memory allocation routines.
*/

#include "Common/assertion.h"

#include <cstddef>
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
@note Call free_aligned_memory(void*) to deallocate the memory. The implementation is based on
`malloc()`, and object lifetime can be reasoned w.r.t. `malloc()`. This function is thread safe.
*/
[[nodiscard]]
void* allocate_aligned_memory(std::size_t numBytes, std::size_t alignmentInBytes);

/*!
@param ptr The memory to be deallocated. @p ptr must be allocated by allocate_aligned_memory(std::size_t, std::size_t).
If @p ptr is `nullptr`, no action is performed.
@note This function is thread safe.
*/
void free_aligned_memory(void* ptr);

struct AlignedMemoryDeleter
{
	inline void operator () (void* const ptr) const
	{
		free_aligned_memory(ptr);
	}

	inline void operator () (const void* const ptr) const
	{
		// `const_cast` to support const overload. This is fine as aligned memory is only returned as
		// a non-const pointer, and casting an originally non-const type is safe.
		free_aligned_memory(const_cast<void*>(ptr));
	}
};

}// end namespace detail

template<typename T>
using TAlignedMemoryUniquePtr = std::unique_ptr<T, detail::AlignedMemoryDeleter>;

// Note that `detail::AlignedMemoryDeleter` is for empty base optimization on `std::unique_ptr`, 
// see https://stackoverflow.com/questions/42715492/stdunique-ptr-and-custom-deleters.
// This would reduce the size of the resulting `unique_ptr` to the size of a single pointer.
// Reference: https://stackoverflow.com/questions/45341371/memory-efficient-custom-deleter-for-stdunique-ptr
// 
// The following test will ensure this is true:
static_assert(sizeof(TAlignedMemoryUniquePtr<void>) == sizeof(void*));

/*! @brief Create an aligned memory resource.

The returned memory resource will follow the life time of `std::unique_ptr`. Note that the memory
allocated by this function is raw memory--placement new is required before any use of the memory
content, otherwise it is UB by C++ standard (for non-implicit-lifetime types). For implicit-lifetime
types, accessing the raw memory without placement new has defined behavior after C++20.

@tparam T The type to create memory for. `alignmentInBytes` should be compatible with the type.
@param numBytes Number of bytes to allocate. Must be an integer multiple of @p alignmentInBytes.
@param alignmentInBytes How many bytes to align (so the returned pointer is an integer multiple
of @p alignmentInBytes). Must be an integer power of 2 and a multiple of `sizeof(void*)`.
@return Pointer to the beginning of newly allocated memory. `nullptr` on failure.
@note This function is thread safe.
*/
template<typename T = void>
inline auto make_aligned_memory(std::size_t numBytes, std::size_t alignmentInBytes)
-> TAlignedMemoryUniquePtr<T>;

template<typename T>
void from_bytes(const std::byte* srcBytes, T* out_dstValue);

template<typename T>
void to_bytes(const T& srcValue, std::byte* out_dstBytes);

template<std::size_t N>
void reverse_bytes(std::byte* bytes);

/*! @brief Wrapper for `std::start_lifetime_as()`.
Primarily a fallback when C++23 is not available. This function may touch the storage. For cv overloads
or one that does not touch the storage, see `std::start_lifetime_as()` (requires C++23).
@note Starting lifetime of an array of `unsigned char` or `std::byte` implicitly creates objects within
the region of storage. See [intro.object] section 13 (https://timsong-cpp.github.io/cppwp/intro.object#13).
*/
template<typename T>
T* start_implicit_lifetime_as(void* ptr) noexcept;

/*! @brief Wrapper for `std::start_lifetime_as_array()`.
Primarily a fallback when C++23 is not available. This function may touch the storage. For cv overloads
or one that does not touch the storage, see `std::start_lifetime_as_array()` (requires C++23).
@note Starting lifetime of an array of `unsigned char` or `std::byte` implicitly creates objects within
the region of storage. See [intro.object] section 13 (https://timsong-cpp.github.io/cppwp/intro.object#13).
*/
template<typename T>
T* start_implicit_lifetime_as_array(void* ptr, std::size_t arrSize) noexcept;

}// end namespace ph

#include "Common/memory.ipp"
