#include <Utility/Concurrent/TSPSCExecutor.h>
#include <Utility/TFunction.h>

#include <gtest/gtest.h>

#include <functional>
#include <thread>
#include <vector>
#include <atomic>

using namespace ph;

TEST(TSPSCExecutorTest, ConstructWithoutWork)
{
	for(int i = 0; i < 1000; ++i)
	{
		{
			TSPSCExecutor<std::function<void()>> executor;
			EXPECT_TRUE(executor.getId() != std::thread::id());
		}

		{
			TSPSCExecutor<TFunction<void()>> executor;
			EXPECT_TRUE(executor.getId() != std::thread::id());
		}
	}
}

TEST(TSPSCExecutorTest, RunningMultipleWorks)
{
	// Run a single work and terminate
	{
		for(int i = 0; i < 100; ++i)
		{
			TSPSCExecutor<std::function<void()>> executor;
			executor.start();

			int value = 0;
			executor.addWork(
				[&value]()
				{
					value += 1;
				});

			executor.waitAllWorks();

			EXPECT_EQ(value, 1);
		}
	}

	// Run multiple works and terminate
	{
		for(int i = 0; i < 100; ++i)
		{
			TSPSCExecutor<std::function<void()>> executor;
			executor.start();

			const int numWorks = i + 2;

			int value = 0;
			for(int j = 0; j < numWorks; ++j)
			{
				executor.addWork(
					[&value]()
					{
						value += 1;
					});
			}

			executor.waitAllWorks();

			EXPECT_EQ(value, numWorks);
		}
	}
}

//TEST(TSPSCExecutorTest, MPSCWorks)
//{
//	TSingleThreadExecutor<std::function<void()>> executor;
//
//	const int numProducers = 20;
//	const int numWorksPerProducer = 1000;
//
//	std::atomic_uint32_t counter = 0;
//
//	std::vector<std::thread> producers(numProducers);
//	for(int pi = 0; pi < numProducers; ++pi)
//	{
//		producers[pi] = std::thread(
//			[&executor, &counter, numWorksPerProducer]()
//			{
//				for(int wi = 0; wi < numWorksPerProducer; ++wi)
//				{
//					executor.addWork(
//						[&counter]()
//						{
//							counter.fetch_add(1, std::memory_order_relaxed);
//						});
//				}
//
//				executor.waitAllWorksFromThisThread();
//			});
//	}
//
//	// Now wait for all producers done their waiting
//	for(int pi = 0; pi < numProducers; ++pi)
//	{
//		producers[pi].join();
//	}
//
//	EXPECT_EQ(counter.load(std::memory_order_relaxed), numProducers * numWorksPerProducer);
//}
