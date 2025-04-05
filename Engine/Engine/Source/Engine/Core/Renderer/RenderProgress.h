#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

class RenderProgress final
{
public:
	/*! @brief Represents no progress.
	*/
	RenderProgress();

	RenderProgress(
		uint64 totalWork, 
		uint64 workDone,
		uint64 elapsedMs);

	uint64 getTotalWork() const;
	uint64 getWorkDone() const;

	/*!
	@return Time spent for getting the work done. Whether the time will be in wall clock time or CPU time
	(or any other measurements) is up to the progress provider.
	*/
	uint64 getElapsedMs() const;

	real getNormalizedProgress() const;
	real getPercentageProgress() const;

	RenderProgress& operator += (const RenderProgress& rhs);

private:
	uint64 m_totalWork;
	uint64 m_workDone;
	uint64 m_elapsedMs;
};

// In-header Implementations:

inline RenderProgress::RenderProgress() : 
	RenderProgress(0, 0, 0)
{}

inline RenderProgress::RenderProgress(
	const uint64 totalWork,
	const uint64 workDone,
	const uint64 elapsedMs) :
	m_totalWork(totalWork),
	m_workDone(workDone),
	m_elapsedMs(elapsedMs)
{}

inline uint64 RenderProgress::getTotalWork() const
{
	return m_totalWork;
}

inline uint64 RenderProgress::getWorkDone() const
{
	return m_workDone;
}

inline uint64 RenderProgress::getElapsedMs() const
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
