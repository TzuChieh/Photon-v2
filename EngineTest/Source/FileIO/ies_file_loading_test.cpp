#include <FileIO/Data/IesFile.h>

#include <gtest/gtest.h>

TEST(IesFileLoadingTest, Parse1995Files)
{
	using namespace ph;

	const double acceptableError = 0.0001;

	///////////////////////////////////////////////////////////////////////////
	// file 1

	IesFile file1(Path("./Resource/Test/IES/1.ies"));
	ASSERT_TRUE(file1.load());
	
	EXPECT_TRUE(file1.getIesFileType() == IesFile::EIesFileType::LM_63_1995);
	EXPECT_STREQ(file1.getTestInfo().c_str(), " 3310_1 BY: ERCO / LUM650");
	EXPECT_STREQ(file1.getLuminaireManufacturer().c_str(), " ERCO Leuchten GmbH");
	EXPECT_STREQ(file1.getLuminaireCatalogNumber().c_str(), " 83479000");
	EXPECT_STREQ(file1.getLuminaireDescription().c_str(), " Panarc Corridor luminaire");
	EXPECT_STREQ(file1.getLampCatalogNumber().c_str(), " TC-F 36W");
	EXPECT_EQ(file1.getNumLamps(), 1);
	EXPECT_NEAR(file1.getLumensPerLamp(), 2700.0_r, acceptableError);
	EXPECT_NEAR(file1.getCandelaMultiplier(), 1.0_r, acceptableError);
	EXPECT_EQ(file1.getNumVerticalAngles(), 19);
	EXPECT_EQ(file1.getNumHorizontalAngles(), 7);

	const auto verticalAngles1   = file1.getVerticalAngles();
	const auto horizontalAngles1 = file1.getHorizontalAngles();
	const auto candelaValues1    = file1.getCandelaValues();
	ASSERT_EQ(verticalAngles1.size(),   file1.getNumVerticalAngles());
	ASSERT_EQ(horizontalAngles1.size(), file1.getNumHorizontalAngles());
	ASSERT_EQ(candelaValues1.size(),    file1.getNumHorizontalAngles());
	for(const auto& verticalValues : candelaValues1)
	{
		ASSERT_EQ(verticalValues.size(), file1.getNumVerticalAngles());
	}

	EXPECT_NEAR(candelaValues1[0][0], 197.8, acceptableError);
	EXPECT_NEAR(candelaValues1[0][1], 213.8, acceptableError);
	EXPECT_NEAR(candelaValues1[0][2], 275.2, acceptableError);
	EXPECT_NEAR(candelaValues1[0].back(), 1.1, acceptableError);

	EXPECT_NEAR(candelaValues1[2][0], 197.8, acceptableError);
	EXPECT_NEAR(candelaValues1[2][2], 250.0, acceptableError);
	EXPECT_NEAR(candelaValues1[2][5], 412.3, acceptableError);
	EXPECT_NEAR(candelaValues1[2].back(), 1.1, acceptableError);

	EXPECT_NEAR(candelaValues1.back()[0], 197.8, acceptableError);
	EXPECT_NEAR(candelaValues1.back()[1], 197.4, acceptableError);
	EXPECT_NEAR(candelaValues1.back()[3], 193.2, acceptableError);
	EXPECT_NEAR(candelaValues1.back().back(), 0.0, acceptableError);

	///////////////////////////////////////////////////////////////////////////
	// file 2

	IesFile file2(Path("./Resource/Test/IES/7.ies"));
	ASSERT_TRUE(file2.load());
	
	EXPECT_TRUE(file2.getIesFileType() == IesFile::EIesFileType::LM_63_1995);
	EXPECT_EQ(file2.getNumLamps(), 1);
	EXPECT_NEAR(file2.getLumensPerLamp(), 4450.0_r, acceptableError);
	EXPECT_NEAR(file2.getCandelaMultiplier(), 1.0_r, acceptableError);
	EXPECT_EQ(file2.getNumVerticalAngles(), 37);
	EXPECT_EQ(file2.getNumHorizontalAngles(), 7);

	const auto verticalAngles2   = file2.getVerticalAngles();
	const auto horizontalAngles2 = file2.getHorizontalAngles();
	const auto candelaValues2    = file2.getCandelaValues();
	ASSERT_EQ(verticalAngles2.size(),   file2.getNumVerticalAngles());
	ASSERT_EQ(horizontalAngles2.size(), file2.getNumHorizontalAngles());
	ASSERT_EQ(candelaValues2.size(),    file2.getNumHorizontalAngles());
	for(const auto& verticalValues : candelaValues2)
	{
		ASSERT_EQ(verticalValues.size(), file2.getNumVerticalAngles());
	}

	EXPECT_NEAR(candelaValues2[0][0], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2[0][1], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2[0][2], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2[0].back(), 1054.2, acceptableError);

	EXPECT_NEAR(candelaValues2[2][0], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2[2][2], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2[2][5], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2[2].back(), 1054.2, acceptableError);

	EXPECT_NEAR(candelaValues2.back()[0], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2.back()[1], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2.back()[3], 0.0, acceptableError);
	EXPECT_NEAR(candelaValues2.back().back(), 1054.2, acceptableError);

	///////////////////////////////////////////////////////////////////////////
	// file 3

	IesFile file3(Path("./Resource/Test/IES/L1768-65BR30FL_FL55.ies"));
	ASSERT_TRUE(file3.load());
	
	EXPECT_TRUE(file3.getIesFileType() == IesFile::EIesFileType::LM_63_1995);
	EXPECT_EQ(file3.getNumLamps(), 1);
	EXPECT_NEAR(file3.getLumensPerLamp(), 635.0_r, acceptableError);
	EXPECT_NEAR(file3.getCandelaMultiplier(), 0.1_r, acceptableError);
	EXPECT_EQ(file3.getNumVerticalAngles(), 73);
	EXPECT_EQ(file3.getNumHorizontalAngles(), 9);

	const auto verticalAngles3   = file3.getVerticalAngles();
	const auto horizontalAngles3 = file3.getHorizontalAngles();
	const auto candelaValues3    = file3.getCandelaValues();
	ASSERT_EQ(verticalAngles3.size(),   file3.getNumVerticalAngles());
	ASSERT_EQ(horizontalAngles3.size(), file3.getNumHorizontalAngles());
	ASSERT_EQ(candelaValues3.size(),    file3.getNumHorizontalAngles());
	for(const auto& verticalValues : candelaValues3)
	{
		ASSERT_EQ(verticalValues.size(), file3.getNumVerticalAngles());
	}

	EXPECT_NEAR(candelaValues3[0][0], 5453.0, acceptableError);
	EXPECT_NEAR(candelaValues3[0][1], 5669.0, acceptableError);
	EXPECT_NEAR(candelaValues3[0][2], 5798.0, acceptableError);
	EXPECT_NEAR(candelaValues3[0].back(), 0.0, acceptableError);

	EXPECT_NEAR(candelaValues3[2][0], 5453.0, acceptableError);
	EXPECT_NEAR(candelaValues3[2][2], 5640.0, acceptableError);
	EXPECT_NEAR(candelaValues3[2][5], 5084.0, acceptableError);
	EXPECT_NEAR(candelaValues3[2].back(), 0.0, acceptableError);

	EXPECT_NEAR(candelaValues3.back()[0], 5453.0, acceptableError);
	EXPECT_NEAR(candelaValues3.back()[1], 5178.0, acceptableError);
	EXPECT_NEAR(candelaValues3.back()[3], 4400.0, acceptableError);
	EXPECT_NEAR(candelaValues3.back().back(), 0.0, acceptableError);
}