#include "config.h"

#include <DataIO/Stream/FormattedTextInputStream.h>
#include <Common/os.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(FormattedTextInputStreamTest, StringStreamReadAll)
{
	{
		auto stream = FormattedTextInputStream("123456");
		
		std::string content;
		stream.readAllTightly(&content);
		EXPECT_STREQ(content.c_str(), "123456");
	}

	{
		auto stream = FormattedTextInputStream("abc de fg hijk");

		std::string content;
		stream.readAllTightly(&content);
		EXPECT_STREQ(content.c_str(), "abcdefghijk");
	}

	{
		auto stream = FormattedTextInputStream(
			"0, \n"
			"2, \r\n"
			"4, \t\t\r6, 8, 10\n"
			"\n"
			"");

		std::string content;
		stream.readAllTightly(&content);
		EXPECT_STREQ(content.c_str(), "0,2,4,6,8,10");
	}
}

TEST(FormattedTextInputStreamTest, StringStreamReadLine)
{
	{
		auto stream = FormattedTextInputStream(
			".\n"
			"..\n"
			"...");

		std::string line;
		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), ".");

		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "..");

		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "...");

		ASSERT_FALSE(stream.readLineTightly(&line));
	}

	{
		auto stream = FormattedTextInputStream(
			"\n"
			"\n"
			" x \n"
			"y\r\n"
			"\tz\t\n"
			"");

		std::string line;
		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "x");

		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "y");

		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "z");

		ASSERT_FALSE(stream.readLineTightly(&line));
	}

	{
		auto stream = FormattedTextInputStream(
			".\n"
			"..\r\n"
			"\r\r");

		std::string line;
		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), ".");

		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "..");

		ASSERT_FALSE(stream.readLineTightly(&line));
	}
}

TEST(FormattedTextInputStreamTest, FileStreamReadAll)
{
	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_text.txt")));

		std::string content;
		stream.readAllTightly(&content);
		EXPECT_STREQ(content.c_str(), "123456");
	}

	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		std::string content;
		stream.readAllTightly(&content);
		EXPECT_STREQ(content.c_str(), "vvvvvv");
	}
}

TEST(FormattedTextInputStreamTest, FileStreamReadLine)
{
	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_text.txt")));

		std::string line;
		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "123456");
	}

	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		std::string line;
		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "v");

		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "vv");

		ASSERT_TRUE(stream.readLineTightly(&line));
		EXPECT_STREQ(line.c_str(), "vvv");

		ASSERT_FALSE(stream.readLineTightly(&line));
	}
}

TEST(FormattedTextInputStreamTest, FileStreamReadByte)
{
	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		std::byte byte;
		ASSERT_TRUE(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'v'});

#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS
		// CRLF should be formatted to LF only
		ASSERT_TRUE(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'\n'});

		ASSERT_TRUE(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'v'});
#else
		ASSERT_TRUE(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'\r'});

		ASSERT_TRUE(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'\n'});

		ASSERT_TRUE(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'v'});
#endif
	}
}

TEST(FormattedTextInputStreamTest, SeekTellConsistency)
{
	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		EXPECT_EQ(stream.tellGet(), 0);

		stream.seekGet(5);
		EXPECT_EQ(stream.tellGet(), 5);

		// try multiple tells
		for(std::size_t i = 0; i < 10; ++i)
		{
			EXPECT_EQ(stream.tellGet(), 5);
		}

		// seek to EOF
		stream.seekGet(9);
		EXPECT_EQ(stream.tellGet(), 9);
	}
}

TEST(FormattedTextInputStreamTest, ValidityCheck)
{
	{
		auto stream = FormattedTextInputStream();
		EXPECT_FALSE(stream);
	}

	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/empty.txt")));
		EXPECT_TRUE(stream);
	}
}
