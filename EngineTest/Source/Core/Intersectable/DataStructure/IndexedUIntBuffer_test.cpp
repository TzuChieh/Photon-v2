#include <Core/Intersectable/DataStructure/IndexedUIntBuffer.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(IndexedUIntBufferTest, BasicBufferStates)
{
	{
		IndexedUIntBuffer buffer;
		EXPECT_FALSE(buffer.isAllocated());
		EXPECT_EQ(buffer.numUInts(), 0);
		EXPECT_GT(buffer.estimateMemoryUsageBytes(), 0);
	}

	{
		IndexedUIntBuffer buffer;
		buffer.setUIntFormat(32);
		buffer.allocate(100);
		EXPECT_TRUE(buffer.isAllocated());
		EXPECT_EQ(buffer.numUInts(), 100);
		EXPECT_GT(buffer.estimateMemoryUsageBytes(), 100 * 32 / 4);
	}
}

TEST(IndexedUIntBufferTest, BufferIO)
{
	{
		IndexedUIntBuffer buffer;
		buffer.setUIntFormat(32);
		buffer.allocate(1);

		buffer.setUInt(7, 0);
		EXPECT_EQ(buffer.getUInt(0), 7);
	}

	{
		IndexedUIntBuffer buffer;
		buffer.setUIntFormat(32);
		buffer.allocate(10);

		buffer.setUInt(0,   0);
		buffer.setUInt(100, 1);
		buffer.setUInt(200, 2);
		buffer.setUInt(300, 3);
		buffer.setUInt(400, 4);
		buffer.setUInt(500, 5);
		buffer.setUInt(600, 6);
		buffer.setUInt(700, 7);
		buffer.setUInt(800, 8);
		buffer.setUInt(900, 9);

		EXPECT_EQ(buffer.getUInt(0),   0);
		EXPECT_EQ(buffer.getUInt(100), 1);
		EXPECT_EQ(buffer.getUInt(200), 2);
		EXPECT_EQ(buffer.getUInt(300), 3);
		EXPECT_EQ(buffer.getUInt(400), 4);
		EXPECT_EQ(buffer.getUInt(500), 5);
		EXPECT_EQ(buffer.getUInt(600), 6);
		EXPECT_EQ(buffer.getUInt(700), 7);
		EXPECT_EQ(buffer.getUInt(800), 8);
		EXPECT_EQ(buffer.getUInt(900), 9);
	}
}
