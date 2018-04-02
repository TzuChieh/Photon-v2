#include <FileIO/TextFileLoader.h>

#include <gtest/gtest.h>

#include <string>

TEST(FileLoadingTest, LoadSimpleTextFile)
{
	std::string text1;
	bool isLoaded = ph::TextFileLoader::load(ph::Path("./Resource/Test/Text/simple_text.txt"), &text1);
	ASSERT_TRUE(isLoaded);
	EXPECT_STREQ(text1.c_str(), "123456");
}