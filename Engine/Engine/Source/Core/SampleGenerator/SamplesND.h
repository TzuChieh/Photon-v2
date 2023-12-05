#pragma once

#include "Math/TArithmeticArray.h"
#include "Math/Random/shuffle.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <cstddef>
#include <array>

namespace ph
{

class SamplesND final
{
public:
	SamplesND();

	SamplesND(
		real*       buffer, 
		std::size_t numDims, 
		std::size_t numSamples);

	SamplesND(
		real*       buffer, 
		std::size_t numDims, 
		std::size_t numSamples, 
		std::size_t strideSize, 
		std::size_t offsetInStride);

	void shuffle();
	void shuffleDimension(std::size_t dimIndex);
	void shuffleDimensions(std::size_t dimIndexBegin, std::size_t dimIndexEnd);
	void shuffleAllDimensions();

	void setSample(std::size_t index, const real* sample);

	template<std::size_t N>
	void setSample(std::size_t index, const std::array<real, N>& sample);

	real* buffer();

	std::size_t numDims() const;
	std::size_t numSamples() const;
	std::size_t numElements() const;
	bool isValid() const;

	real* operator [] (std::size_t index);

private:
	real*       m_buffer;
	std::size_t m_numDims;
	std::size_t m_numSamples;
	std::size_t m_strideSize;
	std::size_t m_offsetInStride;
};

// In-header Implementations:

inline SamplesND::SamplesND() :
	m_buffer        (nullptr), 
	m_numDims       (0), 
	m_numSamples    (0),
	m_strideSize    (0),
	m_offsetInStride(0)
{}

inline SamplesND::SamplesND(
	real* const       buffer, 
	const std::size_t numDims, 
	const std::size_t numSamples) :

	SamplesND(
		buffer,
		numDims,
		numSamples,
		numDims,
		0)
{}

inline SamplesND::SamplesND(
	real* const       buffer,
	const std::size_t numDims,
	const std::size_t numSamples,
	const std::size_t strideSize,
	const std::size_t offsetInStride) :

	m_buffer        (buffer),
	m_numDims       (numDims),
	m_numSamples    (numSamples),
	m_strideSize    (strideSize),
	m_offsetInStride(offsetInStride)
{
	PH_ASSERT(buffer);
	PH_ASSERT_LT(offsetInStride, strideSize);
	PH_ASSERT_IN_RANGE_INCLUSIVE(numDims, 1, strideSize - offsetInStride);
	PH_ASSERT_GT(numSamples, 0);
}

inline void SamplesND::shuffle()
{
	math::shuffle_durstenfeld_index_pairs(
		0, m_numSamples, 
		[this](const std::size_t indexA, const std::size_t indexB)
		{
			for(std::size_t di = 0; di < m_numDims; ++di)
			{
				std::swap((*this)[indexA][di],
				          (*this)[indexB][di]);
			}
		});
}

inline void SamplesND::shuffleDimension(const std::size_t dimIndex)
{
	PH_ASSERT_MSG(dimIndex < m_numDims,
		"dimension index must be smaller than number of dimensions");

	math::shuffle_durstenfeld_index_pairs(
		0, m_numSamples, 
		[this, dimIndex](const std::size_t indexA, const std::size_t indexB)
		{
			std::swap((*this)[indexA][dimIndex], 
			          (*this)[indexB][dimIndex]);
		});
}

inline void SamplesND::shuffleDimensions(const std::size_t dimIndexBegin, const std::size_t dimIndexEnd)
{
	PH_ASSERT_MSG(dimIndexBegin < dimIndexEnd,
		"invalid dimension index range");

	for(std::size_t dimIndex = dimIndexBegin; dimIndex < dimIndexEnd; ++dimIndex)
	{
		shuffleDimension(dimIndex);
	}
}

inline void SamplesND::shuffleAllDimensions()
{
	shuffleDimensions(0, m_numDims);
}

inline void SamplesND::setSample(const std::size_t index, const real* const sample)
{
	PH_ASSERT(sample);

	for(std::size_t di = 0; di < m_numDims; ++di)
	{
		(*this)[index][di] = sample[di];
	}
}

template<std::size_t N>
inline void SamplesND::setSample(const std::size_t index, const std::array<real, N>& sample)
{
	PH_ASSERT_EQ(N, m_numDims);

	setSample(index, sample.data());
}

inline real* SamplesND::buffer()
{
	return m_buffer;
}

inline std::size_t SamplesND::numDims() const
{
	return m_numDims;
}

inline std::size_t SamplesND::numSamples() const
{
	return m_numSamples;
}

inline std::size_t SamplesND::numElements() const
{
	return m_numSamples * m_numDims;
}

inline bool SamplesND::isValid() const
{
	return m_buffer != nullptr;
}

inline real* SamplesND::operator [] (const std::size_t index)
{
	PH_ASSERT_LT(index, numSamples());

	return &(m_buffer[index * m_strideSize + m_offsetInStride]);
}

}// end namespace ph
