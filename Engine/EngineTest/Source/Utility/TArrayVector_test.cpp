#include <Utility/TArrayVector.h>

#include <gtest/gtest.h>

#include <memory>
#include <type_traits>
#include <cstddef>

using namespace ph;

TEST(TArrayVectorTest, BasicConstruction)
{
	{
		TArrayVector<int, 3> vec;
		EXPECT_TRUE(vec.isEmpty());
	}
	
	{
		TArrayVector<float, 1> vec;
		EXPECT_TRUE(vec.isEmpty());
	}

	{
		TArrayVector<double*, 10> vec;
		EXPECT_TRUE(vec.isEmpty());
	}

	{
		TArrayVector<int, 2> vecA;
		vecA.pushBack(1);
		vecA.pushBack(2);

		auto vecB = vecA;

		EXPECT_EQ(vecA[0], vecB[0]);
		EXPECT_EQ(vecA[1], vecB[1]);
	}
}

TEST(TArrayVectorTest, SafeOutOfBoundAccessor)
{
	{
		TArrayVector<int, 2> vec;
		vec.pushBack(4);
		vec.pushBack(5);

		EXPECT_TRUE(vec.get(0));
		EXPECT_EQ(*(vec.get(0)), 4);

		EXPECT_TRUE(vec.get(1));
		EXPECT_EQ(*(vec.get(1)), 5);
	}

	{
		TArrayVector<int, 2> vec;
		EXPECT_FALSE(vec.get(0));
		EXPECT_FALSE(vec.get(1));
		EXPECT_FALSE(vec.get(100));

		for(std::size_t i = 0; i < 1000; ++i)
		{
			EXPECT_FALSE(vec.get(i));
		}
	}
}

TEST(TArrayVectorTest, PushAndPop)
{
	{
		TArrayVector<int, 3> vec;
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

	// Push lvalue
	{
		TArrayVector<float, 3> vec;

		float val = -3.0f;
		vec.pushBack(val);
		// vec state: [-3.0f]
		EXPECT_EQ(vec[0], -3.0f);

		val = 6.3f;
		vec.pushBack(val);
		// vec state: [-3.0f, 6.3f]
		EXPECT_EQ(vec[1], 6.3f);

		val = 0.111111f;
		vec.pushBack(val);
		// vec state: [-3.0f, 6.3f, 0.111111f]
		EXPECT_EQ(vec[2], 0.111111f);
	}
}

TEST(TArrayVectorTest, RequiredProperties)
{
	{
		using Vec = TArrayVector<char, 10>;
		EXPECT_TRUE(std::is_copy_constructible_v<Vec>);
		EXPECT_TRUE(std::is_move_constructible_v<Vec>);
		EXPECT_TRUE(std::is_copy_assignable_v<Vec>);
		EXPECT_TRUE(std::is_move_assignable_v<Vec>);
	}

	{
		using Vec = TArrayVector<double, 10>;
		EXPECT_TRUE(std::is_copy_constructible_v<Vec>);
		EXPECT_TRUE(std::is_move_constructible_v<Vec>);
		EXPECT_TRUE(std::is_copy_assignable_v<Vec>);
		EXPECT_TRUE(std::is_move_assignable_v<Vec>);
	}
	
	{
		using Vec = TArrayVector<std::unique_ptr<float>, 10>;
		EXPECT_FALSE(std::is_copy_constructible_v<Vec>);
		EXPECT_TRUE(std::is_move_constructible_v<Vec>);
		EXPECT_FALSE(std::is_copy_assignable_v<Vec>);
		EXPECT_TRUE(std::is_move_assignable_v<Vec>);
	}
}

TEST(TArrayVectorTest, PushNonCopyable)
{
	{
		TArrayVector<std::unique_ptr<int>, 3> vec;
		EXPECT_EQ(vec.size(), 0);
		EXPECT_TRUE(vec.isEmpty());

		vec.pushBack(std::make_unique<int>(100));
		EXPECT_EQ(*(vec.front()), 100);
		EXPECT_EQ(vec.size(), 1);
		EXPECT_FALSE(vec.isEmpty());

		vec.pushBack(std::make_unique<int>(10));
		EXPECT_EQ(*(vec.back()), 10);
		EXPECT_EQ(vec.size(), 2);
		EXPECT_FALSE(vec.isEmpty());
	}
}
