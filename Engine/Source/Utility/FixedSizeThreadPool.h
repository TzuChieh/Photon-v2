#pragma once

#include "Utility/INoncopyable.h"
#include "Common/primitive_type.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

namespace ph
{

// TODO: templatize size
class FixedSizeThreadPool final : public INoncopyable
{
public:
	typedef std::function<void()> Work;

public:
	FixedSizeThreadPool(std::size_t numWorkers);
	~FixedSizeThreadPool();

	void queueWork(const Work& work);
	void queueWork(Work&& work);
	void requestExit();
	void waitAllWorks();

	inline std::size_t numWorkers() const
	{
		return m_workers.size();
	}

private:
	std::vector<std::thread> m_workers;
	std::queue<Work>         m_works;
	std::mutex               m_poolMutex;
	std::condition_variable  m_workersCv;
	std::condition_variable  m_allWorksDoneCv;
	bool                     m_isExitRequested;
	uint64                   m_numQueuedWorks;
	uint64                   m_numProcessedWorks;

	void asyncProcessWork();
};

}// end namespace ph