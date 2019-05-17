#include <Core/Intersectable/IndexedKdtree/TIndexedPointKdtree.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <vector>
#include <algorithm>

TEST(TIndexedPointKdtreeTest, RangeSearchPointsOnAxis)
{
	using namespace ph;

	// treat input points as on y-axis
	auto pointTo3D = [](const float& point)
	{
		return Vector3R(0, static_cast<real>(point), 0);
	};

	// points on x axis (with overlapping points)
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

TEST(TIndexedPointKdtreeTest, RangeSearchCubeVertices)
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
	tree.findWithinRange({1, 0, 0}, 1.001_r, results);
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

TEST(TIndexedPointKdtreeTest, FindNearestCubeVertices)
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
	tree.findNearest({1, 1, 1}, 1, results);
	EXPECT_EQ(results.size(), 1);
	EXPECT_TRUE(results[0].equals({1, 1, 1}));

	results.clear();
	tree.findNearest({0.01_r, 0.02_r, 0.03_r}, 1, results);
	EXPECT_EQ(results.size(), 1);
	EXPECT_TRUE(results[0].equals({0, 0, 0}));

	results.clear();
	tree.findNearest({0, 0, 0}, 4, results);
	EXPECT_EQ(results.size(), 4);
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 0, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(1, 0, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 1, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 0, 1)) != results.end());

	results.clear();
	tree.findNearest({100, -200, 900}, 1024, results);
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

TEST(TIndexedPointKdtreeTest, FindInAllOverlappingPoints)
{
	using namespace ph;

	// treat input points as on y-axis
	auto trivialPointCalculator = [](const Vector3R& point)
	{
		return point;
	};

	std::vector<Vector3R> points = {
		{0, 1, 0},
		{0, 1, 0},
		{0, 1, 0}
	};
	
	auto tree = TIndexedPointKdtree<Vector3R, int, decltype(trivialPointCalculator)>(1, trivialPointCalculator);
	tree.build(std::move(points));

	std::vector<Vector3R> results;

	// range search

	results.clear();
	tree.findWithinRange({0, 0, 0}, 1.001_r, results);
	EXPECT_EQ(results.size(), 3);
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 1, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 1, 0)) != results.end());
	EXPECT_TRUE(std::find(results.begin(), results.end(), Vector3R(0, 1, 0)) != results.end());

	results.clear();
	tree.findWithinRange({0, 0, 0}, 0.999_r, results);
	EXPECT_EQ(results.size(), 0);

	// nearest search

	results.clear();
	tree.findNearest({-123, -456, 789}, 1, results);
	EXPECT_EQ(results.size(), 1);
	EXPECT_TRUE(results[0].equals({0, 1, 0}));

	results.clear();
	tree.findNearest({-246, 357, 147}, 2, results);
	EXPECT_EQ(results.size(), 2);
	EXPECT_TRUE(results[0].equals({0, 1, 0}));
	EXPECT_TRUE(results[1].equals({0, 1, 0}));

	results.clear();
	tree.findNearest({1357, 321, 555}, 3, results);
	EXPECT_EQ(results.size(), 3);
	EXPECT_TRUE(results[0].equals({0, 1, 0}));
	EXPECT_TRUE(results[1].equals({0, 1, 0}));
	EXPECT_TRUE(results[2].equals({0, 1, 0}));
}