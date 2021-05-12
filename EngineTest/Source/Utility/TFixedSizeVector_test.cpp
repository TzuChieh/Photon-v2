#include <Utility/TFixedSizeVector.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TFixedSizeVectorTest, BasicConstruction)
{
	{
		TFixedSizeVector<int, 3> vec;
		EXPECT_TRUE(vec.isEmpty());
	}
	
	{
		TFixedSizeVector<float, 1> vec;
		EXPECT_TRUE(vec.isEmpty());
	}

	{
		TFixedSizeVector<double*, 10> vec;
		EXPECT_TRUE(vec.isEmpty());
	}

	{
		TFixedSizeVector<int, 2> vecA;
		vecA[0] = 1;
		vecA[1] = 2;

		auto vecB = vecA;

		EXPECT_EQ(vecA[0], vecB[0]);
		EXPECT_EQ(vecA[1], vecB[1]);
	}
}

TEST(TFixedSizeVectorTest, PushAndPop)
{
	{
		ph::TFixedSizeVector<int, 3> vec;
		EXPECT_EQ(vec.size(), 0);
		EXPECT_TRUE(vec.isEmpty());

		vec.pushBack(-7);
		// vec state: [-7]
		EXPECT_EQ(vec.front(), -7);
		EXPECT_EQ(vec.size(), 1);
		EXPECT_FALSE(vec.isEmpty());

		vec.push(0);
		// vec state: [-7, 0]
		EXPECT_EQ(vec.back(), 0);
		EXPECT_EQ(vec.size(), 2);
		EXPECT_FALSE(vec.isEmpty());

		vec.pop();
		// vec state: [-7]
		EXPECT_EQ(vec.back(), -7);
		EXPECT_EQ(vec.size(), 1);
		EXPECT_FALSE(vec.isEmpty());

		vec.push(10);
		// vec state: [-7, 10]
		EXPECT_EQ(vec.back(), 10);
		EXPECT_EQ(vec.size(), 2);
		EXPECT_FALSE(vec.isEmpty());

		vec.push(-10);
		// vec state: [-7, 10, -10]
		EXPECT_EQ(vec.back(), -10);
		EXPECT_EQ(vec.size(), 3);
		EXPECT_FALSE(vec.isEmpty());

		// vec state: [-7, 10, -10]
		EXPECT_EQ(vec[0], -7);
		EXPECT_EQ(vec[1], 10);
		EXPECT_EQ(vec[2], -10);
		EXPECT_FALSE(vec.isEmpty());

		vec.clear();
		// vec state: []
		EXPECT_EQ(vec.size(), 0);
		EXPECT_TRUE(vec.isEmpty());
	}
}
