#include <FileIO/Data/IesFile.h>

#include <gtest/gtest.h>

TEST(IesFileLoadingTest, Parse1995Files)
{
	using namespace ph;

	IesFile file1(Path("C:/Users/OrangeDog/Desktop/IES58k/IES Lights/Best IES/1.ies"));
	ASSERT_TRUE(file1.load());
	
	EXPECT_TRUE(file1.getIesFileType() == IesFile::EIesFileType::LM_63_1995);
	EXPECT_STREQ(file1.getTestInfo().c_str(), " 3310_1 BY: ERCO / LUM650");
	EXPECT_STREQ(file1.getLuminaireManufacturer().c_str(), " ERCO Leuchten GmbH");
	EXPECT_STREQ(file1.getLuminaireCatalogNumber().c_str(), " 83479000");
	EXPECT_STREQ(file1.getLuminaireDescription().c_str(), " Panarc Corridor luminaire");
	EXPECT_STREQ(file1.getLampCatalogNumber().c_str(), " TC-F 36W");
	EXPECT_EQ(file1.getNumLamps(), 1);
	EXPECT_EQ(file1.getLumensPerLamp(), 2700.0_r);
	EXPECT_EQ(file1.getCandelaMultiplier(), 1.0_r);
	EXPECT_EQ(file1.getNumVerticalAngles(), 19);
	EXPECT_EQ(file1.getNumHorizontalAngles(), 7);
}