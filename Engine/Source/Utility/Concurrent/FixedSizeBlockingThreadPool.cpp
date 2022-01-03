#include "Utility/Concurrent/FixedSizeBlockingThreadPool.h"
#include "Common/assertion.h"

/*
The design is similar to the pool introduced in
https://embeddedartistry.com/blog/2017/02/08/implementing-an-asynchronous-dispatch-queue/
*/

namespace ph
{
	
FixedSizeBlockingThreadPool::FixedSizeBlockingThreadPool(const std::size_t numWorkers) :
	m_workers               (numWorkers),
	m_works                 (), 
	m_poolMutex             (), 
	m_workersCv             (), 
	m_allWorksDoneCv        (),
	m_isTerminationRequested(false),
	m_numQueuedWorks        (0), 
	m_numProcessedWorks     (0)
{
	for(auto& worker : m_workers)
	{
		worker = std::thread([this]()
		{
			asyncProcessWork();
		});
	}
}

FixedSizeBlockingThreadPool::~FixedSizeBlockingThreadPool()
{
	requestTermination();

	// Wait for any workers that are still processing to finish
	for(auto& worker : m_workers)
	{
		if(worker.joinable())
		{
			worker.join();
		}
	}
}

void FixedSizeBlockingThreadPool::queueWork(const Work& work)
{
	// Exclusively access the work queue since it is also used by worker threads
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		m_works.push(work);
		m_numQueuedWorks++;
	}

	m_workersCv.notify_one();
}

// Essentially the same as its const reference variant, except work is moved.
void FixedSizeBlockingThreadPool::queueWork(Work&& work)
{
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		m_works.push(std::move(work));
		m_numQueuedWorks++;
	}

	m_workersCv.notify_one();
}

void FixedSizeBlockingThreadPool::asyncProcessWork()
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

			PH_ASSERT_GT(m_numQueuedWorks, m_numProcessedWorks);

			m_numProcessedWorks++;
			if(m_numQueuedWorks == m_numProcessedWorks)
			{
				m_allWorksDoneCv.notify_all();
			}
		}
	} while(!m_isTerminationRequested);
}

void FixedSizeBlockingThreadPool::requestTermination()
{
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		m_isTerminationRequested = true;
	}

	m_workersCv.notify_all();
}

void FixedSizeBlockingThreadPool::waitAllWorks()
{
	std::unique_lock<std::mutex> lock(m_poolMutex);

	PH_ASSERT(!m_isTerminationRequested);

	if(m_numQueuedWorks != m_numProcessedWorks)
	{
		PH_ASSERT_GT(m_numQueuedWorks, m_numProcessedWorks);

		// Wait until being notified that all queued works are done
		m_allWorksDoneCv.wait(lock, [this]()
		{
			return m_numQueuedWorks == m_numProcessedWorks;
		});
	}

	PH_ASSERT_EQ(m_numQueuedWorks, m_numProcessedWorks);
}

}// end namespace ph
