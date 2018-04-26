#include <FileIO/FileSystem/Path.h>
#include <Common/os.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(FileSystemPath, PathOperationTest)
{
	const ph::Path relativePath("./some/path");
	EXPECT_TRUE(relativePath.isRelative());
	EXPECT_FALSE(relativePath.isAbsolute());

#if defined(PH_OPERATING_SYSTEM_IS_WINDOWS)
	const ph::Path absolutePath("D:/some/path");
#elif defined(PH_OPERATING_SYSTEM_IS_LINUX)
	const ph::Path absolutePath("/some/path");
#endif
	EXPECT_FALSE(absolutePath.isRelative());
	EXPECT_TRUE(absolutePath.isAbsolute());

	const ph::Path leadingSeparatorPath("/some/path/");
	EXPECT_TRUE(leadingSeparatorPath.removeLeadingSeparator() == Path("some/path/"));

	const ph::Path trailingSeparatorPath("/some/path/");
	EXPECT_TRUE(trailingSeparatorPath.removeTrailingSeparator() == Path("/some/path"));

	const ph::Path firstPath("C:/first/second/");
	const ph::Path secondPath("/third/fourth/");
	const ph::Path fullPath = firstPath.append(secondPath);
	EXPECT_TRUE(fullPath == Path("C:/first/second/third/fourth/"));
}