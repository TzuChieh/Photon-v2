#include <Core/Intersectable/IndexedKdtree/TCenterKdtree.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <vector>
#include <algorithm>

TEST(CenterKdtreeTest, RangeSearch)
{
	using namespace ph;

	// Item is already a point, just return itself as center
	auto trivialCenterCalculator = [](const Vector3R& point)
	{
		return point;
	};

	// vertices of a unit cube
	std::vector<Vector3R> points = {
		{0, 0, 0},
		{1, 0, 0},
		{0, 1, 0},
		{1, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{0, 1, 1},
		{1, 1, 1},
	};
	
	auto tree = TCenterKdtree<Vector3R, int, decltype(trivialCenterCalculator)>(1, trivialCenterCalculator);
	tree.build(std::move(points));

	std::vector<Vector3R> results;

	results.clear();
	tree.findWithinRange({0, 0, 0}, 0.5_r, results);
	EXPECT_EQ(results.size(), 1);
	EXPECT_TRUE(results[0].equals({0, 0, 0}));

	results.clear();
	tree.findWithinRange({1, 1, 1}, 0.5_r, results);
	EXPECT_EQ(results.size(), 1);
	EXPECT_TRUE(results[0].equals({1, 1, 1}));

	results.clear();
	tree.findWithinRange({1, 0, 0}, 1.0_r, results);
	EXPECT_EQ(results.size(), 4);
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(1, 0, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 0, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(1, 1, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(1, 0, 1)) != results.end());

	results.clear();
	tree.findWithinRange({0.5_r, 0.5_r, 0.5_r}, 0.5_r, results);
	EXPECT_EQ(results.size(), 0);

	results.clear();
	tree.findWithinRange({-1, 0, 0}, 0.999_r, results);
	EXPECT_EQ(results.size(), 0);

	results.clear();
	tree.findWithinRange({-1, 0, 0}, 1.001_r, results);
	EXPECT_EQ(results.size(), 1);
	EXPECT_TRUE(results[0].equals({0, 0, 0}));

	results.clear();
	tree.findWithinRange({100, -200, 900}, 10000.0_r, results);
	EXPECT_EQ(results.size(), 8);
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 0, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(1, 0, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 1, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(1, 1, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 0, 1)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(1, 0, 1)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 1, 1)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(1, 1, 1)) != results.end());
}