#include "Utility/Concurrent/PeriodicSingleThreadExecutor.h"
#include "Common/assertion.h"
#include "Utility/Timer.h"

#include <utility>
#include <chrono>

namespace ph
{

PeriodicSingleThreadExecutor::PeriodicSingleThreadExecutor(const uint64 periodMs) :
	m_worker                (),
	m_works                 (),
	m_executorMutex         (),
	m_workerCv              (),
	m_isWorking             (false),
	m_isTerminationRequested(false),
	m_periodMs              (periodMs)
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
	Timer timer;
	timer.start();

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

		for(const Work& work : m_works)
		{
			work();
		}

		// We are done using the work storage
		lock.unlock();

		timer.stop();
		const uint64 deltaMs = timer.getDeltaMs();
		timer.start();

		const uint64 periodMs = m_periodMs.load(std::memory_order_relaxed);
		if(periodMs > deltaMs)
		{
			const std::chrono::milliseconds sleepTimeMs(periodMs - deltaMs);
			std::this_thread::sleep_for(sleepTimeMs);
		}

		// Current thread must own the lock before calling wait(2)
		lock.lock();
	}
}

void PeriodicSingleThreadExecutor::pause()
{
	std::lock_guard<std::mutex> lock(m_executorMutex);

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
