#include "constants_for_test.h"

#include <Core/Filmic/HdrRgbFilm.h>
#include <Frame/TFrame.h>
#include <Core/Filmic/SampleFilters.h>
#include <Core/Quantity/SpectralStrength.h>
#include <Common/config.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ph;

#ifdef PH_RENDER_MODE_RGB

TEST(FilmSampleWeightingTest, HdrRgbFilmDevelopesToFrame)
{
	const int64 filmWpx = 1;
	const int64 filmHpx = 2;

	HdrRgbFrame frame(static_cast<uint32>(filmWpx), static_cast<uint32>(filmHpx));
	const auto& filter = SampleFilters::createGaussianFilter();
	HdrRgbFilm film(static_cast<uint64>(filmWpx), static_cast<uint64>(filmHpx), filter);

	const float64 testSamplePos1Xpx = film.getSampleWindowPx().minVertex.x + 0.2;
	const float64 testSamplePos1Ypx = film.getSampleWindowPx().minVertex.y + 0.2;
	const float64 testSamplePos2Xpx = film.getSampleWindowPx().minVertex.x + 0.4;
	const float64 testSamplePos2Ypx = film.getSampleWindowPx().minVertex.y + 0.4;
	film.addSample(testSamplePos1Xpx,
	               testSamplePos1Ypx,
	               SpectralStrength(0.7_r));
	film.addSample(testSamplePos2Xpx,
	               testSamplePos2Ypx,
	               SpectralStrength(0.3_r));
	film.develop(frame);

	HdrRgbFrame::Pixel pixel;
	frame.getPixel(0, 0, &pixel);

	// r, g, b should be equal - since the input samples are monochrome
	EXPECT_NEAR(pixel[0], pixel[1], TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[1], pixel[2], TEST_FLOAT32_EPSILON);

	// r, g, b should be non-zero
	EXPECT_TRUE(pixel[0] != 0 && pixel[1] != 0 && pixel[2] != 0);

	// predicting the pixel value
	const float64 pixelToSample1Xpx = 0 + 0.5 - testSamplePos1Xpx;
	const float64 pixelToSample1Ypx = 0 + 0.5 - testSamplePos1Ypx;
	const float64 pixelToSample2Xpx = 0 + 0.5 - testSamplePos2Xpx;
	const float64 pixelToSample2Ypx = 0 + 0.5 - testSamplePos2Ypx;
	const real weight1 = static_cast<real>(filter.evaluate(pixelToSample1Xpx, pixelToSample1Ypx));
	const real weight2 = static_cast<real>(filter.evaluate(pixelToSample2Xpx, pixelToSample2Ypx));

	EXPECT_TRUE(weight1 > 0.0_r);
	EXPECT_TRUE(weight2 > 0.0_r);
	EXPECT_TRUE(weight1 != weight2);
	EXPECT_NEAR(pixel[0],
	            (weight1 * 0.7_r + weight2 * 0.3_r) / (weight1 + weight2), 
	            TEST_REAL_EPSILON);
}

#endif

//class MockFrame : public Frame
//{
//public:
//	MOCK_METHOD2      (resize,             void(uint32 newWidthPx, uint32 newHeightPx));
//	MOCK_CONST_METHOD3(getPixel,           void(uint32 x, uint32 y, Vector3R* out_pixel));
//	MOCK_METHOD5      (setPixel,           void(uint32 x, uint32 y, real r, real g, real b));
//	MOCK_CONST_METHOD0(getWidthPx,         uint32());
//	MOCK_CONST_METHOD0(getHeightPx,        uint32());
//	MOCK_CONST_METHOD0(numPixelComponents, uint32());
//	MOCK_CONST_METHOD0(getPixelData,       const real*());
//
//public:
//	real r, g, b;
//
//private:
//
//};