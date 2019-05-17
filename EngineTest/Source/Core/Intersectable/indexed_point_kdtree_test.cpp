#include <Core/Intersectable/IndexedKdtree/TIndexedPointKdtree.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <vector>
#include <algorithm>

TEST(TIndexedPointKdtree, RangeSearchPointsOnAxis)
{
	using namespace ph;

	// treat input points as on y-axis
	auto pointTo3D = [](const float& point)
	{
		return Vector3R(0, static_cast<real>(point), 0);
	};

	// points on x axis
	std::vector<float> points = {
		6, 1, 3, 9, 5, 2, 7, 6, 6, 6
	};
	
	auto tree = TIndexedPointKdtree<float, int, decltype(pointTo3D)>(1, pointTo3D);
	tree.build(std::move(points));

	std::vector<float> results;

	results.clear();
	tree.findWithinRange({0, 0, 0}, 0.5_r, results);
	EXPECT_EQ(results.size(), 0);

	results.clear();
	tree.findWithinRange({0, 1, 0}, 0.5_r, results);
	EXPECT_EQ(results.size(), 1);
	EXPECT_EQ(results[0], 1.0f);

	results.clear();
	tree.findWithinRange({0, 9, 0}, 0.5_r, results);
	EXPECT_EQ(results.size(), 1); 
	EXPECT_EQ(results[0], 9.0f);

	results.clear();
	tree.findWithinRange({0, 6, 0}, 0.5_r, results);
	EXPECT_EQ(results.size(), 4);
	EXPECT_EQ(results[0], 6.0f);
	EXPECT_EQ(results[1], 6.0f);
	EXPECT_EQ(results[2], 6.0f);
	EXPECT_EQ(results[3], 6.0f);
}

TEST(CenterKdtreeTest, RangeSearchCubeVertices)
{
	using namespace ph;

	// Item is already a point, just return itself as center
	auto trivialPointCalculator = [](const Vector3R& point)
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
	
	auto tree = TIndexedPointKdtree<Vector3R, int, decltype(trivialPointCalculator)>(2, trivialPointCalculator);
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