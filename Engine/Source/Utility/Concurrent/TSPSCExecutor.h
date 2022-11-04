#pragma once

#include "Utility/Concurrent/TBlockableAtomicQueue.h"
#include "Utility/Concurrent/InitiallyPausedThread.h"
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
class TSPSCExecutor final
{
	static_assert(std::is_invocable_r_v<void, Work>,
		"Work must be callable as void(void)const.");

	static_assert(std::is_constructible_v<Work, decltype([]() -> void {})>,
		"Work must be constructible from a functor which is callable as void(void).");

public:
	/*! @brief Create an executor waiting for new work.
	*/
	inline TSPSCExecutor()
		requires std::default_initializable<Work>
		: TSPSCExecutor(Work())
	{}

	/*! @brief Create an executor waiting for new work.
	*/
	template<typename DeducedWork>
	inline explicit TSPSCExecutor(DeducedWork&& defaultWork)
		: m_thread                ()
		, m_workQueue             ()
		, m_isTerminationRequested()
		, m_waitWorksCv           ()
		, m_defaultWork           (std::forward<DeducedWork>(defaultWork))
		, m_producerThreadId      ()
	{
		m_thread = InitiallyPausedThread(
			[this]()
			{
				asyncProcessWork();
			});
	}

	/*! @brief Terminate the execution. Wait for any ongoing work to finish.
	*/
	~TSPSCExecutor();

	/*! @brief Start the execution of the consumer (work processor).
	Whichever thread calls this method will be the producer thread. Can only be called once in the
	lifetime of the executor.
	@note Thread-safe.
	*/
	void start();

	template<typename DeducedWork>
	void addWork(DeducedWork&& work);

	/*!
	@note Producer thread only.
	*/
	void waitAllWorks();

	/*! @brief Stop the executor.
	Executor will stop processing any work as soon as possible. Any work that is already being processed
	will still complete. No further write operations should be performed after requesting termination. 
	Requesting termination multiple times has the same effect.
	@note Producer thread only.
	*/
	void requestTermination();

	/*! @brief Get ID of the underlying thread.
	@note Thread-safe.
	*/
	std::thread::id getId() const;

private:
	/*! @brief Start processing works.
	@note Producer thread only.
	*/
	void asyncProcessWork();

	/*! @brief Check whether current thread is the worker thread.
	@note Thread-safe.
	*/
	bool isConsumerThread() const;

	/*! @brief Check whether current thread is the thread that calls start().
	@note Thread-safe.
	*/
	bool isProducerThread() const;

	/*! @brief Stop processing works.
	@note Thread-safe.
	*/
	void terminate();

	// NOTE: moodycamel has faster SPSC queue, consider using it.

	InitiallyPausedThread       m_thread;
	TBlockableAtomicQueue<Work> m_workQueue;
	std::atomic_flag            m_isTerminationRequested;
	std::condition_variable     m_waitWorksCv;
	std::thread::id             m_producerThreadId;

	// Worker-thread only fields
	Work m_defaultWork;
};

}// end namespace ph

#include "Utility/Concurrent/TSPSCExecutor.ipp"
