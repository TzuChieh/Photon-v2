#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <cstddef>

namespace ph
{

class RenderProgress
{
public:
	// Represents no progress.
	RenderProgress();

	RenderProgress(
		std::size_t totalWork, 
		std::size_t workDone,
		std::size_t elapsedMs);

	std::size_t getTotalWork() const;
	std::size_t getWorkDone() const;
	std::size_t getElapsedMs() const;
	real getNormalizedProgress() const;
	real getPercentageProgress() const;

	RenderProgress& operator += (const RenderProgress& rhs);

private:
	std::size_t m_totalWork;
	std::size_t m_workDone;
	std::size_t m_elapsedMs;
};

// In-header Implementations:

inline RenderProgress::RenderProgress() : 
	RenderProgress(0, 0, 0)
{}

inline RenderProgress::RenderProgress(
	const std::size_t totalWork, 
	const std::size_t workDone,
	const std::size_t elapsedMs) : 
	m_totalWork(totalWork),
	m_workDone(workDone),
	m_elapsedMs(elapsedMs)
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

inline std::size_t RenderProgress::getElapsedMs() const
{
	return m_elapsedMs;
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
	m_elapsedMs += rhs.m_elapsedMs;

	return *this;
}

}// end namespace ph