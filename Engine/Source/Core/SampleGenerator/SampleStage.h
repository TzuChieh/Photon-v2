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

	SampleStage() = default;

	SampleStage(
		std::size_t bufferIndex,
		std::size_t numDims,
		std::size_t numSamples);

	SampleStage(
		std::size_t bufferIndex,
		std::size_t numDimensions,
		std::size_t numSamples,
		SizeHints   dimSizeHints);

	void setNumSamples(std::size_t numSamples);
	void setDimSizeHints(SizeHints sizeHints);
	void setStrideLayout(std::size_t strideSize, std::size_t offsetInStride);
	SampleStage popFirstND(std::size_t N);

	std::size_t getBufferIndex() const;
	std::size_t numDims() const;
	std::size_t numSamples() const;
	std::size_t numElements() const;
	std::size_t getStrideSize() const;
	std::size_t getOffsetInStride() const;
	std::size_t getBufferSize() const;
	const SizeHints& getDimSizeHints() const;

	std::string toString() const;

private:
	std::size_t m_bufferIndex;
	std::size_t m_numSamples;
	std::size_t m_numDims;
	std::size_t m_strideSize;
	std::size_t m_offsetInStride;
	SizeHints   m_dimSizeHints;
};

// In-header Implementations:

inline SampleStage::SampleStage(
	const std::size_t bufferIndex,
	const std::size_t numDims,
	const std::size_t numSamples) :

	SampleStage(
		bufferIndex,
		numDims,
		numSamples, 
		makeIdentityDimSizeHints(numDims))
{}

inline SampleStage::SampleStage(
	const std::size_t bufferIndex,
	const std::size_t numDims,
	const std::size_t numSamples,
	SizeHints         dimSizeHints) :

	m_bufferIndex   (bufferIndex),
	m_numDims       (numDims),
	m_strideSize    (numSamples),
	m_offsetInStride(0)
{
	setNumSamples(numSamples);
	setDimSizeHints(std::move(dimSizeHints));
}

inline void SampleStage::setNumSamples(const std::size_t numSamples)
{
	m_numSamples = numSamples;
}

inline void SampleStage::setDimSizeHints(SizeHints sizeHints)
{
	PH_ASSERT_EQ(sizeHints.size(), m_numDims);

	m_dimSizeHints = std::move(sizeHints);
}

inline void SampleStage::setStrideLayout(const std::size_t strideSize, const std::size_t offsetInStride)
{
	PH_ASSERT_LT(offsetInStride, strideSize);
	PH_ASSERT_IN_RANGE_INCLUSIVE(m_numDims, 0, strideSize - offsetInStride);

	m_strideSize     = strideSize;
	m_offsetInStride = offsetInStride;
}

inline SampleStage SampleStage::popFirstND(const std::size_t N)
{
	PH_ASSERT_LE(N, m_numDims);

	const auto numSamples  = m_numSamples;
	const auto remainingN  = m_numDims - N;
	const auto strideSize  = m_strideSize;
	const auto sampleIndex = m_bufferIndex;

	const auto firstOffsetInStride  = m_offsetInStride;
	const auto secondOffsetInStride = firstOffsetInStride + N;

	const auto firstDimSizeHints = SizeHints(
		m_dimSizeHints.begin(),
		m_dimSizeHints.begin() + N);
	const auto secondDimSizeHints = SizeHints(
		m_dimSizeHints.begin() + N,
		m_dimSizeHints.end());

	*this = SampleStage(
		sampleIndex,
		remainingN, 
		numSamples,
		secondDimSizeHints);
	this->setStrideLayout(strideSize, secondOffsetInStride);

	auto poppedStage = SampleStage(
		sampleIndex,
		N,
		numSamples,
		firstDimSizeHints);
	poppedStage.setStrideLayout(strideSize, firstOffsetInStride);

	return std::move(poppedStage);
}

inline std::size_t SampleStage::getBufferIndex() const
{
	return m_bufferIndex;
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

inline std::size_t SampleStage::getStrideSize() const
{
	return m_strideSize;
}

inline std::size_t SampleStage::getOffsetInStride() const
{
	return m_offsetInStride;
}

inline std::size_t SampleStage::getBufferSize() const
{
	return m_strideSize * m_numSamples;
}

inline auto SampleStage::getDimSizeHints() const
	-> const SizeHints&
{
	return m_dimSizeHints;
}

inline auto SampleStage::makeIdentityDimSizeHints(const std::size_t numDims)
	-> SizeHints
{
	// let each dimension equally sized
	return SizeHints(numDims, 1);
}

inline std::string SampleStage::toString() const
{
	std::string result("{Sample Stage: ");
	
	result += "index = " + std::to_string(m_bufferIndex) + ", ";
	result += "dims = " + std::to_string(m_numDims) + ", ";
	result += "samples = " + std::to_string(m_numSamples) + ", ";
	result += "stride size = " + std::to_string(m_strideSize) + ", ";
	result += "offset in stride = " + std::to_string(m_offsetInStride) + ", ";
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
