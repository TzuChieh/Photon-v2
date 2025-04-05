#pragma once

#include <Common/assertion.h>

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

	template<typename T>
	using TRange = std::pair<T, T>;

	static SizeHints makeIdentityDimSizeHints(std::size_t numDims);

	SampleStage() = default;

	SampleStage(
		std::size_t         bufferIndex,
		std::size_t         numSamples,
		TRange<std::size_t> dimIndexRange);

	SampleStage(
		std::size_t         bufferIndex,
		std::size_t         numSamples,
		TRange<std::size_t> dimIndexRange,
		SizeHints           dimSizeHints);

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
	TRange<std::size_t> getDimIndexRange() const;
	const SizeHints& getDimSizeHints() const;

	std::string toString() const;

private:
	std::size_t         m_bufferIndex;
	std::size_t         m_numSamples;
	TRange<std::size_t> m_dimIndexRange;
	std::size_t         m_strideSize;
	std::size_t         m_offsetInStride;
	SizeHints           m_dimSizeHints;

	static std::size_t numDimensions(const TRange<std::size_t>& dimIndexRange);
};

// In-header Implementations:

inline SampleStage::SampleStage(
	const std::size_t   bufferIndex,
	const std::size_t   numSamples,
	TRange<std::size_t> dimIndexRange) :

	SampleStage(
		bufferIndex,
		numSamples, 
		std::move(dimIndexRange),
		makeIdentityDimSizeHints(numDimensions(dimIndexRange)))
{}

inline SampleStage::SampleStage(
	const std::size_t   bufferIndex,
	const std::size_t   numSamples,
	TRange<std::size_t> dimIndexRange,
	SizeHints           dimSizeHints) :

	m_bufferIndex   (bufferIndex),
	m_numSamples    (numSamples),
	m_dimIndexRange (std::move(dimIndexRange)),
	m_strideSize    (numDimensions(dimIndexRange)),
	m_offsetInStride(0),
	m_dimSizeHints  (std::move(dimSizeHints))
{
	PH_ASSERT_LE(m_dimIndexRange.first, m_dimIndexRange.second);
	PH_ASSERT_EQ(m_dimSizeHints.size(), numDimensions(m_dimIndexRange));
}

inline void SampleStage::setNumSamples(const std::size_t numSamples)
{
	m_numSamples = numSamples;
}

inline void SampleStage::setDimSizeHints(SizeHints sizeHints)
{
	PH_ASSERT_EQ(sizeHints.size(), numDims());

	m_dimSizeHints = std::move(sizeHints);
}

inline void SampleStage::setStrideLayout(const std::size_t strideSize, const std::size_t offsetInStride)
{
	PH_ASSERT_LE(offsetInStride, strideSize);
	PH_ASSERT_IN_RANGE_INCLUSIVE(numDims(), 0, strideSize - offsetInStride);

	m_strideSize     = strideSize;
	m_offsetInStride = offsetInStride;
}

inline SampleStage SampleStage::popFirstND(const std::size_t N)
{
	PH_ASSERT_LE(N, numDims());

	const auto numSamples  = m_numSamples;
	const auto strideSize  = m_strideSize;
	const auto bufferIndex = m_bufferIndex;

	const auto firstOffsetInStride  = m_offsetInStride;
	const auto secondOffsetInStride = firstOffsetInStride + N;

	const auto firstDimIndexRange  = TRange<std::size_t>{m_dimIndexRange.first, m_dimIndexRange.first + N};
	const auto secondDimIndexRange = TRange<std::size_t>{firstDimIndexRange.second, m_dimIndexRange.second};

	const auto firstDimSizeHints = SizeHints(
		m_dimSizeHints.begin(),
		m_dimSizeHints.begin() + N);
	const auto secondDimSizeHints = SizeHints(
		m_dimSizeHints.begin() + N,
		m_dimSizeHints.end());

	auto poppedStage = SampleStage(
		bufferIndex,
		numSamples,
		firstDimIndexRange,
		firstDimSizeHints);
	poppedStage.setStrideLayout(strideSize, firstOffsetInStride);

	*this = SampleStage(
		bufferIndex,
		numSamples,
		secondDimIndexRange,
		secondDimSizeHints);
	this->setStrideLayout(strideSize, secondOffsetInStride);

	return poppedStage;
}

inline std::size_t SampleStage::getBufferIndex() const
{
	return m_bufferIndex;
}

inline std::size_t SampleStage::numDims() const
{
	return numDimensions(m_dimIndexRange);
}

inline std::size_t SampleStage::numSamples() const
{
	return m_numSamples;
}

inline std::size_t SampleStage::numElements() const
{
	return numDims() * m_numSamples;
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

inline auto SampleStage::getDimIndexRange() const
	-> TRange<std::size_t>
{
	return m_dimIndexRange;
}

inline auto SampleStage::getDimSizeHints() const
	-> const SizeHints&
{
	return m_dimSizeHints;
}

inline auto SampleStage::makeIdentityDimSizeHints(const std::size_t numDims)
	-> SizeHints
{
	// Let each dimension equally sized
	return SizeHints(numDims, 1);
}

inline std::string SampleStage::toString() const
{
	std::string result("{Sample Stage: ");
	
	result += "index = " + std::to_string(m_bufferIndex) + ", ";
	result += "samples = " + std::to_string(m_numSamples) + ", ";
	result += "[dim-begin, dim-end) = [" + std::to_string(m_dimIndexRange.first) + ", " + 
	                                       std::to_string(m_dimIndexRange.second) + "), ";
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

inline std::size_t SampleStage::numDimensions(const TRange<std::size_t>& dimIndexRange)
{
	PH_ASSERT_LE(dimIndexRange.first, dimIndexRange.second);

	return dimIndexRange.second - dimIndexRange.first;
}

}// end namespace ph
