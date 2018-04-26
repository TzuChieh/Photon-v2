#include <Utility/FixedSizeThreadPool.h>

#include <gtest/gtest.h>

#include <random>
#include <atomic>

TEST(FixedSizeThreadPoolTest, CalculateNumberSum)
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<> dis(-10, 10);

	const int NUM_WORKERS        = 4;
	const int NUMBERS_PER_WORKER = 10000;
	const int NUMBERS            = NUM_WORKERS * NUMBERS_PER_WORKER;

	int numbers[NUMBERS];
	for(int i = 0; i < NUMBERS; i++)
	{
		numbers[i] = dis(rng);
	}

	int actualSum = 0;
	for(int i = 0; i < NUMBERS; i++)
	{
		actualSum += numbers[i];
	}

	ph::FixedSizeThreadPool threadPool(NUM_WORKERS);
	std::atomic_int testSum = 0;
	for(int i = 0; i < NUM_WORKERS; i++)
	{
		threadPool.queueWork([i, NUMBERS_PER_WORKER, &numbers, &testSum]()
		{
			for(int j = i * NUMBERS_PER_WORKER; 
			    j < (i + 1) * NUMBERS_PER_WORKER;
			    j++)
			{
				testSum += numbers[j];
			}
		});
	}
	threadPool.waitAllWorks();

	EXPECT_EQ(testSum, actualSum);
}

TEST(FixedSizeThreadPoolTest, GetAttributes)
{
	ph::FixedSizeThreadPool threadPool(2);
	EXPECT_EQ(threadPool.numWorkers(), 2);
}