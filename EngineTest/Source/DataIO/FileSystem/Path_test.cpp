#include <DataIO/FileSystem/Path.h>
#include <Common/os.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(FileSystemPathTest, PathOperation)
{
	const Path relativePath("./some/path");
	EXPECT_TRUE(relativePath.isRelative());
	EXPECT_FALSE(relativePath.isAbsolute());

#if defined(PH_OPERATING_SYSTEM_IS_WINDOWS)
	const Path absolutePath("D:/some/path");
#else
	const Path absolutePath("/some/path");
#endif
	EXPECT_FALSE(absolutePath.isRelative());
	EXPECT_TRUE(absolutePath.isAbsolute());

	const Path leadingSeparatorPath("/some/path/");
	EXPECT_TRUE(leadingSeparatorPath.removeLeadingSeparator() == Path("some/path/"));

	const Path trailingSeparatorPath("/some/path/");
	EXPECT_TRUE(trailingSeparatorPath.removeTrailingSeparator() == Path("/some/path"));

	const Path firstPath("C:/first/second/");
	const Path secondPath("/third/fourth/");
	const Path fullPath = firstPath.append(secondPath);
	EXPECT_TRUE(fullPath == Path("C:/first/second/third/fourth/"));
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
#if defined(PH_OPERATING_SYSTEM_IS_WINDOWS)
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
