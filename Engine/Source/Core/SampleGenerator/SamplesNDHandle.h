#pragma once

#include "Math/TArithmeticArray.h"

#include <cstddef>
#include <utility>

namespace ph
{

class SamplesNDHandle final
{
	friend class SampleGenerator;

protected:
	explicit SamplesNDHandle(std::size_t stageIndex);

	std::size_t getStageIndex() const;

private:
	std::size_t m_stageIndex;
};

// In-header Implementations:

inline SamplesNDHandle::SamplesNDHandle(const std::size_t stageIndex) :
	m_stageIndex(stageIndex)
{}

inline std::size_t SamplesNDHandle::getStageIndex() const
{
	return m_stageIndex;
}

}// end namespace ph
