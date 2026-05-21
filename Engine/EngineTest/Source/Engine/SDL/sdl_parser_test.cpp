#include <Engine/SDL/sdl_parser.h>
#include <Engine/SDL/sdl_exceptions.h>

#include <gtest/gtest.h>

#include <string>
#include <string_view>

using namespace ph::sdl_parser;

TEST(SdlParserTest, SpecifierHelpers)
{
	EXPECT_TRUE(is_specifier(persistent_specifier));
	EXPECT_TRUE(is_specifier(cached_specifier));
	EXPECT_FALSE(is_specifier('\0'));
	EXPECT_FALSE(is_specifier('x'));

	EXPECT_TRUE(starts_with_specifier("@ref"));
	EXPECT_TRUE(starts_with_specifier("   $packet"));
	EXPECT_FALSE(starts_with_specifier(""));
	EXPECT_FALSE(starts_with_specifier("   name"));
}

TEST(SdlParserTest, TrimNameToken)
{
	{
		auto remaining = std::string_view("  \"abc def\"  tail");
		std::string_view name;

		remaining = trim_name(remaining, &name);
		EXPECT_EQ(name, "abc def");

		remaining = trim_name(remaining, &name);
		EXPECT_EQ(name, "tail");

		EXPECT_TRUE(remaining.empty());
	}

	{
		auto remaining = std::string_view("  foo  bar");
		std::string_view name;

		remaining = trim_name(remaining, &name);
		EXPECT_EQ(name, "foo");

		remaining = trim_name(remaining, &name);
		EXPECT_EQ(name, "bar");

		EXPECT_TRUE(remaining.empty());
	}

	{
		std::string_view name = "unchanged";
		const auto remaining = trim_name("   \t\r\n  ", &name);
		EXPECT_TRUE(remaining.empty());
		EXPECT_EQ(name, "unchanged");
	}

	{
		std::string_view name;
		EXPECT_THROW(trim_name("\"abc", &name), ph::SdlLoadError);
	}
}

TEST(SdlParserTest, TrimSpecifierName)
{
	{
		auto remaining = std::string_view("   $first   ");

		std::string_view name;
		remaining = trim_name(remaining, ESpecifier::Cached, &name);
		EXPECT_EQ(name, "first");
	}

	{
		auto remaining = std::string_view("  $ first  $\"second name\"  $ \"third name\"  ");
		std::string_view name;

		remaining = trim_name(remaining, ESpecifier::Cached, &name);
		EXPECT_EQ(name, "first");

		remaining = trim_name(remaining, ESpecifier::Cached, &name);
		EXPECT_EQ(name, "second name");

		remaining = trim_name(remaining, ESpecifier::Cached, &name);
		EXPECT_EQ(name, "third name");

		EXPECT_TRUE(remaining.empty());
	}

	{
		auto remaining = std::string_view("@left @right");
		std::string_view name;

		remaining = trim_name(remaining, ESpecifier::Persistent, &name);
		EXPECT_EQ(name, "left");

		remaining = trim_name(remaining, ESpecifier::Persistent, &name);
		EXPECT_EQ(name, "right");

		EXPECT_TRUE(remaining.empty());
	}

	{
		auto remaining = std::string_view("  @left @ right  ");
		std::string_view name;

		remaining = trim_name(remaining, ESpecifier::Persistent, &name);
		EXPECT_EQ(name, "left");

		remaining = trim_name(remaining, ESpecifier::Persistent, &name);
		EXPECT_EQ(name, "right");

		EXPECT_TRUE(remaining.empty());
	}
}

TEST(SdlParserTest, TrimSpecifierNameFailure)
{
	// Mismatched specifier
	{
		std::string_view name = "unchanged";
		const std::string_view input = "  @ref";
		const auto remaining = trim_name(input, ESpecifier::Cached, &name);
		EXPECT_EQ(remaining, "@ref");
		EXPECT_EQ(name, "unchanged");
	}

	// Syntax error: specifier must be followed by name
	{
		std::string_view name;
		EXPECT_THROW(trim_name("$", ESpecifier::Cached, &name), ph::SdlLoadError);
	}

	// Syntax error: specifier must be followed by name
	{
		std::string_view name;
		EXPECT_THROW(trim_name("$   ", ESpecifier::Cached, &name), ph::SdlLoadError);
	}

	// Syntax error: specifier must be followed by name
	{
		std::string_view name;
		EXPECT_THROW(trim_name("$\"\"", ESpecifier::Cached, &name), ph::SdlLoadError);
	}
}

TEST(SdlParserTest, TrimUtilsShouldRemoveWhitespaces)
{
	{
		const auto remaining = trim_specifier("   \t\r\n  ");
		EXPECT_TRUE(remaining.empty());
	}

	{
		std::string_view name;
		const auto remaining = trim_name("   \t\r\n  ", &name);
		EXPECT_TRUE(remaining.empty());
	}

	{
		std::string_view name;
		const auto remaining = trim_name("   \t\r\n  ", ESpecifier::Cached, &name);
		EXPECT_TRUE(remaining.empty());
	}
}

TEST(SdlParserTest, TrimSpecifierReportsTrimmedSpecifier)
{
	{
		ESpecifier trimmedSpecifier = ESpecifier::Persistent;
		const auto remaining = trim_specifier("$abc", &trimmedSpecifier);
		EXPECT_EQ(trimmedSpecifier, ESpecifier::Cached);
		EXPECT_EQ(remaining, "abc");
	}

	{
		ESpecifier trimmedSpecifier = ESpecifier::Persistent;
		const auto remaining = trim_specifier("abc", &trimmedSpecifier);
		EXPECT_EQ(trimmedSpecifier, ESpecifier::None);
		EXPECT_EQ(remaining, "abc");
	}
}

TEST(SdlParserTest, TrimSpecifierNoOpOnNonSpecifierHead)
{
	{
		ESpecifier trimmedSpecifier = ESpecifier::Cached;
		const auto remaining = trim_specifier("   name", &trimmedSpecifier);
		EXPECT_EQ(trimmedSpecifier, ESpecifier::None);
		EXPECT_EQ(remaining, "name");
	}

	{
		ESpecifier trimmedSpecifier = ESpecifier::Cached;
		const auto remaining = trim_specifier("   #name", &trimmedSpecifier);
		EXPECT_EQ(trimmedSpecifier, ESpecifier::None);
		EXPECT_EQ(remaining, "#name");
	}
}

TEST(SdlParserTest, SpecifierToEnumMapping)
{
	EXPECT_EQ(specifier_to_char(ESpecifier::None), '\0');
	EXPECT_EQ(specifier_to_char(ESpecifier::Persistent), persistent_specifier);
	EXPECT_EQ(specifier_to_char(ESpecifier::Cached), cached_specifier);
}

TEST(SdlParserTest, NullCharIsNotWhitespace)
{
	const std::string input("\0$abc", 5);
	const std::string_view inputView(input.data(), input.size());

	{
		ESpecifier trimmedSpecifier = ESpecifier::Persistent;
		const auto remaining = trim_specifier(inputView, &trimmedSpecifier);
		EXPECT_EQ(trimmedSpecifier, ESpecifier::None);
		EXPECT_EQ(remaining.size(), inputView.size());
		EXPECT_EQ(remaining.front(), '\0');
	}

	{
		std::string_view name = "unchanged";
		const auto remaining = trim_name(inputView, ESpecifier::Cached, &name);
		EXPECT_EQ(remaining.size(), inputView.size());
		EXPECT_EQ(remaining.front(), '\0');
		EXPECT_EQ(name, "unchanged");
	}
}
