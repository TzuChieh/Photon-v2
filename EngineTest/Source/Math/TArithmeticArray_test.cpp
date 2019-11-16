#include <Math/TArithmeticArray.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;
using namespace ph::math;

TEST(TArithmeticArrayTest, Requirements)
{
	EXPECT_TRUE((std::is_trivially_copyable_v<TArithmeticArray<char, 1>>));
	EXPECT_TRUE((std::is_trivially_copyable_v<TArithmeticArray<int, 3>>));
	EXPECT_TRUE((std::is_trivially_copyable_v<TArithmeticArray<unsigned int, 5>>));
	EXPECT_TRUE((std::is_trivially_copyable_v<TArithmeticArray<float, 7>>));
	EXPECT_TRUE((std::is_trivially_copyable_v<TArithmeticArray<double, 9>>));
}

TEST(TArithmeticArrayTest, AddsRhsValue)
{
	constexpr std::size_t SIZE = 10;

	const TArithmeticArray<int, SIZE> array1(1);
	const TArithmeticArray<int, SIZE> array2(1);
	const auto addedArray1 = array1.add(array2);
	for(std::size_t i = 0; i < SIZE; i++)
	{
		EXPECT_EQ(addedArray1[i], 2);
	}
	
	const TArithmeticArray<int, SIZE> array3(1);
	const auto addedArray2 = array3.add(2);
	for(std::size_t i = 0; i < SIZE; i++)
	{
		EXPECT_EQ(addedArray2[i], 3);
	}

	const TArithmeticArray<int, SIZE> array4(2);
	const auto addedArray3 = array4 + 2;
	for(std::size_t i = 0; i < SIZE; i++)
	{
		EXPECT_EQ(addedArray3[i], 4);
	}
}

TEST(TArithmeticArrayTest, FindsLargestElement)
{
	TArithmeticArray<float, 5> array1;
	array1[0] = -3.0f;
	array1[1] =  0.0f;
	array1[2] = -1.3f;
	array1[3] =  9.9f;
	array1[4] =  2.0f;
	EXPECT_EQ(array1.max(), 9.9f);
}

TEST(TArithmeticArrayTest, PerElementAbsolute)
{
	TArithmeticArray<float, 4> array1;
	array1[0] = -1.0f;
	array1[1] =  0.0f;
	array1[2] =  2.0f;
	array1[3] = -8.0f;

	const TArithmeticArray<float, 4> absArray1 = array1.abs();
	EXPECT_EQ(absArray1[0], 1.0f);
	EXPECT_EQ(absArray1[1], 0.0f);
	EXPECT_EQ(absArray1[2], 2.0f);
	EXPECT_EQ(absArray1[3], 8.0f);
}

TEST(TArithmeticArrayTest, ToVector)
{
	TArithmeticArray<float, 4> array;
	array[0] = -1;
	array[1] = -2;
	array[2] = -3;
	array[3] = -4;

	const auto vector = array.toVector();
	ASSERT_EQ(vector.size(), 7);
	EXPECT_EQ(vector[0], -1);
	EXPECT_EQ(vector[1], -2);
	EXPECT_EQ(vector[2], -3);
	EXPECT_EQ(vector[3], -4);
}
