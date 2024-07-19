#include "util.h"

#include <Math/Geometry/TLineSegment.h>
#include <Math/TVector3.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <cmath>

using namespace ph;
using ph::math::TLineSegment;

TEST(TLineSegmentTest, Requirements)
{
	EXPECT_TRUE(std::is_trivially_copyable_v<TLineSegment<real>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TLineSegment<float>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TLineSegment<double>>);
}

TEST(TLineSegmentTest, ParametricDistance)
{
	// Head and tail coordinates
	{
		const TLineSegment<real> line(math::Vector3R(0), math::Vector3R(1), 0, 1);
		EXPECT_EQ(line.getTail(), line.getOrigin()) << line.getTail().toString();
		EXPECT_EQ(line.getHead(), line.getDir()) << line.getHead().toString();
	}

	// Coordinates from parametric distance
	{
		const TLineSegment<real> line1(math::Vector3R(0, 0, 0), math::Vector3R(0, 0, 1), -1, 1);

		// In range
		PH_EXPECT_REAL_EQ(line1.getPoint(-0.5_r).z(), -0.5_r);
		PH_EXPECT_REAL_EQ(line1.getPoint(-0.123_r).z(), -0.123_r);
		PH_EXPECT_REAL_EQ(line1.getPoint(0.789_r).z(), 0.789_r);

		// Out of range
		PH_EXPECT_REAL_EQ(line1.getPoint(-1.5_r).z(), -1.5_r);
		PH_EXPECT_REAL_EQ(line1.getPoint(999.0_r).z(), 999.0_r);

		const TLineSegment<real> line2(math::Vector3R(0, 0, 1), math::Vector3R(0, 0, 1), 0, 1);
		
		// In range
		PH_EXPECT_REAL_EQ(line2.getPoint(0.5_r).z(), 1.5_r);
		PH_EXPECT_REAL_EQ(line2.getPoint(0.75_r).z(), 1.75_r);

		// Out of range
		PH_EXPECT_REAL_EQ(line2.getPoint(100.0_r).z(), 101.0_r);
	}

	// Parametric distance from coordinates: normalized direction
	{
		const TLineSegment<real> line(math::Vector3R(0, 0, 0), math::Vector3R(1, 0, 0), 0, 1);

		// In range
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(0, 0, 0)), 0.0_r);
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(0.5_r, 0, 0)), 0.5_r);
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(1, 0, 0)), 1.0_r);

		// Out of range
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(-100.0_r, 0, 0)), -100.0_r);
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(777.0_r, 0, 0)), 777.0_r);
	}

	// Parametric distance from coordinates: non-normalized direction
	{
		const TLineSegment<real> line(math::Vector3R(0, 0, 0), math::Vector3R(0, 2, 0), 0, 1);

		// In range
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(0, 0, 0)), 0.0_r);
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(0, 2.0_r, 0)), 1.0_r);
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(0, 4.0_r, 0)), 2.0_r);

		// Out of range
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(0, -200.0_r, 0)), -100.0_r);
		PH_EXPECT_REAL_EQ(line.getProjectedT(math::Vector3R(0, 468.0_r, 0)), 234.0_r);
	}

	// Parametric distance from coordinates: extreme coordinates can results in non-finite distance
	{
		const TLineSegment<real> line(math::Vector3R(1e9_r), math::Vector3R(1e3_r));

		const math::Vector3R head = line.getHead();
		EXPECT_TRUE(!head.isFinite()) << head.toString();

		EXPECT_TRUE(!std::isfinite(line.getProjectedT(head))) << line.getProjectedT(head);
	}
}
