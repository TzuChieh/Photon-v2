#include "Utility/Concurrent/PeriodicSingleThreadExecutor.h"
#include "Common/assertion.h"
#include "Utility/Timer.h"

#include <utility>

namespace ph
{

PeriodicSingleThreadExecutor::PeriodicSingleThreadExecutor(const uint64 periodMS) :
	m_worker                (),
	m_works                 (),
	m_executorMutex         (),
	m_workerCv              (),
	m_isWorking             (false),
	m_isTerminationRequested(false),
	m_periodMS              (periodMS)
{
	m_worker = std::thread([this]()
	{
		asyncProcessWork();
	});
}

PeriodicSingleThreadExecutor::~PeriodicSingleThreadExecutor()
{
	requestTermination();

	// Wait for the worker that were still processing to finish
	if(m_worker.joinable())
	{
		m_worker.join();
	}
}

void PeriodicSingleThreadExecutor::addWork(const Work& work)
{
	// Exclusively access the work storage since it is also used by the worker thread
	{
		std::lock_guard<std::mutex> lock(m_executorMutex);

		PH_ASSERT(!m_isTerminationRequested);

		m_works.push_back(work);
	}

	m_workerCv.notify_one();
}

void PeriodicSingleThreadExecutor::addWork(Work&& work)
{
	// Exclusively access the work storage since it is also used by the worker thread
	{
		std::lock_guard<std::mutex> lock(m_executorMutex);

		PH_ASSERT(!m_isTerminationRequested);

		m_works.push_back(work);
	}

	m_workerCv.notify_one();
}

void PeriodicSingleThreadExecutor::asyncProcessWork()
{
	std::unique_lock<std::mutex> lock(m_executorMutex);

	while(true)
	{
		// Wait if the worker should not work; until being notified that the worker should work again
		m_workerCv.wait(lock, [this]()
		{
			return m_isWorking || m_isTerminationRequested;
		});
		
		if(m_isTerminationRequested)
		{
			break;
		}

		

		

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
	}
}

void PeriodicSingleThreadExecutor::pause()
{
	std::unique_lock<std::mutex> lock(m_executorMutex);

	PH_ASSERT(!m_isTerminationRequested);
	PH_ASSERT(!isWorkerThread());

	m_isWorking = false;
}

void PeriodicSingleThreadExecutor::resume()
{
	{
		std::lock_guard<std::mutex> lock(m_executorMutex);

		PH_ASSERT(!m_isTerminationRequested);
		PH_ASSERT(!isWorkerThread());

		m_isWorking = true;
	}

	m_workerCv.notify_one();
}

void PeriodicSingleThreadExecutor::requestTermination()
{
	{
		std::lock_guard<std::mutex> lock(m_executorMutex);

		m_isTerminationRequested = true;
	}

	m_workerCv.notify_one();
}

}// end namespace ph
