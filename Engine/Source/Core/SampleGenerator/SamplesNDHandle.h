#pragma once

#include "Math/TArithmeticArray.h"
#include "Common/assertion.h"

#include <cstddef>
#include <utility>

namespace ph
{

class SamplesNDHandle final
{
	friend class SampleGenerator;

protected:
	SamplesNDHandle(std::size_t stageIndex, std::size_t numDims);

	std::size_t getStageIndex() const;
	std::size_t numDims() const;

private:
	std::size_t m_stageIndex;
	std::size_t m_numDims;
};

// In-header Implementations:

inline SamplesNDHandle::SamplesNDHandle(const std::size_t stageIndex, const std::size_t numDims) :
	m_stageIndex(stageIndex), m_numDims(numDims)
{
	PH_ASSERT_GT(m_numDims, 0);
}

inline std::size_t SamplesNDHandle::getStageIndex() const
{
	return m_stageIndex;
}

inline std::size_t SamplesNDHandle::numDims() const
{
	return m_numDims;
}

}// end namespace ph
