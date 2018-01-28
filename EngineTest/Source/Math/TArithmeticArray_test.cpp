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