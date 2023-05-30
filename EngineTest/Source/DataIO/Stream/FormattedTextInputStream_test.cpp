#include "config.h"

#include <DataIO/Stream/FormattedTextInputStream.h>
#include <Common/os.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(FormattedTextInputStreamTest, StringStreamReadAll)
{
	// Read all--preserve all contents

	{
		auto stream = FormattedTextInputStream("123456");
		
		std::string content;
		ASSERT_NO_THROW(stream.readAll(&content));
		EXPECT_STREQ(content.c_str(), "123456");
		EXPECT_FALSE(stream);
	}

	{
		auto stream = FormattedTextInputStream("abc de fg hijk");

		std::string content;
		ASSERT_NO_THROW(stream.readAll(&content));
		EXPECT_STREQ(content.c_str(), "abc de fg hijk");
	}

	{
		auto stream = FormattedTextInputStream(
			"0, \n"
			"2, \r\n"
			"4, \t\t\r6, 8, 10\n"
			"\n");

		std::string content;
		ASSERT_NO_THROW(stream.readAll(&content));
		EXPECT_STREQ(content.c_str(), "0, \n"
		                              "2, \r\n"
		                              "4, \t\t\r6, 8, 10\n"
		                              "\n");
		EXPECT_FALSE(stream);
	}
}

TEST(FormattedTextInputStreamTest, StringStreamReadAllTightly)
{
	// Read all tightly--without whitespaces

	{
		auto stream = FormattedTextInputStream("123456");
		
		std::string content;
		ASSERT_NO_THROW(stream.readAllTightly(&content));
		EXPECT_STREQ(content.c_str(), "123456");
		EXPECT_FALSE(stream);
	}

	{
		auto stream = FormattedTextInputStream("abc de fg hijk");

		std::string content;
		ASSERT_NO_THROW(stream.readAllTightly(&content));
		EXPECT_STREQ(content.c_str(), "abcdefghijk");
	}

	{
		auto stream = FormattedTextInputStream(
			"0, \n"
			"2, \r\n"
			"4, \t\t\r6, 8, 10\n"
			"\n");

		std::string content;
		ASSERT_NO_THROW(stream.readAllTightly(&content));
		EXPECT_STREQ(content.c_str(), "0,2,4,6,8,10");
		EXPECT_FALSE(stream);
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

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), ".");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "..");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "...");

		EXPECT_FALSE(stream);
	}

	{
		auto stream = FormattedTextInputStream(
			"XXX\n"
			" X X  X \r\n"
			" X \t XX\t\n");

		std::string line;

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "XXX");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), " X X  X \r");// new line is always LF in code, no matter the OS

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), " X \t XX\t");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "");// there is a LF in the last line, so we can read another line fine

		EXPECT_FALSE(stream);
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

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), " x ");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "y\r");// new line is always LF in code, no matter the OS

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "\tz\t");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "");// there is a LF in the last line, so we can read another line fine

		EXPECT_FALSE(stream);
	}

	{
		auto stream = FormattedTextInputStream(
			".\n"
			"..\r\n"
			"\r\r");

		std::string line;
		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), ".");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "..\r");// new line is always LF in code, no matter the OS

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "\r\r");

		EXPECT_FALSE(stream);
	}
}

TEST(FormattedTextInputStreamTest, FileStreamReadAll)
{
	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_text.txt")));

		std::string content;
		ASSERT_NO_THROW(stream.readAllTightly(&content));
		EXPECT_STREQ(content.c_str(), "123456");
		EXPECT_FALSE(stream);
	}

	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		std::string content;
		ASSERT_NO_THROW(stream.readAllTightly(&content));
		EXPECT_STREQ(content.c_str(), "vvvvvv");
		EXPECT_FALSE(stream);
	}
}

TEST(FormattedTextInputStreamTest, FileStreamReadLine)
{
	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_text.txt")));

		std::string line;

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "123456");

		EXPECT_FALSE(stream);
	}

	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		std::string line;

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "v");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "vv");

		ASSERT_NO_THROW(stream.readLine(&line));
		EXPECT_STREQ(line.c_str(), "vvv");

		EXPECT_FALSE(stream);
	}
}

TEST(FormattedTextInputStreamTest, FileStreamReadByte)
{
	{
		auto stream = FormattedTextInputStream(Path(
			PH_TEST_RESOURCE_PATH("Text/simple_multi_line.txt")));

		std::byte byte;
		ASSERT_NO_THROW(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'v'});

#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS
		// CRLF should be formatted to LF only
		ASSERT_NO_THROW(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'\n'});

		ASSERT_NO_THROW(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'v'});
#else
		ASSERT_NO_THROW(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'\r'});

		ASSERT_NO_THROW(stream.read(1, &byte));
		EXPECT_EQ(byte, std::byte{'\n'});

		ASSERT_NO_THROW(stream.read(1, &byte));
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

		ASSERT_NO_THROW(stream.seekGet(5));
		EXPECT_EQ(stream.tellGet(), 5);

		// try multiple tells
		for(std::size_t i = 0; i < 10; ++i)
		{
			EXPECT_EQ(stream.tellGet(), 5);
		}

		// seek to EOF
		ASSERT_NO_THROW(stream.seekGet(9));
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
