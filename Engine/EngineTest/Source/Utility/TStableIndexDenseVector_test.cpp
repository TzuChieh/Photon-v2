#include <Utility/TStableIndexDenseVector.h>

#include <gtest/gtest.h>

#include <string>
#include <cstdint>
#include <cstddef>
#include <vector>

using namespace ph;

TEST(TStableIndexDenseVectorTest, CtorAndDtor)
{
	// Default index type
	{
		TStableIndexDenseVector<int> intVec;
		TStableIndexDenseVector<float> floatVec;
		TStableIndexDenseVector<double> doubleVec;
		TStableIndexDenseVector<std::string> stringVec;

		TStableIndexDenseVector<std::string> stringVec2(128);
	}

	// Explicit index type
	{
		TStableIndexDenseVector<int, std::uint32_t> intVec;
		TStableIndexDenseVector<float, std::int32_t> floatVec;
		TStableIndexDenseVector<double, std::int16_t> doubleVec;
		TStableIndexDenseVector<std::string, std::int8_t> stringVec;
		TStableIndexDenseVector<char, std::int64_t> charVec;
		TStableIndexDenseVector<bool, std::uint64_t> boolVec;

		TStableIndexDenseVector<bool, std::uint64_t> boolVec2(1024);
	}
}

TEST(TStableIndexDenseVectorTest, AddObject)
{
	// Simple add and test stable index
	{
		TStableIndexDenseVector<int, std::uint32_t> vec;
		std::vector<std::uint32_t> stableIndices;
		for(int i = 0; i < 1024; ++i)
		{
			stableIndices.push_back(vec.add(i));
		}

		ASSERT_EQ(vec.size(), stableIndices.size());

		for(int i = 0; i < 1024; ++i)
		{
			ASSERT_TRUE(vec.isStableIndexValid(stableIndices[i]));
			EXPECT_EQ(*vec.get(stableIndices[i]), i);
		}
	}
}

TEST(TStableIndexDenseVectorTest, AddAndRemoveObject)
{
	// Add and remove
	{
		TStableIndexDenseVector<std::string, std::int32_t> vec;
		std::vector<std::int32_t> stableIndices;

		// Add 100 initial numbers as strings
		for(int i = 0; i < 100; ++i)
		{
			stableIndices.push_back(vec.add(std::to_string(i)));
		}

		// Repeatedly remove and add the same number of numbers
		for(int currentCount = 10; currentCount <= 50; ++currentCount)
		{
			// Remove the last `currentCount` numbers
			for(int i = 0; i < currentCount; ++i)
			{
				auto stableIndex = stableIndices.back();
				stableIndices.pop_back();
				EXPECT_TRUE(vec.remove(stableIndex));
			}

			ASSERT_EQ(vec.size(), stableIndices.size());
			EXPECT_EQ(vec.size(), 100 - currentCount);

			// Test the value of remaining numbers
			for(int i = 0; i < 100 - currentCount; ++i)
			{
				ASSERT_TRUE(vec.isStableIndexValid(stableIndices[i]));
				EXPECT_STREQ(vec[stableIndices[i]].c_str(), std::to_string(i).c_str());
			}

			// Add `currentCount` numbers back as 0
			for(int i = 0; i < currentCount; ++i)
			{
				stableIndices.push_back(vec.add(std::to_string(0)));
			}
		}
		
		// The first 50 numbers should remain the same
		for(int i = 0; i < 50; ++i)
		{
			EXPECT_STREQ(vec[stableIndices[i]].c_str(), std::to_string(i).c_str());
		}

		// The last 50 numbers should all be 0
		for(int i = 50; i < 100; ++i)
		{
			EXPECT_STREQ(vec[stableIndices[i]].c_str(), std::to_string(0).c_str());
		}
	}
}
