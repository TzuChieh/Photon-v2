#include "Common/memory.h"
#include "Common/os.h"
#include "Common/math_basics.h"

#if PH_OPERATING_SYSTEM_IS_WINDOWS

#include <malloc.h>
#include <crtdbg.h>

#else

#include <cstdlib>

#endif

namespace ph
{

namespace detail
{

/*!
@param numBytes Number of bytes to allocate. Must be an integer multiple of @p alignmentInBytes.
@param alignmentInBytes How many bytes to align (so the returned pointer is an integer multiple
of @p alignmentInBytes). Must be an integer power of 2 and a multiple of `sizeof(void*)`.
@return Pointer to the beginning of newly allocated memory. `nullptr` on failure.
@note Call free_aligned_memory(void*) to deallocate the memory. This function is thread safe.
*/
void* allocate_aligned_memory(const std::size_t numBytes, const std::size_t alignmentInBytes)
{
	// Alignment must be an integer power of 2.
	PH_ASSERT(math::is_power_of_2(alignmentInBytes));

	// Alignment must be a multiple of `sizeof(void*)`.
	PH_ASSERT_EQ(alignmentInBytes % sizeof(void*), 0);

	// Allocation size must be an integer multiple of alignment (`std::aligned_alloc` requirement).
	// (here `alignmentInBytes` is guaranteed to be > 0 by the previous assertion)
	PH_ASSERT(numBytes % alignmentInBytes == 0);

	PH_ASSERT_GT(numBytes, 0);

#if PH_OPERATING_SYSTEM_IS_WINDOWS

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
@note This function is thread safe.
*/
void free_aligned_memory(void* const ptr)
{
#if PH_OPERATING_SYSTEM_IS_WINDOWS

	// Reduced to a call to `_aligned_free` when `_DEBUG` is not defined.
	// See https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/aligned-free-dbg?view=msvc-170
	_aligned_free_dbg(ptr);

#else

	std::free(ptr);

#endif
}

}// end namespace detail

}// end namespace ph
