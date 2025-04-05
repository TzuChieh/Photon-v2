#pragma once

#include <cstddef>
#include <functional>

namespace ph
{

class FixedSizeThreadPool;

/*! @brief Runs specified works in parallel.
The function will block the calling thread until all works are complete.
@param numWorkers Number of workers running the works.
@param work The actual work that is going to be executed; where the index of the executing worker
is specified as input.
*/
void parallel_work(
	const std::size_t numWorkers,

	std::function<
		void(std::size_t workerIdx)
	> work);

/*! @brief Runs specified works in parallel.
The function will block the calling thread until all works are complete. Note that 0-sized works are not
executed (@p work is not called in this case).
@param totalWorkSize Total amount of work.
@param numWorkers Number of workers running the works.
@param work The actual work that is going to be executed; where the index range `[workBegin, workEnd)`
is specified as input, as well as the index of the executing worker.
*/
void parallel_work(
	const std::size_t totalWorkSize,
	const std::size_t numWorkers,

	std::function<
		void(std::size_t workerIdx, std::size_t workBegin, std::size_t workEnd)
	> work);

/*! @brief Runs specified works in parallel using a thread pool.
The function will block the calling thread until all works are complete. Note that 0-sized works are not
executed (@p work is not called in this case).
@param workers The thread pool to use.
@param totalWorkSize Total amount of work.
@param work The actual work that is going to be executed; where the index range `[workBegin, workEnd)`
is specified as input, as well as the index of the executing worker.
*/
void parallel_work(
	FixedSizeThreadPool& workers,
	const std::size_t totalWorkSize,

	std::function<
		void(std::size_t workerIdx, std::size_t workBegin, std::size_t workEnd)
	> work);

}// end namespace ph
