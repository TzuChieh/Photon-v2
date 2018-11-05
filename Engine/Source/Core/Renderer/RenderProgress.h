#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <cstddef>

namespace ph
{

class RenderProgress
{
public:
	RenderProgress(std::size_t totalWork, std::size_t workDone);

	std::size_t getTotalWork() const;
	std::size_t getWorkDone() const;
	real getNormalizedProgress() const;
	real getPercentageProgress() const;

	RenderProgress& operator += (const RenderProgress& rhs);

private:
	std::size_t m_totalWork;
	std::size_t m_workDone;
};

// In-header Implementations:

inline RenderProgress::RenderProgress(const std::size_t totalWork, const std::size_t workDone) : 
	m_totalWork(totalWork),
	m_workDone(workDone)
{
	PH_ASSERT_GE(m_totalWork, m_workDone);
}

inline std::size_t RenderProgress::getTotalWork() const
{
	return m_totalWork;
}

inline std::size_t RenderProgress::getWorkDone() const
{
	return m_workDone;
}

inline real RenderProgress::getNormalizedProgress() const
{
	if(m_totalWork != 0)
	{
		return static_cast<real>(m_workDone) / static_cast<real>(m_totalWork);
	}
	else
	{
		return 0.0_r;
	}
}

inline real RenderProgress::getPercentageProgress() const
{
	return getNormalizedProgress() * 100.0_r;
}

inline RenderProgress& RenderProgress::operator += (const RenderProgress& rhs)
{
	m_totalWork += rhs.m_totalWork;
	m_workDone  += rhs.m_workDone;

	return *this;
}

}// end namespace ph