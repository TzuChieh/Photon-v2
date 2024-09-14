#include <Utility/TSortedVector.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <memory>

using namespace ph;

TEST(TSortedVectorTest, Creation)
{
	// Default creation
	{
		TSortedVector<int32> int32Vec;
		EXPECT_EQ(int32Vec.size(), 0);

		TSortedVector<int64> int64Vec;
		EXPECT_EQ(int64Vec.size(), 0);

		TSortedVector<uint32> uint32Vec;
		EXPECT_EQ(uint32Vec.size(), 0);

		TSortedVector<uint64> uint64Vec;
		EXPECT_EQ(uint64Vec.size(), 0);

		TSortedVector<std::unique_ptr<float>> uptrVec;
		EXPECT_EQ(uptrVec.size(), 0);

		TSortedVector<std::shared_ptr<std::vector<int>>> svptrVec;
		EXPECT_EQ(svptrVec.size(), 0);
	}

	// With initial capacity
	{
		TSortedVector<int32> int32Vec(22);
		EXPECT_EQ(int32Vec.size(), 0);

		TSortedVector<int64> int64Vec(44);
		EXPECT_EQ(int64Vec.size(), 0);

		TSortedVector<uint32> uint32Vec(66);
		EXPECT_EQ(uint32Vec.size(), 0);

		TSortedVector<uint64> uint64Vec(88);
		EXPECT_EQ(uint64Vec.size(), 0);

		TSortedVector<std::unique_ptr<float>> uptrVec(16);
		EXPECT_EQ(uptrVec.size(), 0);

		TSortedVector<std::shared_ptr<std::vector<int>>> svptrVec(32);
		EXPECT_EQ(svptrVec.size(), 0);
	}
}

TEST(TSortedVectorTest, AddValuesAndRead)
{
	{
		TSortedVector<float> vec;
		vec.addValue(-2.0f);
		vec.addValue(-1.0f);
		vec.addValue(0.0f);
		vec.addValue(10.0f);
		vec.addValue(20.0f);

		EXPECT_EQ(vec.size(), 5);
		EXPECT_EQ(*vec.indexOfValue(-2.0f), 0);
		EXPECT_EQ(*vec.indexOfValue(0.0f), 2);
		EXPECT_EQ(*vec.indexOfValue(10.0f), 3);
		EXPECT_TRUE(vec.indexOfValue(10.0f).has_value());
		EXPECT_FALSE(vec.indexOfValue(12345.0f).has_value());
	}

	// unique values
	{
		TSortedVector<int> vec;
		EXPECT_TRUE(vec.addUniqueValue(1));
		EXPECT_TRUE(vec.addUniqueValue(2));
		EXPECT_TRUE(vec.addUniqueValue(3));
		EXPECT_FALSE(vec.addUniqueValue(1));
		EXPECT_FALSE(vec.addUniqueValue(2));
		EXPECT_FALSE(vec.addUniqueValue(3));
		EXPECT_TRUE(vec.addUniqueValue(4));

		// should contain {1, 2, 3, 4}
		EXPECT_EQ(vec.size(), 4);

		EXPECT_TRUE(vec.hasValue(1));
		EXPECT_TRUE(vec.hasValue(2));
		EXPECT_TRUE(vec.hasValue(4));
		EXPECT_FALSE(vec.hasValue(12345));
	}

	// check values are sorted
	{
		TSortedVector<int> vec;
		vec.addValue(5);
		vec.addValue(4);
		vec.addValue(-7);
		vec.addValue(999);
		vec.addValue(0);

		EXPECT_EQ(vec.get(0), -7);
		EXPECT_EQ(vec.get(1), 0);
		EXPECT_EQ(vec.get(2), 4);
		EXPECT_EQ(vec.get(3), 5);
		EXPECT_EQ(vec.get(4), 999);
	}
}

TEST(TSortedVectorTest, AddAndRemoval)
{
	{
		TSortedVector<float> vec(10);
		EXPECT_EQ(vec.addValue(-2.0f), 0);
		EXPECT_EQ(vec.addValue(-1.0f), 1);
		EXPECT_EQ(vec.addValue( 0.0f), 2);
		EXPECT_EQ(vec.addValue(10.0f), 3);
		EXPECT_EQ(vec.addValue(20.0f), 4);

		EXPECT_EQ(vec.size(), 5);

		EXPECT_TRUE(vec.removeValue(-2.0f));
		EXPECT_FALSE(vec.removeValue(0.001f));

		// should contain {-1, 0, 10, 20}
		EXPECT_EQ(vec.size(), 4);

		// remove {0}
		EXPECT_EQ(vec.removeValueByIndex(1), 0.0f);

		// should contain {-1, 10, 20}
		EXPECT_EQ(vec.size(), 3);
		EXPECT_EQ(vec.get(0), -1.0f);
		EXPECT_EQ(vec.get(1), 10.0f);
		EXPECT_EQ(vec.get(2), 20.0f);

		vec.addValue(10.0f);
		vec.addValue(10.0f);

		// should contain {-1, 10, 10, 10, 20}
		EXPECT_EQ(vec.size(), 5);

		std::size_t numRemovedValues = 0;
		EXPECT_EQ(*vec.removeValues(10.0f, &numRemovedValues), 1);
		EXPECT_EQ(numRemovedValues, 3);

		// should contain {-1, 20}
		EXPECT_EQ(vec.size(), 2);
		EXPECT_EQ(vec.get(0), -1.0f);
		EXPECT_EQ(vec.get(1), 20.0f);

		EXPECT_TRUE(vec.removeValue(-1.0f));
		EXPECT_TRUE(vec.removeValue(20.0f));
		EXPECT_TRUE(vec.isEmpty());
	}
}
