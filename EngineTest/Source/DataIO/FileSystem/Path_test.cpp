#include <DataIO/FileSystem/Path.h>
#include <Common/os.h>

#include <gtest/gtest.h>

#include <array>

using namespace ph;

TEST(FileSystemPathTest, PathOperation)
{
	{
		Path relativePath("./some/path");
		EXPECT_TRUE(relativePath.isRelative());
		EXPECT_FALSE(relativePath.isAbsolute());
	}

	{
#if PH_OPERATING_SYSTEM_IS_WINDOWS
		Path absolutePath("D:/some/path");
#else
		Path absolutePath("/some/path");
#endif
		EXPECT_FALSE(absolutePath.isRelative());
		EXPECT_TRUE(absolutePath.isAbsolute());
	}

	// Remove single leading separator
	{
		const Path leadingSeparatorPath("/some/path/");
		EXPECT_STREQ(
			leadingSeparatorPath.removeLeadingSeparator().toString().c_str(),
			Path("some/path/").toString().c_str());
	}
	
	// Remove single trailing separator
	{
		const Path trailingSeparatorPath("/some/path/");
		EXPECT_STREQ(
			trailingSeparatorPath.removeTrailingSeparator().toString().c_str(),
			Path("/some/path").toString().c_str());
	}

	// Remove multiple leading separators
	{
		const Path leadingSeparatorPath("/////sunny/day/");
		EXPECT_STREQ(
			leadingSeparatorPath.removeLeadingSeparator().toString().c_str(),
			Path("sunny/day/").toString().c_str());
	}

	// Remove multiple trailing separators
	{
		const Path trailingSeparatorPath("/snowy/winter/night/////");
		EXPECT_STREQ(
			trailingSeparatorPath.removeTrailingSeparator().toString().c_str(),
			Path("/snowy/winter/night").toString().c_str());
	}

	{
		const Path firstPath("C:/first/second/");
		const Path secondPath("/third/fourth/");
		const Path fullPath = firstPath.append(secondPath);
		EXPECT_STREQ(
			fullPath.toString().c_str(),
			Path("C:/first/second/third/fourth/").toString().c_str());
	}
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
		EXPECT_STREQ(path3.getLeadingElement().toString().c_str(), "/");

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

TEST(FileSystemPathTest, ToNativeString)
{
	{
		const Path relativePath("./some/path");

		// Copied result includes null-terminator
		{
			std::array<char, 4> buffer;
			EXPECT_EQ(relativePath.toNativeString(buffer), 4);
			EXPECT_STREQ(buffer.data(), "./s");
		}
		
		// Copied result includes null-terminator
		{
			std::array<char, 12> buffer;
			EXPECT_EQ(relativePath.toNativeString(buffer), 12);
			EXPECT_STREQ(buffer.data(), "./some/path");
		}

		// Copied result includes null-terminator
		{
			std::array<char, 6> buffer;
			EXPECT_EQ(relativePath.toNativeString(buffer), 6);
			EXPECT_STREQ(buffer.data(), "./som");
		}

		// Copied result excludes null-terminator
		{
			std::array<char, 11> buffer;
			EXPECT_EQ(relativePath.toNativeString(buffer, nullptr, false), 11);

			std::string strBuffer(buffer.begin(), buffer.end());
			EXPECT_STREQ(strBuffer.c_str(), "./some/path");
		}

		// Copied result excludes null-terminator
		{
			std::array<char, 6> buffer;
			EXPECT_EQ(relativePath.toNativeString(buffer, nullptr, false), 6);

			std::string strBuffer(buffer.begin(), buffer.end());
			EXPECT_STREQ(strBuffer.c_str(), "./some");
		}
	}

	// Get total number of chars of a conversion
	{
		Path path("aaa/bbb/ccc/ddd/");

		// Null-terminated
		{
			std::array<char, 1> buffer;
			std::size_t numTotalChars;
			path.toNativeString(buffer, &numTotalChars);
			EXPECT_EQ(numTotalChars, 16 + 1);
		}
		
		// Not null-terminated
		{
			std::array<char, 1> buffer;
			std::size_t numTotalChars;
			path.toNativeString(buffer, &numTotalChars, false);
			EXPECT_EQ(numTotalChars, 16);
		}
	}
}
