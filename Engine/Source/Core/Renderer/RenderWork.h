#pragma once

#include "Core/Filmic/filmic_fwd.h"
#include "Common/assertion.h"
#include "Core/Renderer/RenderProgress.h"


#include <atomic>
#include <cstddef>
#include <limits>

namespace ph
{

class RenderWork
{
public:
	RenderWork();
	RenderWork(const RenderWork& other);
	virtual ~RenderWork();

	void work();

	RenderProgress asyncGetProgress();

protected:
	void setTotalWork(std::size_t totalWork);
	void setWorkDone(std::size_t workDone);
	void incrementWorkDone();
	void setElapsedMs(std::size_t elapsedMs);

	RenderWork& operator = (const RenderWork& rhs);

private:
	virtual void doWork() = 0;

	std::atomic_uint32_t m_totalWork;
	std::atomic_uint32_t m_workDone;
	std::atomic_uint32_t m_elapsedMs;
};

// In-header Implementations:

inline RenderWork::RenderWork() : 
	m_totalWork(0),
	m_workDone (0),
	m_elapsedMs(0)
{}

inline RenderWork::RenderWork(const RenderWork& other) : 
	m_totalWork(other.m_totalWork.load(std::memory_order_relaxed)),
	m_workDone (other.m_workDone.load(std::memory_order_relaxed)),
	m_elapsedMs(other.m_elapsedMs.load(std::memory_order_relaxed))
{}

inline RenderWork::~RenderWork() = default;

inline void RenderWork::setTotalWork(const std::size_t totalWork)
{
	PH_ASSERT_LE(totalWork, std::numeric_limits<std::uint32_t>::max());

	m_totalWork.store(static_cast<std::uint32_t>(totalWork), std::memory_order_relaxed);
}

inline void RenderWork::setWorkDone(const std::size_t workDone)
{
	PH_ASSERT_LE(workDone, std::numeric_limits<std::uint32_t>::max());

	m_workDone.store(static_cast<std::uint32_t>(workDone), std::memory_order_relaxed);
}

inline void RenderWork::incrementWorkDone()
{
	m_workDone.fetch_add(1, std::memory_order_relaxed);
}

inline void RenderWork::setElapsedMs(const std::size_t elapsedMs)
{
	PH_ASSERT_LE(elapsedMs, std::numeric_limits<std::uint32_t>::max());

	m_elapsedMs.store(static_cast<std::uint32_t>(elapsedMs), std::memory_order_relaxed);
}

inline RenderProgress RenderWork::asyncGetProgress()
{
	return RenderProgress(
		m_totalWork.load(std::memory_order_relaxed),
		m_workDone.load(std::memory_order_relaxed),
		m_elapsedMs.load(std::memory_order_relaxed));
}

inline RenderWork& RenderWork::operator = (const RenderWork& rhs)
{
	m_totalWork = rhs.m_totalWork.load(std::memory_order_relaxed);
	m_workDone  = rhs.m_workDone.load(std::memory_order_relaxed);
	m_elapsedMs = rhs.m_elapsedMs.load(std::memory_order_relaxed);

	return *this;
}

}// end namespace ph
