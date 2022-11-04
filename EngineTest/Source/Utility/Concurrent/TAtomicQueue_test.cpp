#include <Utility/Concurrent/TAtomicQueue.h>

#include <gtest/gtest.h>

#include <cstddef>
#include <thread>

using namespace ph;

TEST(TAtomicQueueTest, Creation)
{
	{
		TAtomicQueue<int> queue;
		EXPECT_EQ(queue.estimatedSize(), 0);
	}

	{
		struct TestStruct
		{
			int x;
			float y;
		};

		TAtomicQueue<TestStruct> queue(123);
		EXPECT_EQ(queue.estimatedSize(), 0);
	}
}

TEST(TAtomicQueue, SingleThreadedEnqueueAndDequeue)
{
	{
		TAtomicQueue<int> queue;

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

TEST(TAtomicQueue, MultiThreadedEnqueueAndDequeue)
{
	// Based on the sample code from https://github.com/cameron314/concurrentqueue/blob/master/samples.md
	{
		TAtomicQueue<int> q;
		int dequeued[1000] = {0};
		std::thread threads[20];

		// Producers
		for(int ti = 0; ti != 10; ++ti)
		{
			threads[ti] = std::thread([&](int ti)
			{
				for(int j = 0; j != 100; ++j)
				{
					q.enqueue(ti * 100 + j);
				}
			}, ti);
		}

		// Consumers
		for(int ti = 10; ti != 20; ++ti)
		{
			threads[ti] = std::thread([&]()
			{
				int item;
				for(int j = 0; j != 200; ++j)
				{
					if(q.tryDequeue(&item))
					{
						++dequeued[item];
					}
				}
			});
		}

		// Wait for all threads
		for(int i = 0; i != 20; ++i)
		{
			threads[i].join();
		}

		// Collect any leftovers (could be some if e.g. consumers finish before producers)
		int item;
		while(q.tryDequeue(&item))
		{
			++dequeued[item];
		}

		// Make sure everything went in and came back out!
		for(int i = 0; i != 1000; ++i)
		{
			EXPECT_EQ(dequeued[i], 1);
		}
	}
}
