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

	std::size_t getStageIndex() const;
	std::size_t numSamples() const;
	std::size_t numElements() const;
	const SizeHints& getDimSizeHints() const;

private:
	std::size_t m_stageIndex;
	std::size_t m_numSamples;
	SizeHints   m_dimSizeHints;
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

template<std::size_t N>
inline std::size_t TSamplesNDStage<N>::getStageIndex() const
{
	return m_stageIndex;
}

template<std::size_t N>
inline std::size_t TSamplesNDStage<N>::numSamples() const
{
	return m_numSamples;
}

template<std::size_t N>
inline std::size_t TSamplesNDStage<N>::numElements() const
{
	return m_numSamples * N;
}

template<std::size_t N>
inline auto TSamplesNDStage<N>::getDimSizeHints() const
	-> const SizeHints&
{
	return m_dimSizeHints;
}

}// end namespace ph
