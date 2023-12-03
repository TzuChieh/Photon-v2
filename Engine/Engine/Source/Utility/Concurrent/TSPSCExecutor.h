#pragma once

#include "Utility/Concurrent/TBlockableAtomicQuasiQueue.h"
#include "Utility/Concurrent/InitiallyPausedThread.h"
#include "Common/primitive_type.h"
#include "Utility/TFunction.h"

#include <type_traits>
#include <thread>
#include <utility>
#include <atomic>
#include <functional>
#include <variant>

namespace ph
{

/*! @brief A single-producer, single-consumer worker thread.
Ctor and dtor are not thread safe. Most of the methods are either producer thread only, or thread-safe.
`start()` must be called before adding work since it also serves as a mean to determine the producer
thread. Setters for function callbacks can only be called before `start()`. Note that if work processor
is not set from ctor, `setWorkProcessor()` must be called with a valid processor function.
@tparam Work Type of the work to be processed.
*/
template<typename Work>
class TSPSCExecutor final
{
public:
	/*! @brief Create an executor waiting for new work.
	*/
	inline TSPSCExecutor()
		: TSPSCExecutor(nullptr)
	{}

	/*! @brief Create an executor waiting for new work.
	*/
	inline explicit TSPSCExecutor(std::function<void(const Work& work)> workProcessor)
		: m_thread                ()
		, m_workloadQueue         ()
		, m_isTerminationRequested()
		, m_isTerminated          ()
		, m_producerThreadId      ()
		, m_workProcessor         (std::move(workProcessor))
		, m_onConsumerStart       ()
		, m_onConsumerTerminate   ()
	{
		m_thread = InitiallyPausedThread(
			[this]()
			{
				asyncExecute();
			});
	}

	/*! @brief Terminate the execution. Wait for any ongoing work to finish.
	*/
	~TSPSCExecutor();

	/*! @brief Setters for callbacks.
	Callbacks will execute on consumer thread.
	@note Can only be set before start().
	*/
	///@{
	void setWorkProcessor(std::function<void(const Work& work)> workProcessor);
	void setOnConsumerStart(std::function<void(void)> onConsumerStart);
	void setOnConsumerTerminate(std::function<void(void)> onConsumerTerminate);
	///@}

	/*! @brief Start the execution of the consumer (worker thread).
	Whichever thread calls this method will be the producer thread. Can only be called once in the
	lifetime of the executor.
	@note The thread that calls this method is considered the producer thread.
	*/
	void start();

	/*!
	Basic aquire-release semantics are guaranteed. This ensures that all the effects of work done 
	by the producer thread before it enqueues a work will be visible on the consumer thread after it 
	dequeues that work.
	@note Producer thread only.
	*/
	template<typename DeducedWork>
	void addWork(DeducedWork&& work);

	// TODO: bulk work addition

	/*! @brief Wait for all queued works to finish.
	Memory effects on consumer thread are made visible to the producer thread.
	@note Producer thread only.
	*/
	void waitAllWorks();

	/*! @brief Stop the executor.
	Executor will stop processing any work as soon as possible. Any work that is already being 
	processed will still complete. No further write operations should be performed after 
	requesting termination. Requesting termination multiple times has the same effect.
	@note Producer thread only.
	*/
	void requestTermination();

	/*! @brief Wait for the executor to stop.
	All operations on the consumer thread will be done after this call returns. Memory effects on 
	consumer thread are made visible to the producer thread. Cannot be called on consumer thread 
	as this can lead to deadlock.
	@note Thread-safe.
	*/
	void waitForTermination();

	/*! @brief Get ID of the underlying thread.
	@note Thread-safe.
	*/
	std::thread::id getId() const;

	/*! @brief Whether the executor has started processing works.
	@note Producer and consumer thread only.
	*/
	bool hasStarted() const;

private:
	/*! @brief Start the execution of the consumer thread.
	@note Consumer thread only.
	*/
	void asyncExecute();

	/*! @brief Start processing works.
	@note Consumer thread only.
	*/
	void asyncProcessWorks();

	/*! @brief Check whether current thread is the worker thread.
	@note Thread-safe.
	*/
	bool isConsumerThread() const;

	/*! @brief Check whether current thread is the thread that calls start().
	@note Thread-safe.
	*/
	bool isProducerThread() const;

	/*! @brief Stop processing works.
	*/
	void terminate();

private:
	// Callable type for internal usages. Warpping with a custom private type so we cannot mix 
	// it with user types
	struct CustomCallable
	{
		TFunction<void(void), 0> callable;
	};

	// Possibly store both user-specified work and custom callables for internal usages
	using Workload = std::variant<
		std::monostate,
		Work,
		CustomCallable>;

	// NOTE: moodycamel has faster SPSC queue, consider using it

	InitiallyPausedThread                 m_thread;
	TBlockableAtomicQuasiQueue<Workload>  m_workloadQueue;
	std::atomic_flag                      m_isTerminationRequested;
	std::atomic_flag                      m_isTerminated;
	std::thread::id                       m_producerThreadId;

	// Worker-thread only fields
	std::function<void(const Work& work)> m_workProcessor;
	std::function<void(void)>             m_onConsumerStart;
	std::function<void(void)>             m_onConsumerTerminate;
};

}// end namespace ph

#include "Utility/Concurrent/TSPSCExecutor.ipp"
