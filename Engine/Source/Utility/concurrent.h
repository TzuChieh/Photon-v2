#pragma once

#include "Common/assertion.h"
#include "Math/math.h"

#include <functional>
#include <cstddef>
#include <thread>

namespace ph
{

/*
	Runs specified works in parallel. The function will block calling thread 
	until all works are complete.

	<totalWorkSize>: total amount of work
	<numWorkers>:    number of workers running the works
	<work>:          The actual work that is going to be executed; where the
	                 index range [workBegin, workEnd) is specified as input,
	                 as well as the index of the executing worker.
*/
inline void parallel_work(
	const std::size_t totalWorkSize,
	const std::size_t numWorkers,

	const std::function<
		void(std::size_t workerIdx, std::size_t workBegin, std::size_t workEnd)
	>& work)
{
	PH_ASSERT(numWorkers > 0);

	std::vector<std::thread> workers(numWorkers);
	for(std::size_t workerIdx = 0; workerIdx < numWorkers; ++workerIdx)
	{
		const auto workRange = math::ith_evenly_divided_range(workerIdx, totalWorkSize, numWorkers);

		// TODO: should we execute 0-sized works? (currently they are executed)
		workers[workerIdx] = std::thread(work, workerIdx, workRange.first, workRange.second);
	}

	for(std::size_t workerIdx = 0; workerIdx < numWorkers; ++workerIdx)
	{
		PH_ASSERT(workers[workerIdx].joinable());
		workers[workerIdx].join();
	}
}

}// end namespace ph
