#include <RenderCore/Memory/GraphicsFrameArena.h>

#include <gtest/gtest.h>

#include <utility>
#include <type_traits>
#include <limits>
#include <vector>

using namespace ph;
using namespace ph::editor;

TEST(GraphicsFrameArenaTest, ConstructAndMove)
{
	// Default construction--no storage
	{
		GraphicsFrameArena arena;
		EXPECT_EQ(arena.numAllocatedBytes(), 0);
		EXPECT_EQ(arena.numUsedBytes(), 0);
		EXPECT_EQ(arena.numRemainingBytes(), 0);
	}

	// Basic construction with size hint
	{
		GraphicsFrameArena arena(123);
		EXPECT_GE(arena.numAllocatedBytes(), 123);
		EXPECT_EQ(arena.numUsedBytes(), 0);
		EXPECT_GE(arena.numRemainingBytes(), 123);
	}

	// Basic construction with move
	{
		GraphicsFrameArena arena(1000);
		EXPECT_GE(arena.numAllocatedBytes(), 1000);
		EXPECT_EQ(arena.numUsedBytes(), 0);
		EXPECT_GE(arena.numRemainingBytes(), 1000);

		GraphicsFrameArena arena2;
		EXPECT_GE(arena2.numAllocatedBytes(), 0);
		EXPECT_EQ(arena2.numUsedBytes(), 0);
		EXPECT_GE(arena2.numRemainingBytes(), 0);

		arena2 = std::move(arena);

		EXPECT_GE(arena.numAllocatedBytes(), 0);
		EXPECT_EQ(arena.numUsedBytes(), 0);
		EXPECT_GE(arena.numRemainingBytes(), 0);

		EXPECT_GE(arena2.numAllocatedBytes(), 1000);
		EXPECT_EQ(arena2.numUsedBytes(), 0);
		EXPECT_GE(arena2.numRemainingBytes(), 1000);
	}

	// Disallow copying
	{
		static_assert(std::is_copy_constructible_v<GraphicsFrameArena> == false);
		static_assert(std::is_copy_assignable_v<GraphicsFrameArena> == false);
	}
}

TEST(GraphicsFrameArenaTest, ObjectAllocation)
{
	// Single `int`
	{
		GraphicsFrameArena arena(100);
		int* ptr = arena.make<int>();// zero-init
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, 0);

		ptr = arena.make<int>(777);
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, 777);

		ptr = arena.make<int>(-999);
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, -999);

		ptr = arena.make<int>(std::numeric_limits<int>::max());
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, std::numeric_limits<int>::max());

		ptr = arena.make<int>(std::numeric_limits<int>::min());
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, std::numeric_limits<int>::min());
	}

	// Single `double`
	{
		GraphicsFrameArena arena(200);
		double* ptr = arena.make<double>(3.0);
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, 3.0);

		ptr = arena.make<double>(-345.678);
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, -345.678);

		ptr = arena.make<double>(std::numeric_limits<double>::max());
		ASSERT_TRUE(ptr);
		EXPECT_EQ(*ptr, std::numeric_limits<double>::max());

		ptr = arena.make<double>(std::numeric_limits<double>::lowest());
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
		GraphicsFrameArena arena(200);
		Test* ptr = arena.make<Test>();
		ASSERT_TRUE(ptr);
		EXPECT_EQ(ptr->x, 1);
		EXPECT_EQ(ptr->y, 2.0);
	}

	// Multiple objects
	{
		constexpr int numObjs = 1000;

		GraphicsFrameArena arena(sizeof(Test) * numObjs);
		std::vector<Test*> ptrs;
		for(int i = 0; i < numObjs; ++i)
		{
			Test test;
			test.x = i;
			test.y = i;

			ptrs.push_back(arena.make<Test>(test));
		}

		for(int i = 0; i < numObjs; ++i)
		{
			ASSERT_TRUE(ptrs[i]);
			EXPECT_EQ(ptrs[i]->x, i);
			EXPECT_EQ(ptrs[i]->y, i);
		}
	}
}

TEST(GraphicsFrameArenaTest, ArrayAllocation)
{
	// `int` array
	{
		constexpr int numInts = 100;

		GraphicsFrameArena arena(sizeof(int) * numInts);
		TSpan<int> ints = arena.makeArray<int>(numInts);// zero-init all elements

		EXPECT_EQ(ints.size(), numInts);
		for(int i = 0; i < numInts; ++i)
		{
			EXPECT_EQ(ints[i], 0);
		}
	}

	// `double` array
	{
		constexpr int numDoubles = 1000;

		GraphicsFrameArena arena(sizeof(double) * numDoubles);
		TSpan<double> doubles = arena.makeArray<double>(numDoubles);

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

		GraphicsFrameArena arena(sizeof(Test) * numObjs);
		TSpan<Test> objs = arena.makeArray<Test>(numObjs);

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

TEST(GraphicsFrameArenaTest, ClearAndReuse)
{
	// `double` array
	{
		constexpr int testSize = 1000;
		constexpr int numClears = 10;

		GraphicsFrameArena arena(sizeof(double) * testSize);

		for(int clearCount = 0; clearCount < numClears; ++clearCount)
		{
			for(int i = 0; i < testSize; ++i)
			{
				ASSERT_TRUE(arena.make<double>());
			}

			arena.clear();

			EXPECT_GE(arena.numAllocatedBytes(), sizeof(double) * testSize);
			EXPECT_EQ(arena.numUsedBytes(), 0);
			EXPECT_GE(arena.numRemainingBytes(), sizeof(double) * testSize);
		}
	}
}

TEST(GraphicsFrameArenaTest, ExhaustedArenaReturnsNull)
{
	// Allocate single `int` until fail
	{
		constexpr int numInts = 100;

		GraphicsFrameArena arena(sizeof(int) * numInts);

		// First exhaust guaranteed memory space
		for(int i = 0; i < numInts; ++i)
		{
			EXPECT_TRUE(arena.make<int>());
		}

		// Create a large amount of ints so any reasonable padding would be exhausted
		for(int i = 0; i < numInts * 100; ++i)
		{
			arena.make<int>();
		}

		// Now any more creation should fail
		for(int i = 0; i < numInts; ++i)
		{
			EXPECT_FALSE(arena.make<int>());
		}

		arena.clear();

		// Work as usual after clear
		for(int i = 0; i < numInts; ++i)
		{
			EXPECT_TRUE(arena.make<int>());
		}
	}
}
