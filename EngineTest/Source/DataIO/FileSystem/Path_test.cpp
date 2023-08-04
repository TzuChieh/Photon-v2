#include <DataIO/FileSystem/Path.h>
#include <Common/os.h>

#include <gtest/gtest.h>

#include <array>

using namespace ph;

TEST(FileSystemPathTest, PathOperation)
{
	const Path relativePath("./some/path");
	EXPECT_TRUE(relativePath.isRelative());
	EXPECT_FALSE(relativePath.isAbsolute());

#if PH_OPERATING_SYSTEM_IS_WINDOWS
	const Path absolutePath("D:/some/path");
#else
	const Path absolutePath("/some/path");
#endif
	EXPECT_FALSE(absolutePath.isRelative());
	EXPECT_TRUE(absolutePath.isAbsolute());

	const Path leadingSeparatorPath("/some/path/");
	EXPECT_STREQ(
		leadingSeparatorPath.removeLeadingSeparator().toString().c_str(), 
		Path("some/path/").toString().c_str());

	const Path trailingSeparatorPath("/some/path/");
	EXPECT_STREQ(
		trailingSeparatorPath.removeTrailingSeparator().toString().c_str(), 
		Path("/some/path").toString().c_str());

	const Path firstPath("C:/first/second/");
	const Path secondPath("/third/fourth/");
	const Path fullPath = firstPath.append(secondPath);
	EXPECT_STREQ(
		fullPath.toString().c_str(),
		Path("C:/first/second/third/fourth/").toString().c_str());
}

TEST(FileSystemPathTest, FileExtension)
{
	const Path pathWithExtension("/some/path/file.ext");
	EXPECT_STREQ(pathWithExtension.getExtension().c_str(), ".ext");

	const Path pathWithoutExtension("/some/path/file");
	EXPECT_STREQ(pathWithoutExtension.getExtension().c_str(), "");
}

TEST(FileSystemPathTest, LeadingAndTrailingElement)
{
	// Get leading element
	{
		Path path("./123/sdf/");
		EXPECT_STREQ(path.getLeadingElement().toString().c_str(), ".");

		Path path2("../123/sdf/");
		EXPECT_STREQ(path2.getLeadingElement().toString().c_str(), "..");

		Path path3("/abc/def/");
#if PH_OPERATING_SYSTEM_IS_WINDOWS
		EXPECT_STREQ(path3.getLeadingElement().toString().c_str(), "\\");
#else
		EXPECT_STREQ(path3.getLeadingElement().toString().c_str(), "/");
#endif

		Path path4("aaa/bbb/ccc/ddd/eee");
		EXPECT_STREQ(path4.getLeadingElement().toString().c_str(), "aaa");
	}

	// Get trailing element
	{
		Path path("./123/sdf");
		EXPECT_STREQ(path.getTrailingElement().toString().c_str(), "sdf");

		Path path2("./aaa/bbb.ext");
		EXPECT_STREQ(path2.getTrailingElement().toString().c_str(), "bbb.ext");

		// Default behavior ignores trailing separator
		Path path3("./aaa/bbb/ccc/");
		EXPECT_STREQ(path3.getTrailingElement().toString().c_str(), "ccc");

		// Do not ignore the trailing separator
		Path path4("./aaa/bbb/ccc/");
		EXPECT_STREQ(path4.getTrailingElement(false).toString().c_str(), "");
	}
}

TEST(FileSystemPathTest, GetParent)
{
	// Folder only
	{
		Path path("./aaa/bbb/ccc");
		Path result("./aaa/bbb");
		EXPECT_STREQ(path.getParent().toString().c_str(), result.toString().c_str());
	}

	// Folder only
	{
		Path path("aaa/bbb/ccc/ddd/.");
		Path result("aaa/bbb/ccc/ddd");
		EXPECT_STREQ(path.getParent().toString().c_str(), result.toString().c_str());
	}

	// Folder only (absolute path)
	{
		Path path("/aaa");
		Path result("/");
		EXPECT_STREQ(path.getParent().toString().c_str(), result.toString().c_str());
	}

	// File path
	{
		Path path("aaa/bbb/ccc.txt");
		Path result("aaa/bbb");
		EXPECT_STREQ(path.getParent().toString().c_str(), result.toString().c_str());
	}
}

TEST(FileSystemPathTest, ExtensionManipulation)
{
	{
		Path path("./aaa/bbb.txt");
		Path result("./aaa/bbb.wow");
		EXPECT_STREQ(path.replaceExtension(".wow").toString().c_str(), result.toString().c_str());
	}

	{
		Path path("./aaa/bbb.jpg");
		Path result("./aaa/bbb.wow");
		EXPECT_STREQ(path.replaceExtension("wow").toString().c_str(), result.toString().c_str());
	}

	{
		Path path("aaa/bbb.");
		Path result("aaa/bbb.wow");
		EXPECT_STREQ(path.replaceExtension("wow").toString().c_str(), result.toString().c_str());
	}

	{
		Path path("aaa/bbb");
		Path result("aaa/bbb.wow");
		EXPECT_STREQ(path.replaceExtension("wow").toString().c_str(), result.toString().c_str());
	}

	{
		Path path("aaa/bbb.jpg");
		Path result("aaa/bbb");
		EXPECT_STREQ(path.removeExtension().toString().c_str(), result.toString().c_str());
	}

	{
		Path path("aaa/bbb.");
		Path result("aaa/bbb");
		EXPECT_STREQ(path.removeExtension().toString().c_str(), result.toString().c_str());
	}

	{
		Path path("aaa/bbb");
		Path result("aaa/bbb");
		EXPECT_STREQ(path.removeExtension().toString().c_str(), result.toString().c_str());
	}

	// Corner case: current directory specifier
	{
		Path path("/path/to/current/.");
		Path result("/path/to/current/.");
		EXPECT_STREQ(path.removeExtension().toString().c_str(), result.toString().c_str());
	}
}

TEST(FileSystemPathTest, ToString)
{
	{
		const Path relativePath("./some/path");

		// Copied result includes null-terminator
		{
			std::array<char, 4> buffer;
			EXPECT_EQ(relativePath.toString(buffer), 4);
#if PH_OPERATING_SYSTEM_IS_WINDOWS
			EXPECT_STREQ(buffer.data(), ".\\s");
#else
			EXPECT_STREQ(buffer.data(), "./s");
#endif
		}
		
		// Copied result includes null-terminator
		{
			std::array<char, 12> buffer;
			EXPECT_EQ(relativePath.toString(buffer), 12);
#if PH_OPERATING_SYSTEM_IS_WINDOWS
			EXPECT_STREQ(buffer.data(), ".\\some\\path");
#else
			EXPECT_STREQ(buffer.data(), "./some/path");
#endif
		}

		// Copied result includes null-terminator
		{
			std::array<char, 6> buffer;
			EXPECT_EQ(relativePath.toString(buffer), 6);
#if PH_OPERATING_SYSTEM_IS_WINDOWS
			EXPECT_STREQ(buffer.data(), ".\\som");
#else
			EXPECT_STREQ(buffer.data(), "./som");
#endif
		}

		// Copied result excludes null-terminator
		{
			std::array<char, 11> buffer;
			EXPECT_EQ(relativePath.toString(buffer, nullptr, false), 11);

			std::string strBuffer(buffer.begin(), buffer.end());
#if PH_OPERATING_SYSTEM_IS_WINDOWS
			EXPECT_STREQ(strBuffer.c_str(), ".\\some\\path");
#else
			EXPECT_STREQ(strBuffer.c_str(), "./some/path");
#endif
		}

		// Copied result excludes null-terminator
		{
			std::array<char, 6> buffer;
			EXPECT_EQ(relativePath.toString(buffer, nullptr, false), 6);

			std::string strBuffer(buffer.begin(), buffer.end());
#if PH_OPERATING_SYSTEM_IS_WINDOWS
			EXPECT_STREQ(strBuffer.c_str(), ".\\some");
#else
			EXPECT_STREQ(strBuffer.c_str(), "./some");
#endif
		}
	}

	// Get total number of chars of a conversion
	{
		Path path("aaa/bbb/ccc/ddd/");

		// Null-terminated
		{
			std::array<char, 1> buffer;
			std::size_t numTotalChars;
			path.toString(buffer, &numTotalChars);
			EXPECT_EQ(numTotalChars, 16 + 1);
		}
		
		// Not null-terminated
		{
			std::array<char, 1> buffer;
			std::size_t numTotalChars;
			path.toString(buffer, &numTotalChars, false);
			EXPECT_EQ(numTotalChars, 16);
		}
	}
}
