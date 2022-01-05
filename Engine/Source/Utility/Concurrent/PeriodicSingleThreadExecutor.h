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

class PeriodicSingleThreadExecutor final
{
public:
	using Work = std::function<void()>;

public:
	explicit PeriodicSingleThreadExecutor(uint64 periodMS);
	~PeriodicSingleThreadExecutor();

	void addWork(const Work& work);
	void addWork(Work&& work);
	void setPeriodMS(uint64 periodMS);
	void pause();
	void resume();
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
	std::atomic<uint64>     m_periodMS;
};

// In-header Implementations:

inline void PeriodicSingleThreadExecutor::setPeriodMS(const uint64 periodMS)
{
	m_periodMS.store(periodMS, std::memory_order_relaxed);
}

inline bool PeriodicSingleThreadExecutor::isWorkerThread() const
{
	return std::this_thread::get_id() == m_worker.get_id();
}

}// end namespace ph
