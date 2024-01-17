#include "Utility/Concurrent/concurrent.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <functional>
#include <cstddef>
#include <thread>

namespace ph
{

void parallel_work(
	const std::size_t numWorkers,

	std::function<
		void(std::size_t workerIdx)
	> work)
{
	PH_ASSERT_GT(numWorkers, 0);

	std::vector<std::thread> workers(numWorkers);
	for(std::size_t workerIdx = 0; workerIdx < numWorkers; ++workerIdx)
	{
		workers[workerIdx] = std::thread(work, workerIdx);
	}

	for(auto& workerThread : workers)
	{
		// Not joining default-constructed thread and already-joined thread
		if(workerThread.joinable())
		{
			workerThread.join();
		}
	}
}

void parallel_work(
	const std::size_t totalWorkSize,
	const std::size_t numWorkers,

	std::function<
		void(std::size_t workerIdx, std::size_t workBegin, std::size_t workEnd)
	> work)
{
	PH_ASSERT_GT(numWorkers, 0);

	std::vector<std::thread> workers(numWorkers);
	for(std::size_t workerIdx = 0; workerIdx < numWorkers; ++workerIdx)
	{
		const auto workRange = math::ith_evenly_divided_range(workerIdx, totalWorkSize, numWorkers);

		// Skip the execution of 0-sized work
		const auto workSize = workRange.second - workRange.first;
		if(workSize > 0)
		{
			workers[workerIdx] = std::thread(work, workerIdx, workRange.first, workRange.second);
		}
	}

	for(auto& workerThread : workers)
	{
		// Not joining default-constructed thread and already-joined thread
		if(workerThread.joinable())
		{
			workerThread.join();
		}
	}
}

void parallel_work(
	FixedSizeThreadPool& workers,
	const std::size_t totalWorkSize,

	std::function<
		void(std::size_t workerIdx, std::size_t workBegin, std::size_t workEnd)
	> work)
{
	for(std::size_t workerIdx = 0; workerIdx < workers.numWorkers(); ++workerIdx)
	{
		const auto workRange = math::ith_evenly_divided_range(workerIdx, totalWorkSize, workers.numWorkers());

		// Skip the execution of 0-sized work
		const auto workSize = workRange.second - workRange.first;
		if(workSize > 0)
		{
			workers.queueWork(
				[work, workerIdx, workRange]()
				{
					work(workerIdx, workRange.first, workRange.second);
				});
		}
	}

	workers.waitAllWorks();
}

}// end namespace ph
