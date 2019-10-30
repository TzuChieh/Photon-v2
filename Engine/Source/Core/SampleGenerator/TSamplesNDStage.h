#pragma once

#include "Math/TArithmeticArray.h"

#include <cstddef>
#include <utility>

namespace ph
{

template<std::size_t N>
class TSamplesNDStage final
{
public:
	using SizeHints = math::TArithmeticArray<std::size_t, N>;

	TSamplesNDStage(std::size_t stageIndex, std::size_t numSamples);
	TSamplesNDStage(
		std::size_t stageIndex, 
		std::size_t numSamples, 
		SizeHints   dimSizeHints);

private:
	std::size_t  m_stageIndex;
	std::size_t  m_numSamples;
	SizeHints    m_dimSizeHints;
};

// In-header Implementations:

template<std::size_t N>
inline TSamplesNDStage<N>::TSamplesNDStage(const std::size_t stageIndex, const std::size_t numSamples) :
	TSamplesNDStage(stageIndex, numSamples, SizeHints(1))
{}

template<std::size_t N>
inline TSamplesNDStage<N>::TSamplesNDStage(
	const std::size_t stageIndex, 
	const std::size_t numSamples,
	SizeHints         dimSizeHints) :

	m_stageIndex  (stageIndex), 
	m_numSamples  (numSamples),
	m_dimSizeHints(std::move(dimSizeHints))
{}

}// end namespace ph
