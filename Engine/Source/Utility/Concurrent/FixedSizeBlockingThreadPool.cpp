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
	m_isTerminationRequested()
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
	}

	m_workersCv.notify_one();
}

// Essentially the same as its const reference variant, except work is moved.
void FixedSizeBlockingThreadPool::queueWork(Work&& work)
{
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		m_works.push(std::move(work));
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
			return !m_works.empty() || isTerminationRequested();
		});

		// We now own the lock after waiting
		if(!m_works.empty() && !isTerminationRequested())
		{
			Work work = std::move(m_works.front());
			m_works.pop();

			// We are done using the work queue
			lock.unlock();

			work();

			// Current thread must own the lock before calling wait(2)
			lock.lock();

			if(m_works.empty())
			{
				m_allWorksDoneCv.notify_all();
			}
		}
	} while(!isTerminationRequested());
}

void FixedSizeBlockingThreadPool::requestTermination()
{
	m_isTerminationRequested.test_and_set(std::memory_order_relaxed);

	m_workersCv.notify_all();
}

void FixedSizeBlockingThreadPool::waitAllWorks()
{
	std::unique_lock<std::mutex> lock(m_poolMutex);

	PH_ASSERT(!isTerminationRequested());

	if(!m_works.empty())
	{
		// Wait until being notified that all queued works are done
		m_allWorksDoneCv.wait(lock, [this]()
		{
			return m_works.empty();
		});
	}

	PH_ASSERT(m_works.empty());
}

}// end namespace ph
