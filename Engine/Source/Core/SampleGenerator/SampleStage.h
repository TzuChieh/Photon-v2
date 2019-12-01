#pragma once

#include "Common/assertion.h"

#include <cstddef>
#include <utility>
#include <vector>
#include <string>

namespace ph
{

class SampleStage final
{
public:
	using SizeHints = std::vector<std::size_t>;

	static SizeHints makeIdentityDimSizeHints(std::size_t numDims);

	SampleStage(
		std::size_t sampleIndex,
		std::size_t numDims,
		std::size_t numSamples);

	SampleStage(
		std::size_t sampleIndex,
		std::size_t numDimensions,
		std::size_t numSamples,
		SizeHints   dimSizeHints);

	void setNumSamples(std::size_t numSamples);
	void setDimSizeHints(SizeHints sizeHints);

	std::size_t getSampleIndex() const;
	std::size_t numDims() const;
	std::size_t numSamples() const;
	std::size_t numElements() const;
	const SizeHints& getDimSizeHints() const;

	std::string toString() const;

private:
	std::size_t m_sampleIndex;
	std::size_t m_numSamples;
	std::size_t m_numDims;
	SizeHints   m_dimSizeHints;
};

// In-header Implementations:

inline SampleStage::SampleStage(
	const std::size_t stageIndex,
	const std::size_t numDims,
	const std::size_t numSamples) :

	SampleStage(
		stageIndex, 
		numDims,
		numSamples, 
		makeIdentityDimSizeHints(numDims))
{}

inline SampleStage::SampleStage(
	const std::size_t sampleIndex,
	const std::size_t numDims,
	const std::size_t numSamples,
	SizeHints         dimSizeHints) :

	m_sampleIndex(sampleIndex),
	m_numDims    (numDims)
{
	PH_ASSERT_GT(numDims, 0);

	setNumSamples(numSamples);
	setDimSizeHints(std::move(dimSizeHints));
}

inline void SampleStage::setNumSamples(const std::size_t numSamples)
{
	PH_ASSERT_GE(numSamples, 1);

	m_numSamples = numSamples;
}

inline void SampleStage::setDimSizeHints(SizeHints sizeHints)
{
	PH_ASSERT_EQ(sizeHints.size(), m_numDims);

	m_dimSizeHints = std::move(sizeHints);
}

inline std::size_t SampleStage::getSampleIndex() const
{
	return m_sampleIndex;
}

inline std::size_t SampleStage::numDims() const
{
	return m_numDims;
}

inline std::size_t SampleStage::numSamples() const
{
	return m_numSamples;
}

inline std::size_t SampleStage::numElements() const
{
	return m_numDims * m_numSamples;
}

inline auto SampleStage::getDimSizeHints() const
	-> const SizeHints&
{
	return m_dimSizeHints;
}

inline auto SampleStage::makeIdentityDimSizeHints(const std::size_t numDims)
	-> SizeHints
{
	PH_ASSERT_GT(numDims, 0);

	// let each dimension equally sized
	return SizeHints(numDims, 1);
}

inline std::string SampleStage::toString() const
{
	std::string result("{Sample Stage: ");
	
	result += "index = " + std::to_string(m_sampleIndex) + ", ";
	result += "dim = " + std::to_string(m_numDims) + ", ";
	result += "samples = " + std::to_string(m_numSamples) + ", ";
	result += "hints = [";
	for(std::size_t di = 0; di < m_dimSizeHints.size(); ++di)
	{
		result += std::to_string(m_dimSizeHints[di]);
		result += di + 1 < m_dimSizeHints.size() ? ", " : "";
	}
	result += "]";

	return result + "}";
}

}// end namespace ph
