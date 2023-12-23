#include "Utility/Concurrent/FixedSizeThreadPool.h"

#include <Common/assertion.h>

#include <utility>

/*
The design is similar to the pool introduced in
https://embeddedartistry.com/blog/2017/02/08/implementing-an-asynchronous-dispatch-queue/
*/

namespace ph
{
	
FixedSizeThreadPool::FixedSizeThreadPool(const std::size_t numWorkers) :
	m_workers               (numWorkers),
	m_works                 (), 
	m_poolMutex             (), 
	m_workersCv             (), 
	m_allWorksDoneCv        (),
	m_isTerminationRequested(false),
	m_numUnfinishedWorks    (0)
{
	PH_ASSERT_GT(numWorkers, 0);

	for(auto& worker : m_workers)
	{
		worker = std::thread([this]()
		{
			asyncProcessWork();
		});
	}
}

FixedSizeThreadPool::~FixedSizeThreadPool()
{
	requestTermination();

	// Wait for any workers that are still processing to finish
	for(auto& worker : m_workers)
	{
		// Not joining default-constructed thread and already-joined thread
		if(worker.joinable())
		{
			worker.join();
		}
	}
}

void FixedSizeThreadPool::queueWork(const Work& work)
{
	// Exclusively access the work queue since it is also used by worker threads
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		PH_ASSERT(!m_isTerminationRequested);

		m_works.push(work);
		++m_numUnfinishedWorks;
	}

	m_workersCv.notify_one();
}

// Essentially the same as its const reference variant, except work is moved.
void FixedSizeThreadPool::queueWork(Work&& work)
{
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		PH_ASSERT(!m_isTerminationRequested);

		m_works.push(std::move(work));
		++m_numUnfinishedWorks;
	}

	m_workersCv.notify_one();
}

void FixedSizeThreadPool::asyncProcessWork()
{
	std::unique_lock<std::mutex> lock(m_poolMutex);

	do
	{
		// Wait until being notified there is new work yet to be processed
		m_workersCv.wait(lock, [this]()
		{
			return !m_works.empty() || m_isTerminationRequested;
		});

		// We now own the lock after waiting
		if(!m_works.empty() && !m_isTerminationRequested)
		{
			Work work = std::move(m_works.front());
			m_works.pop();

			// We are done using the work queue
			lock.unlock();

			work();

			// Current thread must own the lock before calling wait(2)
			lock.lock();

			PH_ASSERT_GT(m_numUnfinishedWorks, m_works.size());

			--m_numUnfinishedWorks;
			if(m_numUnfinishedWorks == 0)
			{
				m_allWorksDoneCv.notify_all();
			}
		}
	} while(!m_isTerminationRequested);
}

void FixedSizeThreadPool::requestTermination()
{
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		m_isTerminationRequested = true;
	}

	m_workersCv.notify_all();
	m_allWorksDoneCv.notify_all();
}

void FixedSizeThreadPool::waitAllWorks()
{
	std::unique_lock<std::mutex> lock(m_poolMutex);

	PH_ASSERT(!m_isTerminationRequested);
	PH_ASSERT(!isWorkerThread());

	// Wait until being notified that all queued works are done
	m_allWorksDoneCv.wait(lock, [this]()
	{
		return m_numUnfinishedWorks == 0 || m_isTerminationRequested;
	});
}

}// end namespace ph
