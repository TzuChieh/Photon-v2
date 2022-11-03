#include "Utility/Concurrent/TSingleThreadExecutor.h"
#include "Common/assertion.h"

#include <condition_variable>

namespace ph
{

template<typename Work>
inline TSingleThreadExecutor<Work>::~TSingleThreadExecutor()
{
	// TODO
}

template<typename Work>
template<typename DeducedWork>
inline void TSingleThreadExecutor<Work>::addWork(DeducedWork&& work)
{
	m_workQueue.enqueue(std::forward<DeducedWork>(work));
}

template<typename Work>
inline void TSingleThreadExecutor<Work>::waitAllWorksFromThisThread()
{
	// As the work queue is FIFO when observed from any single thread, to wait all works added by this
	// thread to finish, we add a new work and wait for it to finish (could be slower because we have
	// one extra work to process, but generally good enough)
	
	addWork();

	// TODO
}

template<typename Work>
inline void TSingleThreadExecutor<Work>::requestTermination()
{
	// TODO
}

template<typename Work>
inline std::thread::id TSingleThreadExecutor<Work>::getId() const
{
	PH_ASSERT(m_thread.get_id() != std::thread::id());

	return m_thread.get_id();
}

template<typename Work>
inline void TSingleThreadExecutor<Work>::asyncProcessWork()
{
	PH_ASSERT(isWorkerThread());

	while(!isTerminationRequested_workerThread())
	{
		Work& currentWork = m_defaultWork;
		m_workQueue.waitDequeue(&currentWork);

		currentWork();
	}

	PH_ASSERT(isTerminationRequested_workerThread());
	while(m_numWaitingThreads.load(std::memory_order_release))

	// TODO
}

template<typename Work>
inline bool TSingleThreadExecutor<Work>::isWorkerThread() const
{
	return getId() == std::this_thread::get_id();
}

template<typename Work>
inline void TSingleThreadExecutor<Work>::requestTermination_workerThread()
{
	PH_ASSERT(isWorkerThread());

	m_isTerminationRequested = true;
}

template<typename Work>
inline bool TSingleThreadExecutor<Work>::isTerminationRequested_workerThread() const
{
	PH_ASSERT(isWorkerThread());

	return m_isTerminationRequested;
}

}// end namespace ph
