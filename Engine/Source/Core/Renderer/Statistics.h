#pragma once

#include "Common/primitive_type.h"

#include <atomic>

namespace ph
{

class Statistics final
{
public:
	class Record final
	{
	public:
		uint32 totalWork;
		uint32 workDone;
		uint32 numSamplesTaken;
		uint32 numMsElapsed;

		inline Record() : totalWork(0), workDone(0), numSamplesTaken(0), numMsElapsed(0) {}
	};

public:
	inline Statistics() :
		m_totalWork(0), m_workDone(0), m_numSamplesTaken(0), m_numMsElapsed(0)
	{}

	inline Statistics(const Statistics& other) :
		m_totalWork      (other.m_totalWork.load()), 
		m_workDone       (other.m_workDone.load()), 
		m_numSamplesTaken(other.m_numSamplesTaken.load()), 
		m_numMsElapsed   (other.m_numMsElapsed.load())
	{}

	inline void setTotalWork(const uint32 totalWork)
	{
		m_totalWork = totalWork;
	}

	inline void setWorkDone(const uint32 workDone)
	{
		m_workDone = workDone;
	}

	inline void setNumSamplesTaken(const uint32 numSamples)
	{
		m_numSamplesTaken = numSamples;
	}

	inline void setNumMsElapsed(const uint32 numMsElapsed)
	{
		m_numMsElapsed = numMsElapsed;
	}

	inline void incrementWorkDone()
	{
		++m_workDone;
	}

	inline Record record() const
	{
		Record record;
		record.totalWork       = m_totalWork;
		record.workDone        = m_workDone;
		record.numSamplesTaken = m_numSamplesTaken;
		record.numMsElapsed    = m_numMsElapsed;

		return record;
	}

	inline Statistics& operator = (const Statistics& rhs)
	{
		m_totalWork       = rhs.m_totalWork.load();
		m_workDone        = rhs.m_workDone.load();
		m_numSamplesTaken = rhs.m_numSamplesTaken.load();
		m_numMsElapsed    = rhs.m_numMsElapsed.load();

		return *this;
	}

private:
	std::atomic_uint32_t m_totalWork;
	std::atomic_uint32_t m_workDone;
	std::atomic_uint32_t m_numSamplesTaken;
	std::atomic_uint32_t m_numMsElapsed;
};

}// end namespace ph