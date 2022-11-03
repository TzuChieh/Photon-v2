#pragma once

#include "Common/config.h"
#include "Common/primitive_type.h"

#include <atomic>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace ph
{

/*! @brief A single-thread executor that runs specified works periodically.
The executor can be used concurrently, namely, it is thread-safe. However, the user must ensure that
the pool is properly initialized before subsequent usages.
*/
class PeriodicSingleThreadExecutor final
{
public:
	using Work = std::function<void()>;

public:
	/*! @brief Create an executor in the paused state.
	*/
	explicit PeriodicSingleThreadExecutor(uint64 periodMs);

	/*! @brief Terminate the execution. Wait for any ongoing work to finish.
	*/
	~PeriodicSingleThreadExecutor();

	/*! @brief Add a work to the executor.
	Work will not start until calling resume().
	@note Thread-safe.
	*/
	///@{
	void addWork(const Work& work);
	void addWork(Work&& work);
	///@}

	/*! @brief Sets execution period of the executor in milliseconds.
	The new period will take effect within finite amount of time (as soon as possible).
	@note Thread-safe.
	*/
	void setPeriodMs(uint64 periodMs);

	/*! @brief Pause the execution.
	Should not be called on worker thread.
	@note Thread-safe.
	*/
	void pause();

	/*! @brief Resume the execution.
	Should not be called on worker thread.
	@note Thread-safe.
	*/
	void resume();

	/*! @brief Stop the executor.
	Worker will stop processing any work as soon as possible. Any work that is already being processed
	will still complete. Paused state will be resumed and terminate. No further write operations should
	be performed after requesting termination.
	Requesting termination multiple times has the same effect.
	@note Thread-safe.
	*/
	void requestTermination();

private:
	/*! @brief Start processing works.
	@note Thread-safe.
	*/
	void asyncProcessWork();

	/*! @brief Check whether current thread is the worker thread.
	@note Thread-safe.
	*/
	bool isWorkerThread() const;


#ifdef PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS
	static_assert(std::atomic<uint64>::is_always_lock_free);
#endif

	std::thread             m_worker;
	std::vector<Work>       m_works;
	std::mutex              m_executorMutex;
	std::condition_variable m_workerCv;
	bool                    m_isWorking;
	bool                    m_isTerminationRequested;
	std::atomic<uint64>     m_periodMs;
};

// In-header Implementations:

inline void PeriodicSingleThreadExecutor::setPeriodMs(const uint64 periodMs)
{
	m_periodMs.store(periodMs, std::memory_order_relaxed);
}

inline bool PeriodicSingleThreadExecutor::isWorkerThread() const
{
	return std::this_thread::get_id() == m_worker.get_id();
}

}// end namespace ph
