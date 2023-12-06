#include <Common/memory.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstddef>
#include <bit>

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
