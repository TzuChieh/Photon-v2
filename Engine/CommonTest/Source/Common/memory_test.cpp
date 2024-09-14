#include <Common/memory.h>
#include <Common/compiler.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstddef>
#include <bit>
#include <type_traits>

TEST(MemoryTest, AllocateAlignedMemory)
{
	// `sizeof(void*)` must be a multiple of 2 for the following loop to work
	ASSERT_TRUE(std::has_single_bit(sizeof(void*)));

	// Max alignment: 64 MiB
	for(std::size_t alignment = sizeof(void*); alignment <= 1024 * 1024 * 64; alignment *= 2)
	{
		// Max allocated size: 64 MiB * 4
		for(std::size_t size = alignment; size <= alignment * 4; size += alignment)
		{
			auto res = ph::make_aligned_memory(size, alignment);
			ASSERT_TRUE(res);

			auto address = reinterpret_cast<std::uintptr_t>(res.get());
			EXPECT_EQ(address % alignment, 0);

			//std::cerr << size << ", " << alignment << ", " << address << std::endl;
		}
	}
}

// Supress the warning: Alignment specifier is less than actual alignment(4), and will be ignored.
#if PH_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable: 4359)
#endif

// Supress the warning: Requested alignment '0' is not a positive power of 2.
#if PH_COMPILER_IS_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

TEST(MemoryTest, AlignedArrayRequirements)
{
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<int, 10, 0>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<int, 10, 1>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<int, 10, 2>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<int, 10, 4>>);

	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 6, 0>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 7, 1>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 8, 2>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 9, 32>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 10, 64>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 10, 128>>);

	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 3>, 8, 0>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 4>, 9, 16>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 5>, 10, 32>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 5>, 10, 64>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 5>, 10, 128>>);
}

#if PH_COMPILER_IS_MSVC
#pragma warning(pop)
#endif

#if PH_COMPILER_IS_GNU
#pragma GCC diagnostic pop
#endif
