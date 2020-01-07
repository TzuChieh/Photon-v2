#include <DataIO/Stream/FormattedTextInputStream.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(FormattedTextInputStreamTest, StringAsStream)
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
		auto stream = FormattedTextInputStream("0, \n2, \r\n4, \t\t\r6, 8, 10\n\n");

		std::string content;
		stream.readAll(&content);
		EXPECT_STREQ(content.c_str(), "0,2,4,6,8,10");
	}
}
