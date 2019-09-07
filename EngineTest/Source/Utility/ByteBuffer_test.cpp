#include <Utility/ByteBuffer.h>

#include <gtest/gtest.h>

#include <array>

using namespace ph;

TEST(ByteBufferTest, Construction)
{
	// 1. Default
	{
		ByteBuffer buffer;

		EXPECT_TRUE(buffer.isEmpty());
		EXPECT_EQ(buffer.numBytes(), 0);
		EXPECT_EQ(buffer.getReadPosition(), 0);
		EXPECT_EQ(buffer.getWritePosition(), 0);
		EXPECT_FALSE(buffer.hasMoreToRead());
	}
	
	// 2. With reserved memory space
	{
		ByteBuffer buffer(123);

		EXPECT_TRUE(buffer.isEmpty());
		EXPECT_EQ(buffer.numBytes(), 0);
		EXPECT_EQ(buffer.getReadPosition(), 0);
		EXPECT_EQ(buffer.getWritePosition(), 0);
		EXPECT_FALSE(buffer.hasMoreToRead());
	}
}

TEST(ByteBufferTest, ReadingAndWriting)
{
	struct TestObject
	{
		int a;
		float b;
	};

	// 1. Write and read once (primitive)
	{
		ByteBuffer buffer;
		buffer.write(-333.0f);

		const auto result = buffer.read<float>();
		EXPECT_EQ(result, -333.0f);
		EXPECT_FALSE(buffer.hasMoreToRead());
	}
	

	// 2. Write and read once (struct)
	{
		ByteBuffer buffer;
		buffer.write(TestObject{1, 2.3f});

		const auto result = buffer.read<TestObject>();
		EXPECT_EQ(result.a, 1);
		EXPECT_EQ(result.b, 2.3f);
		EXPECT_FALSE(buffer.hasMoreToRead());
	}
	
	// 3. Read and write arrays
	{
		using DatumType = double;
		constexpr std::size_t ARRAY_SIZE = 100;

		std::array<DatumType, ARRAY_SIZE> data;
		for(std::size_t i = 0; i < ARRAY_SIZE; ++i)
		{
			data[i] = -static_cast<double>(i);
		}

		ByteBuffer buffer;
		buffer.write(data.data(), ARRAY_SIZE);

		std::array<DatumType, ARRAY_SIZE> result;
		buffer.read(result.data(), ARRAY_SIZE);
		for(std::size_t i = 0; i < ARRAY_SIZE; ++i)
		{
			EXPECT_EQ(result[i], -static_cast<double>(i));
		}

		EXPECT_FALSE(buffer.hasMoreToRead());
	}

	// 4. Read and write multiple times (struct)
	{
		const std::size_t NUM_WRITES = 1000;

		ByteBuffer buffer;
		for(std::size_t i = 0; i < NUM_WRITES; ++i)
		{
			const int   valueA = static_cast<int>(i);
			const float valueB = static_cast<float>(i * 2);
			buffer.write(TestObject{valueA, valueB});
		}

		for(std::size_t i = 0; i < NUM_WRITES; ++i)
		{
			const auto  result = buffer.read<TestObject>();
			const int   valueA = static_cast<int>(i);
			const float valueB = static_cast<float>(i * 2);
			EXPECT_EQ(result.a, valueA);
			EXPECT_EQ(result.b, valueB);
		}

		EXPECT_FALSE(buffer.hasMoreToRead());
	}
}

TEST(ByteBufferTest, Clearing)
{
	{
		using DatumType = int;
		constexpr std::size_t ARRAY_SIZE = 128;
		constexpr std::size_t NUM_ARRAY_BYTES = sizeof(DatumType) * ARRAY_SIZE;

		ByteBuffer buffer;
		buffer.write(std::array<DatumType, ARRAY_SIZE>().data(), ARRAY_SIZE);

		EXPECT_EQ(buffer.numBytes(), NUM_ARRAY_BYTES);
		EXPECT_EQ(buffer.getReadPosition(), 0);
		EXPECT_EQ(buffer.getWritePosition(), NUM_ARRAY_BYTES);

		buffer.clear();

		EXPECT_EQ(buffer.numBytes(), 0);
		EXPECT_EQ(buffer.getReadPosition(), 0);
		EXPECT_EQ(buffer.getWritePosition(), 0);
	}
}

TEST(ByteBufferTest, Reposition)
{
	// 1. Writing & rewind
	{
		ByteBuffer buffer;
		buffer.write(1);
		EXPECT_EQ(buffer.getWritePosition(), 4 * 1);

		buffer.rewindWrite();
		EXPECT_EQ(buffer.getWritePosition(), 0);

		buffer.write(1);
		buffer.write(1);
		buffer.write(1);
		EXPECT_EQ(buffer.getWritePosition(), 4 * 3);

		buffer.setWritePosition(7);
		EXPECT_EQ(buffer.getWritePosition(), 7);

		buffer.rewindWrite();
		EXPECT_EQ(buffer.getWritePosition(), 0);
	}

	// 2. Reading & rewind
	{
		ByteBuffer buffer;
		buffer.write(1);
		EXPECT_EQ(buffer.getReadPosition(), 0);

		buffer.read<int>();
		EXPECT_EQ(buffer.getReadPosition(), 4 * 1);

		buffer.rewindRead();
		EXPECT_EQ(buffer.getReadPosition(), 0);

		buffer.write(1);
		buffer.write(1);

		buffer.read<int>();
		buffer.read<int>();
		buffer.read<int>();
		EXPECT_EQ(buffer.getReadPosition(), 4 * 3);

		buffer.rewindRead();
		EXPECT_EQ(buffer.getReadPosition(), 0);
	}

	// 3. Reading and writing with specified positions
	{
		ByteBuffer buffer;
		buffer.write(1);
		buffer.write(1);
		buffer.write(1);
		EXPECT_EQ(buffer.getReadPosition(), 0);
		EXPECT_EQ(buffer.getWritePosition(), 4 * 3);

		buffer.setReadPosition(7);
		buffer.setWritePosition(5);
		EXPECT_EQ(buffer.getReadPosition(), 7);
		EXPECT_EQ(buffer.getWritePosition(), 5);

		buffer.read<int>();
		buffer.write(1);
		EXPECT_EQ(buffer.getReadPosition(), 7 + 4 * 1);
		EXPECT_EQ(buffer.getWritePosition(), 5 + 4 * 1);

		buffer.setReadPosition(0);
		buffer.setWritePosition(0);
		EXPECT_EQ(buffer.getReadPosition(), 0);
		EXPECT_EQ(buffer.getWritePosition(), 0);

		buffer.setReadPosition(4 * 3);
		buffer.setWritePosition(4 * 3);
		EXPECT_EQ(buffer.getReadPosition(), 4 * 3);
		EXPECT_EQ(buffer.getWritePosition(), 4 * 3);
	}
}
