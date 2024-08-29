#include <Utility/TArrayHeap.h>

#include <gtest/gtest.h>

#include <string>

using namespace ph;

TEST(TArrayHeapTest, InitHeap)
{
	{
		TArrayHeap<int, 3> heap;
		EXPECT_TRUE(heap.isEmpty());
		EXPECT_EQ(heap.size(), 0);
	}

	{
		TArrayHeap<std::string, 5> heap;
		EXPECT_TRUE(heap.isEmpty());
		EXPECT_EQ(heap.size(), 0);
	}
}

TEST(TArrayHeapTest, MaxHeap)
{
	// Same case as https://en.cppreference.com/w/cpp/algorithm/pop_heap
	{
		TArrayHeap<int, 6> heap;
		heap.push(3);
		heap.push(1);
		heap.push(4);
		heap.push(1);
		heap.push(5);
		heap.push(9);

		EXPECT_EQ(heap.size(), 6);
		EXPECT_EQ(heap.top(), 9);

		heap.pop();

		EXPECT_EQ(heap.size(), 5);
		EXPECT_EQ(heap.top(), 5);

		heap.pop();

		EXPECT_EQ(heap.size(), 4);
		EXPECT_EQ(heap.top(), 4);

		heap.pop();

		EXPECT_EQ(heap.size(), 3);
		EXPECT_EQ(heap.top(), 3);

		heap.pop();

		EXPECT_EQ(heap.size(), 2);
		EXPECT_EQ(heap.top(), 1);

		heap.pop();

		EXPECT_EQ(heap.size(), 1);
		EXPECT_EQ(heap.top(), 1);

		heap.pop();

		EXPECT_EQ(heap.size(), 0);
	}
}
