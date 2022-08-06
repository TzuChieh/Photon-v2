#include <Utility/TSortedMap.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <memory>
#include <string>

using namespace ph;

TEST(TSortedMapTest, Creation)
{
	// Default creation
	{
		TSortedMap<int32, float64> primitiveMap;
		EXPECT_EQ(primitiveMap.size(), 0);

		TSortedMap<int64, std::string> stringMap;
		EXPECT_EQ(stringMap.size(), 0);

		TSortedMap<float, std::unique_ptr<int>> ptrMap;
		EXPECT_EQ(ptrMap.size(), 0);
	}

	// With initial capacity
	{
		TSortedMap<int32, float64> primitiveMap(33);
		EXPECT_EQ(primitiveMap.size(), 0);

		TSortedMap<int64, std::string> stringMap(25);
		EXPECT_EQ(stringMap.size(), 0);

		TSortedMap<float, std::unique_ptr<int>> ptrMap(120);
		EXPECT_EQ(ptrMap.size(), 0);
	}
}

TEST(TSortedMapTest, MapAndAccess)
{
	// non-unique mapping
	{
		TSortedMap<int, std::string> strMap;
		strMap.map(1, "one");
		strMap.map(1, "one");
		strMap.map(1, "one");
		strMap.map(2, "two");
		strMap.map(2, "two-second");
		strMap.map(8, "eight");

		EXPECT_EQ(strMap.size(), 6);
		EXPECT_STREQ(strMap.getValue(2, 0)->c_str(), "two-second");
		EXPECT_STREQ(strMap.getValue(2, 1)->c_str(), "two");
		EXPECT_STREQ(strMap.getValue(8)->c_str(), "eight");
		EXPECT_FALSE(strMap.getValue(7));
		EXPECT_TRUE(strMap.getValue(1));
		EXPECT_TRUE(strMap.getValue(1, 0));
		EXPECT_TRUE(strMap.getValue(1, 1));
		EXPECT_TRUE(strMap.getValue(1, 2));
		EXPECT_FALSE(strMap.getValue(1, 3));
		EXPECT_STREQ(strMap.getValue(1)->c_str(), "one");
	}

	// unique mapping
	{
		TSortedMap<int, std::string> strMap;
		EXPECT_TRUE(strMap.mapUnique(2, "two"));
		EXPECT_FALSE(strMap.mapUnique(2, "two two two"));
		EXPECT_FALSE(strMap.mapUnique(2, "two"));
		EXPECT_TRUE(strMap.mapUnique(8, "eight"));

		EXPECT_EQ(strMap.size(), 2);
		EXPECT_STREQ(strMap.getValue(2)->c_str(), "two");
		EXPECT_STREQ(strMap.getValue(8)->c_str(), "eight");
		EXPECT_FALSE(strMap.getValue(0));
		EXPECT_FALSE(strMap.getValue(2, 1));
		EXPECT_TRUE(strMap.getValue(8, 0));
		EXPECT_FALSE(strMap.getValue(8, 1));
	}

	// ranged-for
	{
		TSortedMap<int, int> intMap;
		for(int i = 0; i < 10; ++i)
		{
			intMap.map(i, i * 100);
		}

		EXPECT_EQ(intMap.size(), 10);
		for(int i = 0; i < 10; ++i)
		{
			ASSERT_TRUE(intMap.getValue(i));
			EXPECT_EQ(*intMap.getValue(i), i * 100);
		}
	}

	// modification
	{
		TSortedMap<int, int> intMap;

		intMap.map(1, 2);
		EXPECT_EQ(*intMap.getValue(1), 2);

		*intMap.getValue(1) = 3;
		EXPECT_EQ(*intMap.getValue(1), 3);
	}
}

TEST(TSortedMapTest, MapAndRemoval)
{
	// remove single value
	{
		TSortedMap<int, double> numMap;
		numMap.map(1, 1.1);
		numMap.map(2, 2.2);
		numMap.map(3, 3.3);
		EXPECT_EQ(numMap.size(), 3);

		EXPECT_FALSE(numMap.unmap(0));
		EXPECT_TRUE(numMap.unmap(1));
		EXPECT_TRUE(numMap.unmap(2));
		EXPECT_TRUE(numMap.unmap(3));
		EXPECT_FALSE(numMap.unmap(4));
		EXPECT_TRUE(numMap.isEmpty());
	}

	{
		TSortedMap<int, double> numMap;
		numMap.map(1, 1.1);
		numMap.map(1, 1.2);
		numMap.map(2, 2.2);
		numMap.map(3, 3.3);
		numMap.map(3, 3.4);
		numMap.map(3, 3.5);
		EXPECT_EQ(numMap.size(), 6);

		EXPECT_FALSE(numMap.unmapValues(0));
		EXPECT_TRUE(numMap.unmapValues(1));

		// should contain {{2, 2.2}, {3, 3.5}, {3, 3.4}, {3, 3.3}}
		EXPECT_EQ(numMap.size(), 4);
		EXPECT_EQ(numMap.get(0), 2.2);
		EXPECT_EQ(numMap.get(1), 3.5);
		EXPECT_EQ(numMap.get(2), 3.4);
		EXPECT_EQ(numMap.get(3), 3.3);

		EXPECT_TRUE(numMap.unmapValues(2));
		EXPECT_TRUE(numMap.unmapValues(3));
		EXPECT_FALSE(numMap.unmapValues(4));

		EXPECT_TRUE(numMap.isEmpty());
	}
}
