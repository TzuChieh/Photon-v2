#include <RenderCore/Memory/HostMemoryBlock.h>

#include <gtest/gtest.h>

#include <utility>
#include <type_traits>
#include <limits>
#include <vector>

using namespace ph;
using namespace ph::editor;

TEST(HostMemoryBlockTest, ConstructAndMove)
{
	// Default construction--no storage
	{
		HostMemoryBlock block;
		EXPECT_EQ(block.numAllocatedBytes(), 0);
		EXPECT_EQ(block.numUsedBytes(), 0);
		EXPECT_EQ(block.numRemainingBytes(), 0);
	}

	// Basic construction with size hint
	{
		HostMemoryBlock block(123);
		EXPECT_GE(block.numAllocatedBytes(), 123);
		EXPECT_EQ(block.numUsedBytes(), 0);
		EXPECT_GE(block.numRemainingBytes(), 123);
	}

	// Basic construction with move
	{
		HostMemoryBlock block(1000);
		EXPECT_GE(block.numAllocatedBytes(), 1000);
		EXPECT_EQ(block.numUsedBytes(), 0);
		EXPECT_GE(block.numRemainingBytes(), 1000);

		HostMemoryBlock block2;
		EXPECT_GE(block2.numAllocatedBytes(), 0);
		EXPECT_EQ(block2.numUsedBytes(), 0);
		EXPECT_GE(block2.numRemainingBytes(), 0);

		block2 = std::move(block);

		EXPECT_GE(block.numAllocatedBytes(), 0);
		EXPECT_EQ(block.numUsedBytes(), 0);
		EXPECT_GE(block.numRemainingBytes(), 0);

		EXPECT_GE(block2.numAllocatedBytes(), 1000);
		EXPECT_EQ(block2.numUsedBytes(), 0);
		EXPECT_GE(block2.numRemainingBytes(), 1000);
	}

	// Disallow copying
	{
		static_assert(std::is_copy_constructible_v<HostMemoryBlock> == false);
		static_assert(std::is_copy_assignable_v<HostMemoryBlock> == false);
	}
}

TEST(HostMemoryBlockTest, ObjectAllocation)
{
	// Single `int`
	{
		HostMemoryBlock block(100);
		int* ptr = block.make<int>();// zero-init
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, 0);

		ptr = block.make<int>(777);
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, 777);

		ptr = block.make<int>(-999);
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, -999);

		ptr = block.make<int>(std::numeric_limits<int>::max());
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, std::numeric_limits<int>::max());

		ptr = block.make<int>(std::numeric_limits<int>::min());
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, std::numeric_limits<int>::min());
	}

	// Single `double`
	{
		HostMemoryBlock block(200);
		double* ptr = block.make<double>(3.0);
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, 3.0);

		ptr = block.make<double>(-345.678);
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, -345.678);

		ptr = block.make<double>(std::numeric_limits<double>::max());
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, std::numeric_limits<double>::max());

		ptr = block.make<double>(std::numeric_limits<double>::lowest());
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, std::numeric_limits<double>::lowest());
	}

	struct Test
	{
		int x = 1;
		double y = 2.0;
	};

	// Single object
	{
		HostMemoryBlock block(200);
		Test* ptr = block.make<Test>();
		ASSERT_TRUE(ptr);
		EXPECT_EQ(ptr->x, 1);
		EXPECT_EQ(ptr->y, 2.0);
	}

	// Multiple objects
	{
		constexpr int numObjs = 1000;

		HostMemoryBlock block(sizeof(Test) * numObjs);
		std::vector<Test*> ptrs;
		for(int i = 0; i < numObjs; ++i)
		{
			Test test;
			test.x = i;
			test.y = i;

			ptrs.push_back(block.make<Test>(test));
		}

		for(int i = 0; i < numObjs; ++i)
		{
			ASSERT_TRUE(ptrs[i]);
			EXPECT_EQ(ptrs[i]->x, i);
			EXPECT_EQ(ptrs[i]->y, i);
		}
	}
}

TEST(HostMemoryBlockTest, ArrayAllocation)
{
	// `int` array
	{
		constexpr int numInts = 100;

		HostMemoryBlock block(sizeof(int) * numInts);
		TSpan<int> ints = block.makeArray<int>(numInts);// zero-init all elements

		EXPECT_EQ(ints.size(), numInts);
		for(int i = 0; i < numInts; ++i)
		{
			EXPECT_EQ(ints[i], 0);
		}
	}

	// `double` array
	{
		constexpr int numDoubles = 1000;

		HostMemoryBlock block(sizeof(double) * numDoubles);
		TSpan<double> doubles = block.makeArray<double>(numDoubles);

		EXPECT_EQ(doubles.size(), numDoubles);
		for(int i = 0; i < numDoubles; ++i)
		{
			doubles[i] = i;
		}

		for(int i = 0; i < numDoubles; ++i)
		{
			EXPECT_EQ(doubles[i], i);
		}
	}

	struct Test
	{
		int x = 1;
		double y = 2.0;
	};

	// `Test` array
	{
		constexpr int numObjs = 1000;

		HostMemoryBlock block(sizeof(Test) * numObjs);
		TSpan<Test> objs = block.makeArray<Test>(numObjs);

		EXPECT_EQ(objs.size(), numObjs);
		for(int i = 0; i < numObjs; ++i)
		{
			EXPECT_EQ(objs[i].x, 1);
			EXPECT_EQ(objs[i].y, 2.0);

			// Write to objects
			objs[i].x = i;
			objs[i].y = i;
		}

		// See if our writes succeeded
		for(int i = 0; i < numObjs; ++i)
		{
			EXPECT_EQ(objs[i].x, i);
			EXPECT_EQ(objs[i].y, i);
		}
	}
}

TEST(HostMemoryBlockTest, ClearAndReuse)
{
	// `double` array
	{
		constexpr int testSize = 1000;
		constexpr int numClears = 10;

		HostMemoryBlock block(sizeof(double) * testSize);

		for(int clearCount = 0; clearCount < numClears; ++clearCount)
		{
			for(int i = 0; i < testSize; ++i)
			{
				ASSERT_TRUE(block.make<double>());
			}

			block.clear();

			EXPECT_GE(block.numAllocatedBytes(), sizeof(double) * testSize);
			EXPECT_EQ(block.numUsedBytes(), 0);
			EXPECT_GE(block.numRemainingBytes(), sizeof(double) * testSize);
		}
	}
}

TEST(HostMemoryBlockTest, ExhaustedBlockReturnsNull)
{
	// Allocate single `int` until fail
	{
		constexpr int numInts = 100;

		HostMemoryBlock block(sizeof(int) * numInts);

		// First exhaust guaranteed memory space
		for(int i = 0; i < numInts; ++i)
		{
			EXPECT_TRUE(block.make<int>());
		}

		// Create a large amount of ints so any reasonable padding would be exhausted
		for(int i = 0; i < numInts * 100; ++i)
		{
			block.make<int>();
		}

		// Now any more creation should fail
		for(int i = 0; i < numInts; ++i)
		{
			EXPECT_FALSE(block.make<int>());
		}

		block.clear();

		// Work as usual after clear
		for(int i = 0; i < numInts; ++i)
		{
			EXPECT_TRUE(block.make<int>());
		}
	}
}
