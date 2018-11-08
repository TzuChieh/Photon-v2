#include <Utility/concurrent.h>

#include <gtest/gtest.h>

#include <vector>
#include <atomic>

TEST(ConcurrentTest, ParallelWork)
{
	using namespace ph;

	std::vector<int> numbers = {
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	};

	const std::size_t totalWorkSize = numbers.size();
	const std::size_t numWorkers    = 5;

	std::atomic_int sum = 0;
	parallel_work(totalWorkSize, numWorkers,
		[=, &numbers, &sum](
			const std::size_t workerIdx, 
			const std::size_t workStart,
			const std::size_t workEnd)
		{
			EXPECT_LT(workerIdx, numWorkers);
			EXPECT_EQ(workEnd - workStart, 2);

			for(std::size_t i = workStart; i < workEnd; ++i)
			{
				sum += numbers[i];
			}
		});
	EXPECT_EQ(sum, 55);
}