#pragma once

#include <cstddef>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

namespace ph
{

/*! @brief A thread pool where works are accessed concurrently by blocking other threads.
A thread pool that contains fixed number of threads for work processing. It is a blocking pool, i.e., works
are enqueued and dequeued concurrently by blocking other threads, in FIFO order. The pool can be used 
concurrently, namely, it is thread-safe. However, the user must ensure that the pool is properly
initialized before subsequent usages.
*/
class FixedSizeThreadPool final
{
public:
	using Work = std::function<void()>;

public:
	explicit FixedSizeThreadPool(std::size_t numWorkers);

	/*! @brief Terminates the pool, effectively the same as calling requestTermination(). 
	*/
	~FixedSizeThreadPool();

	/*! @brief Queue up a work to the pool.
	Workers start to process the work right away. The works are guaranteed to be processed in FIFO order.
	@note Thread-safe.
	*/
	///@{
	void queueWork(const Work& work);
	void queueWork(Work&& work);
	///@}

	/*! @brief Blocks until all queued works are finished.
	Should not be called on worker thread. New works can be queued after waiting is finished. It may not
	be desirable to call this method while works are still being queued and processed (e.g., MPMC).
	As it is possible that sometimes the work queue is temporarily empty and cause this method to 
	return early.
	@note Thread-safe.
	*/
	void waitAllWorks();

	/*! @brief Stop processing queued works as soon as possible.
	Workers will stop processing queued works as soon as possible. Any work that is already being
	processed will still complete. No further write operations should be performed after requesting
	termination. Threads waiting for the completion of works, e.g., waiting on waitAllWorks(), will
	stop waiting. Requesting termination multiple times has the same effect.
	@note Thread-safe.
	*/
	void requestTermination();

	/*! @brief Number of worker threads in the pool.
	@note Thread-safe.
	*/
	std::size_t numWorkers() const;

private:
	/*! @brief Start processing works.
	@note Thread-safe.
	*/
	void asyncProcessWork();

	/*! @brief Check whether current thread is one of the worker threads.
	@note Thread-safe.
	*/
	bool isWorkerThread() const;

	std::vector<std::thread> m_workers;
	std::queue<Work>         m_works;
	std::mutex               m_poolMutex;
	std::condition_variable  m_workersCv;
	std::condition_variable  m_allWorksDoneCv;
	bool                     m_isTerminationRequested;
	std::size_t              m_numUnfinishedWorks;
};

// In-header Implementations:

inline std::size_t FixedSizeThreadPool::numWorkers() const
{
	return m_workers.size();
}

inline bool FixedSizeThreadPool::isWorkerThread() const
{
	const auto thisThreadId = std::this_thread::get_id();
	for(const auto& workerThread : m_workers)
	{
		if(thisThreadId == workerThread.get_id())
		{
			return true;
		}
	}
	return false;
}

}// end namespace ph
