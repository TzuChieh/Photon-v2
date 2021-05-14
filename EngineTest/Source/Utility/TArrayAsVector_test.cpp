#include <Utility/TArrayAsVector.h>

#include <gtest/gtest.h>

#include <memory>
#include <type_traits>

using namespace ph;

TEST(TArrayAsVectorTest, BasicConstruction)
{
	{
		TArrayAsVector<int, 3> vec;
		EXPECT_TRUE(vec.isEmpty());
	}
	
	{
		TArrayAsVector<float, 1> vec;
		EXPECT_TRUE(vec.isEmpty());
	}

	{
		TArrayAsVector<double*, 10> vec;
		EXPECT_TRUE(vec.isEmpty());
	}

	{
		TArrayAsVector<int, 2> vecA;
		vecA[0] = 1;
		vecA[1] = 2;

		auto vecB = vecA;

		EXPECT_EQ(vecA[0], vecB[0]);
		EXPECT_EQ(vecA[1], vecB[1]);
	}
}

TEST(TArrayAsVectorTest, SafeOutOfBoundAccessor)
{
	{
		TArrayAsVector<int, 2> vec;
		vec.pushBack(4);
		vec.pushBack(5);

		EXPECT_TRUE(vec.get(0));
		EXPECT_EQ(*(vec.get(0)), 4);

		EXPECT_TRUE(vec.get(1));
		EXPECT_EQ(*(vec.get(1)), 5);
	}

	{
		TArrayAsVector<int, 2> vec;
		EXPECT_FALSE(vec.get(0));
		EXPECT_FALSE(vec.get(1));
		EXPECT_FALSE(vec.get(100));
	}
}

TEST(TArrayAsVectorTest, PushAndPop)
{
	{
		ph::TArrayAsVector<int, 3> vec;
		EXPECT_EQ(vec.size(), 0);
		EXPECT_TRUE(vec.isEmpty());

		vec.pushBack(-7);
		// vec state: [-7]
		EXPECT_EQ(vec.front(), -7);
		EXPECT_EQ(vec.size(), 1);
		EXPECT_FALSE(vec.isEmpty());

		vec.pushBack(0);
		// vec state: [-7, 0]
		EXPECT_EQ(vec.back(), 0);
		EXPECT_EQ(vec.size(), 2);
		EXPECT_FALSE(vec.isEmpty());

		vec.popBack();
		// vec state: [-7]
		EXPECT_EQ(vec.back(), -7);
		EXPECT_EQ(vec.size(), 1);
		EXPECT_FALSE(vec.isEmpty());

		vec.pushBack(10);
		// vec state: [-7, 10]
		EXPECT_EQ(vec.back(), 10);
		EXPECT_EQ(vec.size(), 2);
		EXPECT_FALSE(vec.isEmpty());

		vec.pushBack(-10);
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

TEST(TArrayAsVectorTest, RequiredProperties)
{
	{
		using Vec = TArrayAsVector<char, 10>;
		EXPECT_TRUE(std::is_nothrow_move_constructible_v<Vec>);
	}

	{
		using Vec = TArrayAsVector<double, 10>;
		EXPECT_TRUE(std::is_nothrow_move_constructible_v<Vec>);
	}
	
	{
		using Vec = TArrayAsVector<std::unique_ptr<float>, 10>;
		EXPECT_TRUE(std::is_nothrow_move_constructible_v<Vec>);
	}
}
