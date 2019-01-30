#include <Math/TArithmeticArray.h>

#include <gtest/gtest.h>

TEST(TArithmeticArrayTest, AddsRhsValue)
{
	const std::size_t size = 10;

	const ph::TArithmeticArray<int, size> array1(1);
	const ph::TArithmeticArray<int, size> array2(1);
	const auto addedArray1 = array1.add(array2);
	for(std::size_t i = 0; i < size; i++)
	{
		EXPECT_TRUE(addedArray1[i] == 2);
	}
	
	const ph::TArithmeticArray<int, size> array3(1);
	const auto addedArray2 = array3.add(2);
	for(std::size_t i = 0; i < size; i++)
	{
		EXPECT_TRUE(addedArray2[i] == 3);
	}

	const ph::TArithmeticArray<int, size> array4(2);
	const auto addedArray3 = array4 + 2;
	for(std::size_t i = 0; i < size; i++)
	{
		EXPECT_TRUE(addedArray3[i] == 4);
	}
}

TEST(TArithmeticArrayTest, FindsLargestElement)
{
	ph::TArithmeticArray<float, 5> array1;
	array1[0] = -3.0f;
	array1[1] =  0.0f;
	array1[2] = -1.3f;
	array1[3] =  9.9f;
	array1[4] =  2.0f;
	EXPECT_EQ(array1.max(), 9.9f);
}

TEST(TArithmeticArrayTest, PerElementAbsolute)
{
	ph::TArithmeticArray<float, 4> array1;
	array1[0] = -1.0f;
	array1[1] =  0.0f;
	array1[2] =  2.0f;
	array1[3] = -8.0f;

	const ph::TArithmeticArray<float, 4> absArray1 = array1.abs();
	EXPECT_EQ(absArray1[0], 1.0f);
	EXPECT_EQ(absArray1[1], 0.0f);
	EXPECT_EQ(absArray1[2], 2.0f);
	EXPECT_EQ(absArray1[3], 8.0f);
}