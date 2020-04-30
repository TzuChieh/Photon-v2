#include "constants_for_test.h"

#include <Core/Filmic/HdrRgbFilm.h>
#include <Core/Filmic/SampleFilters.h>

#include <gtest/gtest.h>

#include <memory>

using namespace ph;
using namespace ph::math;

TEST(HdrRgbFilmTest, CorrectRasterCoordinates)
{
	// trial 1: full size effective window

	const int64 filmWpx = 1;
	const int64 filmHpx = 2;

	const auto& filter = SampleFilters::createBoxFilter();
	const auto& film = HdrRgbFilm(static_cast<uint64>(filmWpx), 
	                              static_cast<uint64>(filmHpx), 
	                              filter);

	EXPECT_EQ(film.getActualResPx().x, filmWpx);
	EXPECT_EQ(film.getActualResPx().y, filmHpx);

	EXPECT_EQ(film.getEffectiveResPx().x, filmWpx);
	EXPECT_EQ(film.getEffectiveResPx().y, filmHpx);

	EXPECT_NEAR(film.getSampleResPx().x, 
	            static_cast<float64>(filmWpx) - 2 * 0.5 + filter.getSizePx().x, 
	            TEST_FLOAT64_EPSILON);
	EXPECT_NEAR(film.getSampleResPx().y, 
	            static_cast<float64>(filmHpx) - 2 * 0.5 + filter.getSizePx().y, 
	            TEST_FLOAT64_EPSILON);

	EXPECT_EQ(film.getEffectiveWindowPx().getMinVertex().x, 0);
	EXPECT_EQ(film.getEffectiveWindowPx().getMinVertex().y, 0);
	EXPECT_EQ(film.getEffectiveWindowPx().getMaxVertex().x, filmWpx);
	EXPECT_EQ(film.getEffectiveWindowPx().getMaxVertex().y, filmHpx);

	EXPECT_NEAR(film.getSampleWindowPx().getMinVertex().x,
	            0.0 + 0.5 - filter.getSizePx().x / 2.0,
	            TEST_FLOAT64_EPSILON);
	EXPECT_NEAR(film.getSampleWindowPx().getMinVertex().y,
	            0.0 + 0.5 - filter.getSizePx().y / 2.0,
	            TEST_FLOAT64_EPSILON);
	EXPECT_NEAR(film.getSampleWindowPx().getMaxVertex().x,
	            static_cast<float64>(filmWpx) - 0.5 + filter.getSizePx().x / 2.0,
	            TEST_FLOAT64_EPSILON);
	EXPECT_NEAR(film.getSampleWindowPx().getMaxVertex().y,
	            static_cast<float64>(filmHpx) - 0.5 + filter.getSizePx().y / 2.0,
	            TEST_FLOAT64_EPSILON);
}
