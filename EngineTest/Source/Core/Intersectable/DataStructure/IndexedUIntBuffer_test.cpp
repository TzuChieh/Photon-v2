#include <Core/Intersectable/DataStructure/IndexedUIntBuffer.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(IndexedUIntBufferTest, BasicBufferStates)
{
	{
		IndexedUIntBuffer buffer;
		EXPECT_FALSE(buffer.isAllocated());
		EXPECT_EQ(buffer.numUInts(), 0);
		EXPECT_GT(buffer.memoryUsage(), 0);
	}

	{
		IndexedUIntBuffer buffer;
		buffer.declareUIntFormat(32);
		buffer.allocate(100);
		EXPECT_TRUE(buffer.isAllocated());
		EXPECT_EQ(buffer.numUInts(), 100);
		EXPECT_GT(buffer.memoryUsage(), 100 * 32 / 8);
	}
}

TEST(IndexedUIntBufferTest, BufferIO)
{
	// Single-uint buffer
	{
		IndexedUIntBuffer buffer;
		buffer.declareUIntFormat(32);
		buffer.allocate(1);

		buffer.setUInt(0, 7);
		EXPECT_EQ(buffer.getUInt(0), 7);
	}

	// Small buffer
	{
		IndexedUIntBuffer buffer;
		buffer.declareUIntFormat(32);
		buffer.allocate(10);

		buffer.setUInt(0,   0);
		buffer.setUInt(1, 100);
		buffer.setUInt(2, 200);
		buffer.setUInt(3, 300);
		buffer.setUInt(4, 400);
		buffer.setUInt(5, 500);
		buffer.setUInt(6, 600);
		buffer.setUInt(7, 700);
		buffer.setUInt(8, 800);
		buffer.setUInt(9, 900);

		EXPECT_EQ(buffer.getUInt(0),   0);
		EXPECT_EQ(buffer.getUInt(1), 100);
		EXPECT_EQ(buffer.getUInt(2), 200);
		EXPECT_EQ(buffer.getUInt(3), 300);
		EXPECT_EQ(buffer.getUInt(4), 400);
		EXPECT_EQ(buffer.getUInt(5), 500);
		EXPECT_EQ(buffer.getUInt(6), 600);
		EXPECT_EQ(buffer.getUInt(7), 700);
		EXPECT_EQ(buffer.getUInt(8), 800);
		EXPECT_EQ(buffer.getUInt(9), 900);
	}

	// Arbitrary-bit number
	{
		IndexedUIntBuffer buffer;
		buffer.declareUIntFormat(3);// can only store 0 ~ 7
		buffer.allocate(5);

		buffer.setUInt(0, 6);
		buffer.setUInt(1, 2);
		buffer.setUInt(2, 7);
		buffer.setUInt(3, 0);
		buffer.setUInt(4, 1);

		EXPECT_EQ(buffer.getUInt(0), 6);
		EXPECT_EQ(buffer.getUInt(1), 2);
		EXPECT_EQ(buffer.getUInt(2), 7);
		EXPECT_EQ(buffer.getUInt(3), 0);
		EXPECT_EQ(buffer.getUInt(4), 1);
	}

	// Arbitrary-bit number
	{
		IndexedUIntBuffer buffer;
		buffer.declareUIntFormat(50);
		buffer.allocate(5);

		buffer.setUInt(0, (uint64(1) << 50) - 1);
		buffer.setUInt(1, 0);
		buffer.setUInt(2, uint64(1) << 49);
		buffer.setUInt(3, 0);
		buffer.setUInt(4, (uint64(1) << 50) - 1);

		EXPECT_EQ(buffer.getUInt(0), (uint64(1) << 50) - 1);
		EXPECT_EQ(buffer.getUInt(1), 0);
		EXPECT_EQ(buffer.getUInt(2), uint64(1) << 49);
		EXPECT_EQ(buffer.getUInt(3), 0);
		EXPECT_EQ(buffer.getUInt(4), (uint64(1) << 50) - 1);
	}

	// Larger buffer
	{
		IndexedUIntBuffer buffer;
		buffer.declareUIntFormat(16);
		buffer.allocate(4096);

		for(std::size_t i = 0; i < buffer.numUInts(); ++i)
		{
			buffer.setUInt(i, i);
		}

		for(std::size_t i = 0; i < buffer.numUInts(); ++i)
		{
			EXPECT_EQ(buffer.getUInt(i), i);
		}
	}
}
