#include <Math/Geometry/TLineSegment.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;
using ph::math::TLineSegment;

TEST(TLineSegmentTest, Requirements)
{
	EXPECT_TRUE(std::is_trivially_copyable_v<TLineSegment<real>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TLineSegment<float>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TLineSegment<double>>);
}
