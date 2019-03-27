#include "config.h"

#include <FileIO/Data/IesFile.h>
#include <FileIO/Data/IesData.h>
#include <FileIO/PictureSaver.h>
#include <Frame/TFrame.h>
#include <Math/math.h>

#include <gtest/gtest.h>

#include <cmath>

TEST(IesFileLoadingTest, Parse1995Files)
{
	using namespace ph;

	const double acceptableError = 0.0001;

	///////////////////////////////////////////////////////////////////////////
	// file 1

	IesFile file1(Path(PH_TEST_RESOURCE_PATH("IES/1.ies")));
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

	IesFile file2(Path(PH_TEST_RESOURCE_PATH("IES/7.ies")));
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

	IesFile file3(Path(PH_TEST_RESOURCE_PATH("IES/L1768-65BR30FL_FL55.ies")));
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

TEST(IesFileLoadingTest, DataProcessing)
{
	using namespace ph;

	const real PI               = std::atan(1.0_r) * 4.0_r;
	const real ACCEPTABLE_ERROR = 0.0001_r;

	// This file describes a light emitting 0.5 energy unit everywhere.
	//
	IesData iesData1(Path(PH_TEST_RESOURCE_PATH("IES/constant_point5_candela.IES")));

	// sampled degrees: 0, 90, 180
	//
	ASSERT_EQ(iesData1.numAttenuationFactorThetaSamples(), 3);

	// sampled degrees: 0, 180, 360
	//
	ASSERT_EQ(iesData1.numAttenuationFactorPhiSamples(), 3);

	EXPECT_NEAR(iesData1.sampleAttenuationFactor(0, 0), 1.0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(iesData1.sampleAttenuationFactor(PI, 0), 0.0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(iesData1.sampleAttenuationFactor(0, PI), 1.0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(iesData1.sampleAttenuationFactor(PI, PI), 0.0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(iesData1.sampleAttenuationFactor(2 * PI, 2 * PI), 0.0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(iesData1.sampleAttenuationFactor(2 * PI, 4 * PI), 0.0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(iesData1.sampleAttenuationFactor(-2 * PI, -4 * PI), 1.0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(iesData1.sampleAttenuationFactor(PI / 2, PI), 1.0, ACCEPTABLE_ERROR);

	// DEBUG

	//IesData iesData(Path("./Resource/Test/IES/L1768-65BR30FL_FL55.ies"));
	/*IesData iesData(Path("./Resource/Test/IES/9.ies"));
	LdrRgbFrame iesFrame(400, 200);
	for(std::size_t y = 0; y < iesFrame.heightPx(); y++)
	{
		for(std::size_t x = 0; x < iesFrame.widthPx(); x++)
		{
			const real u = (static_cast<real>(x) + 0.5_r) / static_cast<real>(iesFrame.widthPx());
			const real v = (static_cast<real>(y) + 0.5_r) / static_cast<real>(iesFrame.heightPx());
			const real phi = u * 2.0_r * PI;
			const real theta = (1.0_r - v) * PI;

			const real factor = iesData.sampleAttenuationFactor(theta, phi);
			const LdrComponent factor255 = static_cast<LdrComponent>(Math::clamp(factor * 255.0_r + 0.5_r, 0.0_r, 255.0_r));
			iesFrame.setPixel(x, y, LdrRgbFrame::Pixel({factor255, factor255, factor255}));
		}
	}
	PictureSaver::save(iesFrame, Path("./test_img.png"));*/
}