#pragma once

#include "Common/primitive_type.h"
#include "Core/SampleGenerator/EPhaseType.h"

namespace ph
{

class SampleGenerator;

template<typename T>
class TSamplePhase final
{
public:
	friend class SampleGenerator;

	TSamplePhase(const TSamplePhase& other) :
		m_phaseIndex(other.m_phaseIndex), m_numElements(other.m_numElements)
	{

	}

	inline std::size_t numElements() const
	{
		return m_numElements;
	}

	inline TSamplePhase& operator = (const TSamplePhase& rhs)
	{
		m_phaseIndex  = rhs.m_phaseIndex;
		m_numElements = rhs.m_numElements;
		return *this;
	}

private:
	uint32      m_phaseIndex;
	std::size_t m_numElements;

	inline TSamplePhase(uint32 phaseIndex, std::size_t numElements) :
		m_phaseIndex(phaseIndex), m_numElements(numElements)
	{

	}
};

}// end namespace ph