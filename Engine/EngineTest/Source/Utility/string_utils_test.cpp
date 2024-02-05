#include <Common/Utility/string_utils.h>

#include <gtest/gtest.h>

#include <charconv>
#include <string>

using namespace ph;
using namespace ph::string_utils;

TEST(StringUtilsTest, NextToken)
{
	{
		EXPECT_TRUE(next_token("A B C") == "A");
		EXPECT_TRUE(next_token(" A B C") == "A");
		EXPECT_TRUE(next_token(" \t \n\r  \tA B C") == "A");
		EXPECT_TRUE(next_token("Z") == "Z");
		EXPECT_TRUE(next_token(" T ") == "T");
		EXPECT_TRUE(next_token(" ") == "");
		EXPECT_TRUE(next_token("") == "");
	}

	{
		std::string_view remainingStr;
		EXPECT_TRUE(next_token("A12 B345 C6789", &remainingStr) == "A12");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr) == "B345");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr) == "C6789");
	}

	{
		std::string_view remainingStr;
		EXPECT_TRUE(next_token("   \t\nA B \t C ", &remainingStr) == "A");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr) == "B");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr) == "C");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr) == "");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr) == "");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr) == "");
	}

	{
		constexpr std::string_view sep = ";, ";

		std::string_view remainingStr;
		EXPECT_TRUE(next_token("A, B, C,;;; D ", &remainingStr, sep) == "A");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr, sep) == "B");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr, sep) == "C");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr, sep) == "D");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr, sep) == "");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr, sep) == "");
		EXPECT_TRUE(next_token(remainingStr, &remainingStr, sep) == "");
	}
}

TEST(StringUtilsTest, StringifyIntAlphabetic)
{
	// Base 2 to 36, use `std::to_char()` to verify
	{
		std::string strResult;
		std::string strExpected;
		for(int base = 2; base < 36; ++base)
		{
			for(int value = -1000; value <= 1000; ++value)
			{
				strResult.resize(64);
				strResult.resize(stringify_int_alphabetic(
					value, strResult.data(), strResult.size(), base));
			
				strExpected.resize(64);
				std::to_chars_result result = std::to_chars(
					strExpected.data(), strExpected.data() + strExpected.size(), value, base);
				strExpected.resize(result.ptr - strExpected.data());

				EXPECT_STREQ(strResult.c_str(), strExpected.c_str());
			}
		}
	}

	// TODO: base 37 to 62
}
