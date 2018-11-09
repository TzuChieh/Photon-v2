#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RendererProxy.h"
#include "Core/Renderer/Statistics.h"
#include "Core/Renderer/RenderProgress.h"

#include <atomic>
#include <utility>

namespace ph
{

class RenderWork;

class RenderWorker final
{
public:
	inline RenderWorker() = default;
	RenderWorker(const RendererProxy& renderer, uint32 id);
	RenderWorker(const RenderWorker& other);

	void run();
	void setWork(RenderWork* work);
	void setTotalWork(uint32 totalWork);
	void setWorkDone(uint32 workDone);
	void incrementWorkDone();
	uint32 getId() const;
	//RenderProgress asyncQueryProgress();

	RenderWorker& operator = (const RenderWorker& rhs);

private:
	RendererProxy        m_renderer;
	uint32               m_id;
	RenderWork*          m_work;
	std::atomic_uint32_t m_totalWork;
	std::atomic_uint32_t m_workDone;
	std::atomic_uint32_t m_numElapsedMs;

	void doWork();
};

// In-header Implementations:

inline uint32 RenderWorker::getId() const
{
	return m_id;
}

inline void RenderWorker::setTotalWork(const uint32 totalWork)
{
	m_totalWork = totalWork;
}

inline void RenderWorker::setWorkDone(const uint32 workDone)
{
	m_workDone = workDone;
}

inline void RenderWorker::incrementWorkDone()
{
	m_workDone++;
}

//inline RenderProgress RenderWorker::asyncQueryProgress()
//{
//	RenderProgress progress;
//	progress.totalWork = m_totalWork;
//	progress.workDone  = m_workDone;
//
//	return progress;
//}

}// end namespace ph