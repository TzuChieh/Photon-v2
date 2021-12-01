#include "config.h"

#include <DataIO/Stream/BinaryFileInputStream.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(BinaryFileInputStreamTest, FileStreamReadData)
{
	{
		auto stream = BinaryFileInputStream(Path(
			PH_TEST_RESOURCE_PATH("Binary/test_binary_ascii.dat")));
		
		unsigned char ch;
		EXPECT_NO_THROW(stream.readData(&ch));
		EXPECT_EQ(ch, 't');

		EXPECT_NO_THROW(stream.readData(&ch));
		EXPECT_EQ(ch, 'e');

		stream.seekGet(10);
		EXPECT_NO_THROW(stream.readData(&ch));
		EXPECT_EQ(ch, 'y');

		EXPECT_NO_THROW(stream.readData(&ch));
		EXPECT_EQ(ch, ' ');

		EXPECT_NO_THROW(stream.readData(&ch));
		EXPECT_EQ(ch, 'm');
	}

	{
		auto stream = BinaryFileInputStream(Path(
			PH_TEST_RESOURCE_PATH("Binary/test_binary_hex.dat")));

		std::byte byte;
		EXPECT_NO_THROW(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{0xAA});

		EXPECT_NO_THROW(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{0xBB});

		EXPECT_NO_THROW(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{0xCC});
	}
}

TEST(BinaryFileInputStreamTest, SeekTellConsistency)
{
	{
		auto stream = BinaryFileInputStream(Path(
			PH_TEST_RESOURCE_PATH("Binary/test_binary_ascii.dat")));

		EXPECT_EQ(stream.tellGet(), 0);

		EXPECT_NO_THROW(stream.seekGet(7));
		EXPECT_EQ(stream.tellGet(), 7);

		// try multiple tells
		for(std::size_t i = 0; i < 10; ++i)
		{
			EXPECT_EQ(stream.tellGet(), 7);
		}

		// seek to EOF
		EXPECT_NO_THROW(stream.seekGet(24));
		EXPECT_EQ(stream.tellGet(), 24);
	}
}

TEST(BinaryFileInputStreamTest, ValidityCheck)
{
	{
		auto stream = BinaryFileInputStream();
		EXPECT_FALSE(stream);
	}

	{
		auto stream = BinaryFileInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/empty.txt")));
		EXPECT_TRUE(stream);
	}
}
