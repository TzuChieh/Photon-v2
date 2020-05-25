#pragma once

#include "Common/assertion.h"
#include "Math/math.h"

#include <functional>
#include <cstddef>
#include <thread>

namespace ph
{

/*! @brief Set the parametric distance where the ray ends.

Runs specified works in parallel. The function will block calling thread 
until all works are complete. Note that 0-sized works are not executed 
(@p work is not called in this case).

@param totalWorkSize Total amount of work.
@param numWorkers Number of workers running the works.
@param work The actual work that is going to be executed; where the index
range [workBegin, workEnd) is specified as input, as well as the index of
the executing worker.
*/
inline void parallel_work(
	const std::size_t totalWorkSize,
	const std::size_t numWorkers,

	const std::function<
		void(std::size_t workerIdx, std::size_t workBegin, std::size_t workEnd)
	>& work)
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

}// end namespace ph
