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

	virtual void doWork() = 0;

	RenderProgress asyncGetProgress();

protected:
	void setTotalWork(std::size_t totalWork);
	void setWorkDone(std::size_t workDone);
	void incrementWorkDone();

	RenderWork& operator = (const RenderWork& rhs);

private:
	std::atomic_uint32_t m_totalWork;
	std::atomic_uint32_t m_workDone;
};

// In-header Implementations:

inline RenderWork::RenderWork() : 
	m_totalWork(0),
	m_workDone(0)
{}

inline RenderWork::RenderWork(const RenderWork& other) : 
	m_totalWork(other.m_totalWork.load()),
	m_workDone(other.m_workDone.load())
{}

inline RenderWork::~RenderWork() = default;

inline void RenderWork::setTotalWork(const std::size_t totalWork)
{
	PH_ASSERT_LE(totalWork, std::numeric_limits<std::uint32_t>::max());

	m_totalWork = static_cast<std::uint32_t>(totalWork);
}

inline void RenderWork::setWorkDone(const std::size_t workDone)
{
	PH_ASSERT_LE(workDone, std::numeric_limits<std::uint32_t>::max());

	m_workDone = static_cast<std::uint32_t>(workDone);
}

inline void RenderWork::incrementWorkDone()
{
	PH_ASSERT_LT(m_workDone, m_totalWork);

	++m_workDone;
}

inline RenderProgress RenderWork::asyncGetProgress()
{
	return RenderProgress(m_totalWork, m_workDone);
}

inline RenderWork& RenderWork::operator = (const RenderWork& rhs)
{
	m_totalWork = rhs.m_totalWork.load();
	m_workDone  = rhs.m_workDone.load();

	return *this;
}

}// end namespace ph