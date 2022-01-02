#pragma once

#include "Utility/IMoveOnly.h"
#include "Common/primitive_type.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

namespace ph
{

/*! @brief A thread pool with fixed number of threads. Works are accessed concurrently by blocking other threads.
A thread pool that contains fixed number of threads for work processing. It is a blocking pool, i.e., works
are enqueued and dequeued concurrently by blocking other threads, in FIFO order. The pool can be used 
concurrently, namely, it is thread-safe. However, the user must ensure that the pool is properly
initialized before subsequent usages.
*/
class FixedSizeBlockingThreadPool final : private IMoveOnly
{
public:
	using Work = std::function<void()>;

public:
	explicit FixedSizeBlockingThreadPool(std::size_t numWorkers);

	/*! @brief Terminates the pool, effectively the same as calling requestTermination(). 
	*/
	~FixedSizeBlockingThreadPool();

	void queueWork(const Work& work);
	void queueWork(Work&& work);

	/*! @brief Blocks until all queued works are finished.
	New works can be queued after waiting is finished.
	*/
	void waitAllWorks();

	/*! @brief Stop processing queued works as soon as possible.
	Workers will stop processing queued works as soon as possible. Works that are already being processed 
	will still complete. No other operations should be further performed after requesting termination. 
	Requesting termination multiple times has the same effect.
	*/
	void requestTermination();

	std::size_t numWorkers() const;

private:
	std::vector<std::thread> m_workers;
	std::queue<Work>         m_works;
	std::mutex               m_poolMutex;
	std::condition_variable  m_workersCv;
	std::condition_variable  m_allWorksDoneCv;
	bool                     m_isTerminationRequested;
	uint64                   m_numQueuedWorks;
	uint64                   m_numProcessedWorks;

	void asyncProcessWork();
};

// In-header Implementations:

inline std::size_t FixedSizeBlockingThreadPool::numWorkers() const
{
	return m_workers.size();
}

}// end namespace ph
