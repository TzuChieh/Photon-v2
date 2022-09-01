#include <Common/os.h>

#include <gtest/gtest.h>

TEST(OSTest, GetL1CacheLineSize)
{
	// Currently for most CPUs the size is 64. If not, we would like to make sure
	// whether it is reasonable and update this test.

	EXPECT_EQ(ph::os::get_L1_cache_line_size_in_bytes(), 64);
}
