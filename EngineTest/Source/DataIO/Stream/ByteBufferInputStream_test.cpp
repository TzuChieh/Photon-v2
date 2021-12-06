#include <DataIO/Stream/ByteBufferInputStream.h>

#include <gtest/gtest.h>

#include <array>

using namespace ph;

TEST(ByteBufferInputStreamTest, Allocation)
{
	{
		ByteBufferInputStream stream;
		EXPECT_EQ(stream.numBufferBytes(), 0);
		EXPECT_FALSE(stream);
	}

	{
		ByteBufferInputStream stream(128);
		EXPECT_EQ(stream.numBufferBytes(), 128);
		EXPECT_TRUE(stream);
		EXPECT_EQ(stream.tellGet(), 0);
	}
}

TEST(ByteBufferInputStreamTest, ReadBytes)
{
	{
		std::byte rawData[256];
		for(std::size_t i = 0; i < 256; ++i)
		{
			rawData[i] = static_cast<std::byte>(i);
		}

		ByteBufferInputStream stream(rawData, 256);
		EXPECT_EQ(stream.numBufferBytes(), 256);
		EXPECT_TRUE(stream);
		EXPECT_EQ(stream.tellGet(), 0);

		for(std::size_t i = 0; i < 256; ++i)
		{
			std::byte b;
			EXPECT_NO_THROW(stream.read(1, &b));
			EXPECT_EQ(b, static_cast<std::byte>(i));
		}

		EXPECT_FALSE(stream);
		EXPECT_EQ(stream.tellGet(), 256);
	}
}

TEST(ByteBufferInputStreamTest, Seek)
{
	{
		std::byte rawData[256];
		for(std::size_t i = 0; i < 256; ++i)
		{
			rawData[i] = static_cast<std::byte>(i);
		}

		ByteBufferInputStream stream(rawData, 256);
		EXPECT_EQ(stream.numBufferBytes(), 256);
		EXPECT_TRUE(stream);
		EXPECT_EQ(stream.tellGet(), 0);

		std::byte b;

		EXPECT_NO_THROW(stream.seekGet(50));
		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(50));

		EXPECT_NO_THROW(stream.seekGet(142));
		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(142));

		EXPECT_NO_THROW(stream.seekGet(211));
		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(211));

		EXPECT_NO_THROW(stream.seekGet(255));
		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(255));

		EXPECT_NO_THROW(stream.seekGet(0));
		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(0));

		EXPECT_NO_THROW(stream.seekGet(78));
		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(78));

		// Now read would get value starting from 79

		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(79));

		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(80));

		EXPECT_NO_THROW(stream.read(1, &b));
		EXPECT_EQ(b, static_cast<std::byte>(81));
	}
}

TEST(ByteBufferInputStreamTest, ReadLines)
{
	{
		// Set to LF if the index is divisible by 10
		std::array<char, 50> chars;
		for(std::size_t i = 0; i < chars.size(); ++i)
		{
			chars[i] = i % 10 == 0 ? '\n' : static_cast<char>(i);
		}

		ByteBufferInputStream stream(chars.data(), chars.size());
		EXPECT_EQ(stream.numBufferBytes(), chars.size());
		EXPECT_TRUE(stream);
		EXPECT_EQ(stream.tellGet(), 0);

		// Contains 6 lines, ending at index = 0, 10, 20, 30, 40, and EOF
		for(std::size_t i = 0; i < 6; ++i)
		{
			std::string line;
			EXPECT_NO_THROW(stream.readLine(&line));

			for(std::size_t ci = 0; ci < line.size(); ++ci)
			{
				// i == 0 should never be reached--the first line should be an empty string
				ASSERT_GT(i, 0);

				const auto charValue = (i - 1) * 10 + (ci + 1);
				EXPECT_EQ(line[ci], charValue);
			}
		}

		EXPECT_FALSE(stream);
		EXPECT_EQ(stream.tellGet(), chars.size());
	}
}

