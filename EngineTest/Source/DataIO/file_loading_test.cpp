#include "config.h"

#include <DataIO/TextFileLoader.h>
#include <DataIO/BinaryFileReader.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ph;

TEST(FileLoadingTest, LoadSimpleTextFile)
{
	std::string text1;
	bool isLoaded = ph::TextFileLoader::load(ph::Path(PH_TEST_RESOURCE_PATH("Text/simple_text.txt")), &text1);
	ASSERT_TRUE(isLoaded);
	EXPECT_STREQ(text1.c_str(), "123456");
}

TEST(FileLoadingTest, LoadSimpleBinaryFile)
{
	BinaryFileReader reader1(Path(PH_TEST_RESOURCE_PATH("Binary/test_binary_ascii.dat")));
	ASSERT_TRUE(reader1.open());

	const std::string actualMessage("test binary message here");

	std::string buffer1;
	buffer1.resize(actualMessage.length());
	reader1.read(buffer1.data(), buffer1.length());
	EXPECT_STREQ(buffer1.c_str(), actualMessage.c_str());

	BinaryFileReader reader2(Path(PH_TEST_RESOURCE_PATH("Binary/test_binary_hex.dat")));
	ASSERT_TRUE(reader2.open());

	const std::vector<unsigned char> actualData{0xAA, 0xBB, 0xCC};

	std::vector<unsigned char> buffer2;
	buffer2.resize(actualData.size());
	reader2.read(buffer2.data(), buffer2.size());
	EXPECT_EQ(buffer2[0], actualData[0]);
	EXPECT_EQ(buffer2[1], actualData[1]);
	EXPECT_EQ(buffer2[2], actualData[2]);
}