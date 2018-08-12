#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class SampleGenerator;

template<typename T>
class TSamplePhase final
{
public:
	friend class SampleGenerator;

	TSamplePhase(const TSamplePhase& other) :
		m_phaseIndex(other.m_phaseIndex)
	{}

	inline TSamplePhase& operator = (const TSamplePhase& rhs)
	{
		m_phaseIndex = rhs.m_phaseIndex;

		return *this;
	}

private:
	std::size_t m_phaseIndex;

	inline explicit TSamplePhase(const std::size_t phaseIndex) :
		m_phaseIndex(phaseIndex)
	{}
};

}// end namespace ph