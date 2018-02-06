#include "Utility/FixedSizeThreadPool.h"

#include <iostream>

namespace ph
{
	
FixedSizeThreadPool::FixedSizeThreadPool(const std::size_t numThreads) : 
	m_workers(numThreads), m_works(), 
	m_poolMutex(), m_workersCv(), m_allWorksDoneCv(),
	m_isExitRequested(false),
	m_numQueuedWorks(0), m_numProcessedWorks(0)
{
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
	requestExit();
	waitAllWorks();

	// wait for works that are still processing to finish
	for(auto& worker : m_workers)
	{
		if(worker.joinable())
		{
			worker.join();
		}
	}
}

void FixedSizeThreadPool::queueWork(const Work& work)
{
	// exclusively access the work queue since it is also used by
	// worker threads
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		m_works.push(work);
		m_numQueuedWorks++;
	}

	m_workersCv.notify_all();
}

// Essentially the same as its const reference variant, except work is moved.
void FixedSizeThreadPool::queueWork(Work&& work)
{
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		m_works.push(std::move(work));
		m_numQueuedWorks++;
	}

	m_workersCv.notify_all();
}

void FixedSizeThreadPool::asyncProcessWork()
{
	std::unique_lock<std::mutex> lock(m_poolMutex);

	do
	{
		// wait until being notified there is new work yet to
		// be processed
		m_workersCv.wait(lock, [this]()
		{
			return !m_works.empty() || m_isExitRequested;
		});

		// we now own the lock after waiting
		if(!m_works.empty())
		{
			Work work = std::move(m_works.front());
			m_works.pop();

			// we are done using the work queue
			lock.unlock();

			work();

			// current thread must own the lock before calling wait(2)
			lock.lock();

			m_numProcessedWorks++;
			if(m_numQueuedWorks == m_numProcessedWorks)
			{
				m_allWorksDoneCv.notify_all();
			}
		}
	} while(!m_isExitRequested);
}

void FixedSizeThreadPool::requestExit()
{
	{
		std::lock_guard<std::mutex> lock(m_poolMutex);

		m_isExitRequested = true;
	}

	m_workersCv.notify_all();
}

void FixedSizeThreadPool::waitAllWorks()
{
	std::unique_lock<std::mutex> lock(m_poolMutex);

	// wait until being notified and all queued works are done
	m_allWorksDoneCv.wait(lock, [this]()
	{
		return m_numQueuedWorks == m_numProcessedWorks;
	});
}

}// end namespace ph