#pragma once

#include "Common/primitive_type.h"
#include "Utility/Concurrent/TLockFreeQueue.h"
#include "Common/config.h"

#include <vector>
#include <thread>
#include <condition_variable>
#include <functional>
#include <atomic>

#include <mutex>

namespace ph
{

/*! @brief A thread pool where works are accessed concurrently without blocking other threads.
A thread pool that contains fixed number of threads for work processing. It is a lock-free pool, 
i.e., works are enqueued and dequeued concurrently without blocking other threads. The pool can be 
used concurrently, namely, it is thread-safe. However, the user must ensure that the pool is properly
initialized before subsequent usages.
*/
class FixedSizeLockFreeThreadPool final
{
public:
	using Work = std::function<void()>;

public:
	explicit FixedSizeLockFreeThreadPool(std::size_t numWorkers);

	/*! @brief Terminates the pool, effectively the same as calling requestTermination(). 
	*/
	~FixedSizeLockFreeThreadPool();

	/*! @brief Add a work to the pool.
	@note Thread-safe.
	*/
	///@{
	void queueWork(const Work& work);
	void queueWork(Work&& work);
	///@}

	/*! @brief Blocks until all queued works are finished.
	New works can be queued after waiting is finished.
	@note Thread-safe.
	*/
	void waitAllWorks();

	/*! @brief Stop processing queued works as soon as possible.
	Workers will stop processing queued works as soon as possible. Works that are already being processed 
	will still complete. No other operations should be further performed after requesting termination. 
	Requesting termination multiple times has the same effect.
	@note Thread-safe.
	*/
	void requestTermination();

	/*! @brief Number of worker threads in the pool.
	@note Thread-safe.
	*/
	std::size_t numWorkers() const;

private:

#ifdef PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS
	static_assert(std::atomic_uint64_t::is_always_lock_free);
#endif

	std::vector<std::thread> m_workers;
	TLockFreeQueue<Work>     m_works;
	std::mutex               m_poolMutex;
	std::condition_variable  m_workersCv;
	std::condition_variable  m_allWorksDoneCv;
	bool                     m_isTerminationRequested;
	std::atomic_uint64_t     m_numUnfinishedWorks;

	void asyncProcessWork();
};

// In-header Implementations:

inline std::size_t FixedSizeLockFreeThreadPool::numWorkers() const
{
	return m_workers.size();
}

}// end namespace ph
