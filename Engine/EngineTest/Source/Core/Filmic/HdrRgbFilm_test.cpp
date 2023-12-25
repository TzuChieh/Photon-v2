#include "constants_for_test.h"

#include <Core/Filmic/HdrRgbFilm.h>
#include <Core/Filmic/SampleFilter.h>
#include <Frame/TFrame.h>
#include <Math/Color/Spectrum.h>
#include <Math/Function/TLinearGradient2D.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

using namespace ph;
using namespace ph::math;

TEST(HdrRgbFilmTest, CorrectRasterCoordinates)
{
	// Full size effective window
	{
		const int64 filmWpx = 1;
		const int64 filmHpx = 2;

		const auto& filter = SampleFilter::makeBox();
		const auto& film = HdrRgbFilm(filmWpx, filmHpx, filter);

		EXPECT_EQ(film.getActualResPx().x(), filmWpx);
		EXPECT_EQ(film.getActualResPx().y(), filmHpx);

		EXPECT_EQ(film.getEffectiveResPx().x(), filmWpx);
		EXPECT_EQ(film.getEffectiveResPx().y(), filmHpx);

		EXPECT_NEAR(film.getSampleResPx().x(),
		            static_cast<float64>(filmWpx) - 2 * 0.5 + filter.getSizePx().x(),
		            TEST_FLOAT64_EPSILON);
		EXPECT_NEAR(film.getSampleResPx().y(),
		            static_cast<float64>(filmHpx) - 2 * 0.5 + filter.getSizePx().y(),
		            TEST_FLOAT64_EPSILON);

		EXPECT_EQ(film.getEffectiveWindowPx().getMinVertex().x(), 0);
		EXPECT_EQ(film.getEffectiveWindowPx().getMinVertex().y(), 0);
		EXPECT_EQ(film.getEffectiveWindowPx().getMaxVertex().x(), filmWpx);
		EXPECT_EQ(film.getEffectiveWindowPx().getMaxVertex().y(), filmHpx);

		EXPECT_NEAR(film.getSampleWindowPx().getMinVertex().x(),
		            0.0 + 0.5 - filter.getSizePx().x() / 2.0,
		            TEST_FLOAT64_EPSILON);
		EXPECT_NEAR(film.getSampleWindowPx().getMinVertex().y(),
		            0.0 + 0.5 - filter.getSizePx().y() / 2.0,
		            TEST_FLOAT64_EPSILON);
		EXPECT_NEAR(film.getSampleWindowPx().getMaxVertex().x(),
		            static_cast<float64>(filmWpx) - 0.5 + filter.getSizePx().x() / 2.0,
		            TEST_FLOAT64_EPSILON);
		EXPECT_NEAR(film.getSampleWindowPx().getMaxVertex().y(),
		            static_cast<float64>(filmHpx) - 0.5 + filter.getSizePx().y() / 2.0,
		            TEST_FLOAT64_EPSILON);
	}
	
}

TEST(HdrRgbFilmTest, DevelopesToFrame)
{
	// Symmetric filter
	{
		const int64 filmWpx = 1;
		const int64 filmHpx = 2;

		HdrRgbFrame frame(static_cast<uint32>(filmWpx), static_cast<uint32>(filmHpx));
		const auto& filter = SampleFilter::makeGaussian();
		HdrRgbFilm film(filmWpx, filmHpx, filter);

		const float64 testSamplePos1Xpx = film.getSampleWindowPx().getMinVertex().x() + 0.2;
		const float64 testSamplePos1Ypx = film.getSampleWindowPx().getMinVertex().y() + 0.2;
		const float64 testSamplePos2Xpx = film.getSampleWindowPx().getMinVertex().x() + 0.4;
		const float64 testSamplePos2Ypx = film.getSampleWindowPx().getMinVertex().y() + 0.4;
		film.addSample(testSamplePos1Xpx, testSamplePos1Ypx, Spectrum(0.7_r));
		film.addSample(testSamplePos2Xpx, testSamplePos2Ypx, Spectrum(0.3_r));
		film.develop(frame);

		HdrRgbFrame::PixelType pixel;
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

	// Asymmetric filter
	{
		const int64 filmWpx = 1;
		const int64 filmHpx = 2;

		HdrRgbFrame frame(static_cast<uint32>(filmWpx), static_cast<uint32>(filmHpx));

		const auto unitHorizontalGradient = TLinearGradient2D<float64>::makeHorizontal(1);
		HdrRgbFilm film(filmWpx, filmHpx, SampleFilter::make(unitHorizontalGradient, 1, 1));

		// TODO
	}
}

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
