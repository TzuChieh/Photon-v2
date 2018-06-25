#pragma once

#include "Core/Filmic/filmic_fwd.h"
#include "Common/assertion.h"
#include "Core/Renderer/RenderProgress.h"

#include <atomic>

namespace ph
{

class RenderWorker;

class RenderWork
{
public:
	RenderWork();
	virtual ~RenderWork() = 0;

	virtual void doWork() = 0;

	void setWorker(RenderWorker* worker);
	void setTotalWork(uint32 totalWork);
	void setWorkDone(uint32 workDone);
	void incrementWorkDone();
	RenderProgress asyncQueryProgress();

private:
	RenderWorker*        m_worker;
	std::atomic_uint32_t m_totalWork;
	std::atomic_uint32_t m_workDone;
};

// In-header Implementations:

inline RenderWork::RenderWork() = default;

inline RenderWork::~RenderWork() = default;

inline void RenderWork::setWorker(RenderWorker* const worker)
{
	PH_ASSERT(worker);

	m_worker = worker;
}

inline void RenderWork::setTotalWork(const uint32 totalWork)
{
	m_totalWork = totalWork;
}

inline void RenderWork::setWorkDone(const uint32 workDone)
{
	m_workDone = workDone;
}

inline void RenderWork::incrementWorkDone()
{
	m_workDone++;
}

inline RenderProgress RenderWork::asyncQueryProgress()
{
	RenderProgress progress;
	progress.totalWork = m_totalWork;
	progress.workDone  = m_workDone;

	return progress;
}

}// end namespace ph