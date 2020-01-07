#include "config.h"

#include <DataIO/Stream/FormattedTextInputStream.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(FormattedTextInputStreamTest, StringAsStreamReadAll)
{
	{
		auto stream = FormattedTextInputStream("123456");
		
		std::string content;
		stream.readAll(&content);
		EXPECT_STREQ(content.c_str(), "123456");
	}

	{
		auto stream = FormattedTextInputStream("abc de fg hijk");

		std::string content;
		stream.readAll(&content);
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
		stream.readAll(&content);
		EXPECT_STREQ(content.c_str(), "0,2,4,6,8,10");
	}
}

TEST(FormattedTextInputStreamTest, StringAsStreamReadLine)
{
	{
		auto stream = FormattedTextInputStream(
			".\n"
			"..\n"
			"...");

		std::string line;
		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), ".");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "..");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "...");

		ASSERT_FALSE(stream.readLine(&line));
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
		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), " x ");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "y\r");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "\tz\t");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "");

		ASSERT_FALSE(stream.readLine(&line));
	}

	{
		auto stream = FormattedTextInputStream(
			".\n"
			"..\r\n"
			"\r\r");

		std::string line;
		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), ".");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "..\r");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "\r\r");

		ASSERT_FALSE(stream.readLine(&line));
	}
}

TEST(FormattedTextInputStreamTest, FileAsStreamReadAll)
{
	{
		auto stream = FormattedTextInputStream(
			Path(PH_TEST_RESOURCE_PATH("Text/simple_text.txt")));

		std::string content;
		stream.readAll(&content);
		EXPECT_STREQ(content.c_str(), "123456");
	}

	{
		auto stream = FormattedTextInputStream(
			Path(PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		std::string content;
		stream.readAll(&content);
		EXPECT_STREQ(content.c_str(), "vvvvvv");
	}
}

TEST(FormattedTextInputStreamTest, FileAsStreamReadLine)
{
	{
		auto stream = FormattedTextInputStream(
			Path(PH_TEST_RESOURCE_PATH("Text/simple_text.txt")));

		std::string line;
		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "123456");
	}

	{
		auto stream = FormattedTextInputStream(
			Path(PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		std::string line;
		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "v");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "vv");

		ASSERT_TRUE(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "vvv");

		ASSERT_FALSE(stream.readLine(&line));
	}
}
