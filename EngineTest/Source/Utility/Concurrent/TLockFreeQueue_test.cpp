#include <Utility/Concurrent/TLockFreeQueue.h>

#include <gtest/gtest.h>

#include <cstddef>

using namespace ph;

TEST(TLockFreeQueueTest, Creation)
{
	{
		TLockFreeQueue<int> queue;
		EXPECT_EQ(queue.estimatedSize(), 0);
	}

	{
		struct TestStruct
		{
			int x;
			float y;
		};

		TLockFreeQueue<TestStruct> queue(123);
		EXPECT_EQ(queue.estimatedSize(), 0);
	}
}

TEST(TLockFreeQueueTest, SingleThreadedEnqueueAndDequeue)
{
	{
		TLockFreeQueue<int> queue;

		for(int i = 0; i < 64; ++i)
		{
			queue.enqueue(i);
		}

		for(int i = 64; i < 128; ++i)
		{
			ASSERT_TRUE(queue.tryEnqueue(i));
		}


		for(int i = 0; i < 128; ++i)
		{
			int value = -1;
			ASSERT_TRUE(queue.tryDequeue(&value));
			EXPECT_EQ(value, i);
		}
	}
}

TEST(TLockFreeQueueTest, MultiThreadedEnqueueAndDequeue)
{
	// TODO
}
