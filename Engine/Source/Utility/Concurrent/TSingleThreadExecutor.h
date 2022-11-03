#pragma once

#include "Utility/Concurrent/TBlockableAtomicQueue.h"
#include "Common/primitive_type.h"

#include <type_traits>
#include <thread>
#include <concepts>
#include <utility>
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace ph
{

template<typename Work>
class TSingleThreadExecutor final
{
	static_assert(std::is_invocable_r_v<void, Work>,
		"Work must be callable as void(void)const.");

	static_assert(std::is_constructible_v<Work, decltype([]() -> void {})>,
		"Work must be constructible from a functor which is callable as void(void).");

public:
	/*! @brief Create an executor waiting for new work.
	*/
	inline TSingleThreadExecutor()
		requires std::default_initializable<Work>
		: TSingleThreadExecutor(Work())
	{}

	/*! @brief Create an executor waiting for new work.
	*/
	template<typename DeducedWork>
	inline explicit TSingleThreadExecutor(DeducedWork&& defaultWork)
		: m_thread                ()
		, m_workQueue             ()
		, m_isTerminationRequested()
		, m_numWaitingThreads     (0)
		, m_executorMutex         ()
		, m_waitWorksCv           ()
		, m_defaultWork           (std::forward<DeducedWork>(defaultWork))
	{
		m_thread = std::thread(
			[this]()
			{
				asyncProcessWork();
			});
	}

	/*! @brief Terminate the execution. Wait for any ongoing work to finish.
	*/
	~TSingleThreadExecutor();

	template<typename DeducedWork>
	void addWork(DeducedWork&& work);

	void waitAllWorksFromThisThread();

	/*! @brief Stop the executor.
	Worker will stop processing any work as soon as possible. Any work that is already being processed
	will still complete. No further write operations should be performed after requesting termination. 
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
	std::atomic_flag            m_isTerminationRequested;
	uint32                      m_numWaitingThreads;
	std::mutex                  m_executorMutex;
	std::condition_variable     m_waitWorksCv;

	// Worker-thread only fields
	Work m_defaultWork;
};

}// end namespace ph

#include "Utility/Concurrent/TSingleThreadExecutor.ipp"
