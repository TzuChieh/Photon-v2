#include "Common/memory.h"
#include "Common/math_basics.h"

#include <Common/os.h>

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

/* We are placing allocation/free functions in .cpp file to ensure a program will use the same version
of them. Placing them in a header may expose the program to higher risk of using different versions of
memory management functions which can cause issues.
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
