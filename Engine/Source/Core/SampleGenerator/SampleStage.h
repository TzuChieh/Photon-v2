#pragma once

#include "Common/assertion.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace ph
{

class SampleStage final
{
public:
	using SizeHints = std::vector<std::size_t>;

	static SizeHints makeIdentityDimSizeHints(std::size_t numDimensions);

	SampleStage(
		std::size_t sampleIndex, 
		std::size_t numDimensions,
		std::size_t numSamples);

	SampleStage(
		std::size_t sampleIndex,
		std::size_t numDimensions,
		std::size_t numSamples,
		SizeHints   dimSizeHints);

	std::size_t getSampleIndex() const;
	std::size_t numDimensions() const;
	std::size_t numSamples() const;
	std::size_t numElements() const;
	const SizeHints& getDimSizeHints() const;

private:
	std::size_t m_sampleIndex;
	std::size_t m_numSamples;
	std::size_t m_numDimensions;
	SizeHints   m_dimSizeHints;
};

// In-header Implementations:

inline SampleStage::SampleStage(
	const std::size_t stageIndex,
	const std::size_t numDimensions,
	const std::size_t numSamples) :

	SampleStage(
		stageIndex, 
		numDimensions, 
		numSamples, 
		makeIdentityDimSizeHints(numDimensions))
{}

inline SampleStage::SampleStage(
	const std::size_t sampleIndex,
	const std::size_t numDimensions,
	const std::size_t numSamples,
	SizeHints         dimSizeHints) :

	m_sampleIndex  (sampleIndex),
	m_numDimensions(numDimensions),
	m_numSamples   (numSamples),
	m_dimSizeHints (std::move(dimSizeHints))
{
	PH_ASSERT_GT(numDimensions, 0);
}

inline std::size_t SampleStage::getSampleIndex() const
{
	return m_sampleIndex;
}

inline std::size_t SampleStage::numDimensions() const
{
	return m_numDimensions;
}

inline std::size_t SampleStage::numSamples() const
{
	return m_numSamples;
}

inline std::size_t SampleStage::numElements() const
{
	return m_numDimensions * m_numSamples;
}

inline auto SampleStage::getDimSizeHints() const
	-> const SizeHints&
{
	return m_dimSizeHints;
}

inline auto SampleStage::makeIdentityDimSizeHints(const std::size_t numDimensions)
	-> SizeHints
{
	PH_ASSERT_GT(numDimensions, 0);

	// let each dimension equally sized
	return SizeHints(numDimensions, 1);
}

}// end namespace ph
