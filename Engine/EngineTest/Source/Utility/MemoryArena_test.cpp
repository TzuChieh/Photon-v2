#include <Utility/MemoryArena.h>

#include <gtest/gtest.h>

#include <utility>
#include <vector>
#include <new>

using namespace ph;

TEST(MemoryArenaTest, ConstructAndMove)
{
	// Basic construction
	{
		MemoryArena arena;
		EXPECT_EQ(arena.numAllocatedBlocks(), 0);
		EXPECT_EQ(arena.numAllocatedBytes(), 0);
		EXPECT_EQ(arena.numUsedBytes(), 0);
	}

	// Basic construction with move
	{
		MemoryArena arena(1024 * 16, 3);
		EXPECT_EQ(arena.numAllocatedBlocks(), 3);
		EXPECT_GT(arena.numAllocatedBytes(), 0);
		EXPECT_EQ(arena.numUsedBytes(), 0);

		auto arena2 = std::move(arena);
		EXPECT_EQ(arena2.numAllocatedBlocks(), 3);
		EXPECT_GT(arena2.numAllocatedBytes(), 0);
		EXPECT_EQ(arena2.numUsedBytes(), 0);
	}
}

TEST(MemoryArenaTest, SimpleAllocate)
{
	// Raw memory
	{
		MemoryArena arena;
		std::byte* mem = arena.allocRaw(128);
		EXPECT_GE(arena.numUsedBytes(), 128);

		auto* ptr = new(mem) std::pair<int, int>{-123, 456};
		EXPECT_EQ(ptr->first, -123);
		EXPECT_EQ(ptr->second, 456);
	}

	// Single object
	{
		MemoryArena arena;
		auto* intPtr = new(arena.alloc<int>()) int{999};
		EXPECT_EQ(*intPtr, 999);

		auto* doublePtr = new(arena.alloc<double>()) double{12.12};
		EXPECT_EQ(*doublePtr, 12.12);

		auto* pairPtr = new(arena.alloc<std::pair<float, float>>()) std::pair<float, float>{1.2f, 3.4f};
		EXPECT_EQ(pairPtr->first, 1.2f);
		EXPECT_EQ(pairPtr->second, 3.4f);
	}

	// Object array
	{
		MemoryArena arena;

		auto mem = arena.allocArray<std::pair<int, int>>(100);
		for(int i = 0; i < mem.size(); ++i)
		{
			new(&mem[i]) std::pair<int, int>{i, 3 * i};
		}

		for(int i = 0; i < mem.size(); ++i)
		{
			EXPECT_EQ(mem[i].first, i);
			EXPECT_EQ(mem[i].second, 3 * i);
		}
	}
}

TEST(MemoryArenaTest, LoopAllocate)
{
	struct Test
	{
		int x;
		float y;
		double z;
	};

	auto testFunc = [](MemoryArena& arena)
	{
		constexpr int TEST_SIZE = 100000;

		std::vector<Test*> tests(TEST_SIZE);
		for(int i = 0; i < TEST_SIZE; ++i)
		{
			auto* testPtr = new(arena.alloc<Test>()) Test{i, i * 2.0f, i * 3.0};
			tests[i] = testPtr;
		}

		for(int i = 0; i < TEST_SIZE; ++i)
		{
			ASSERT_EQ(tests[i]->x, i);
			ASSERT_EQ(tests[i]->y, i * 2.0f);
			ASSERT_EQ(tests[i]->z, i * 3.0);
		}
	};

	// Default arena settings
	{
		MemoryArena arena;
		testFunc(arena);
	}

	// Custom arena settings
	{
		// Purposely use a mismatched & small block size
		MemoryArena arena(sizeof(Test) * 16 + 3, 4);
		testFunc(arena);
	}
}

TEST(MemoryArenaTest, Make)
{
	struct Test
	{
		int x;
		float y;
		double z;

		Test(int x, float y, double z) : x(x * 11), y(y * 22.0f), z(z * 33.0)
		{}
	};

	MemoryArena arena;

	constexpr int TEST_SIZE = 100000;

	std::vector<Test*> tests(TEST_SIZE);
	for(int i = 0; i < TEST_SIZE; ++i)
	{
		tests[i] = arena.make<Test>(i, static_cast<float>(i), i);
	}

	for(int i = 0; i < TEST_SIZE; ++i)
	{
		ASSERT_EQ(tests[i]->x, i * 11);
		ASSERT_EQ(tests[i]->y, i * 22.0f);
		ASSERT_EQ(tests[i]->z, i * 33.0);
	}
}

TEST(MemoryArenaTest, SimpleClear)
{
	constexpr int TEST_SIZE = 100000;

	auto testFunc = [](MemoryArena& arena)
	{
		std::vector<double*> ptrs(TEST_SIZE);
		for(int i = 0; i < TEST_SIZE; ++i)
		{
			ptrs[i] = arena.make<double>(i);
		}

		for(int i = 0; i < TEST_SIZE; ++i)
		{
			ASSERT_EQ(*ptrs[i], i);
		}
	};

	MemoryArena arena(sizeof(double) * 2 + 1, 4);
	testFunc(arena);

	EXPECT_GT(arena.numAllocatedBlocks(), 0);
	EXPECT_GT(arena.numAllocatedBytes(), 0);
	EXPECT_GT(arena.numUsedBytes(), 0);

	arena.clear();

	EXPECT_GT(arena.numAllocatedBlocks(), 0);
	EXPECT_GT(arena.numAllocatedBytes(), 0);
	EXPECT_EQ(arena.numUsedBytes(), 0);
	
	const auto previousBlockCount = arena.numAllocatedBlocks();

	// Try 10 more times to see if we use the same number of blocks each time
	for(std::size_t i = 0; i < 10; ++i)
	{
		testFunc(arena);

		EXPECT_EQ(arena.numAllocatedBlocks(), previousBlockCount);

		arena.clear();
	}
}

TEST(MemoryArenaTest, NonTrivialClear)
{
	struct CustomDtor
	{
		int& counter;

		CustomDtor(int& counter)
			: counter(counter)
		{}

		~CustomDtor()
		{
			++counter;
		}
	};

	// Explicitly destruct by calling `clear()`
	{
		constexpr int TEST_SIZE = 100000;

		int counter = 0;

		MemoryArena arena;
		for(int i = 0; i < TEST_SIZE; ++i)
		{
			arena.make<CustomDtor>(counter);
		}
		arena.clear();

		EXPECT_EQ(counter, TEST_SIZE);
	}

	// Implicitly destruct by the destruction of arena
	{
		constexpr int TEST_SIZE = 100000;

		int counter = 0;

		{
			MemoryArena arena;
			for(int i = 0; i < TEST_SIZE; ++i)
			{
				arena.make<CustomDtor>(counter);
			}
		}

		EXPECT_EQ(counter, TEST_SIZE);
	}
}

TEST(MemoryArenaTest, ErrorAndResume)
{
	MemoryArena arena(sizeof(double) * 2 + 1, 4);
	
	std::vector<double*> ptrs;
	for(int i = 0; i < 10; ++i)
	{
		ptrs.push_back(arena.make<double>(i));
	}
	for(int i = 0; i < 10; ++i)
	{
		EXPECT_EQ(*ptrs[i], i);
	}

	const auto previousUsedBytes = arena.numUsedBytes();
	const auto previousAllocatedBytes = arena.numAllocatedBytes();

	// Purposely fail the allocation--using a single large array (larger than block size)
	EXPECT_THROW(arena.allocArray<float>(999), std::bad_alloc);

	// Failed allocation should not affect internal states
	EXPECT_EQ(arena.numUsedBytes(), previousUsedBytes);
	EXPECT_EQ(arena.numAllocatedBytes(), previousAllocatedBytes);

	// More followed allocations
	for(int i = 10; i < 20; ++i)
	{
		ptrs.push_back(arena.make<double>(i));
	}
	ASSERT_EQ(ptrs.size(), 20);
	for(int i = 0; i < 20; ++i)
	{
		EXPECT_EQ(*ptrs[i], i);
	}
}
