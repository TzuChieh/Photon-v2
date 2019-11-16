#pragma once

#include "Math/TArithmeticArray.h"

#include <cstddef>
#include <utility>

namespace ph
{

template<std::size_t N>
class TSamplesNDHandle final
{
	friend class SampleGenerator;

protected:
	explicit TSamplesNDHandle(std::size_t stageIndex);

	std::size_t getStageIndex() const;

private:
	std::size_t m_stageIndex;
};

// In-header Implementations:

template<std::size_t N>
inline TSamplesNDHandle<N>::TSamplesNDHandle(const std::size_t stageIndex) :
	m_stageIndex(stageIndex)
{}

template<std::size_t N>
inline std::size_t TSamplesNDHandle<N>::getStageIndex() const
{
	return m_stageIndex;
}

}// end namespace ph
