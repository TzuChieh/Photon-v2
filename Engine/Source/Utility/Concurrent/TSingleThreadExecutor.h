#pragma once

#include "Utility/Concurrent/TBlockableAtomicQueue.h"

#include <type_traits>
#include <thread>
#include <atomic>

namespace ph
{

template<typename Work>
class TSingleThreadExecutor final
{
	static_assert(std::is_invocable_v<Work>,
		"Work must be callable without any argument.");

public:
	/*! @brief Create an executor at waits for new work.
	*/
	TSingleThreadExecutor();

	/*! @brief Terminate the execution. Wait for any ongoing work to finish.
	*/
	~TSingleThreadExecutor();

	template<typename DeducedWork>
	void addWork(DeducedWork&& work);

	void waitAllWorksFromThisThread();

	/*! @brief Stop the executor.
	Worker will stop processing any work as soon as possible. Any work that is already being processed
	will still complete. No further operations should be performed after requesting termination. 
	Requesting termination multiple times has the same effect.
	@note Thread-safe.
	*/
	void requestTermination();

	/*! @brief Get ID of the underlying thread.
	@note Thread-safe.
	*/
	std::thread::id getId() const;

private:
	/*! @brief Start processing works.
	@note Thread-safe.
	*/
	void asyncProcessWork();

	/*! @brief Check whether current thread is the worker thread.
	@note Thread-safe.
	*/
	bool isWorkerThread() const;

	std::thread                 m_thread;
	TBlockableAtomicQueue<Work> m_workQueue;
	std::atomic_bool            m_isTerminationRequested;
};

}// end namespace ph

#include "Utility/Concurrent/TSingleThreadExecutor.ipp"
