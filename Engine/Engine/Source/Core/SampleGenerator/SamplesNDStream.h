#pragma once

#include "Core/SampleGenerator/SampleFlow.h"
#include "Math/Random/Random.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <array>
#include <vector>

namespace ph
{

class SamplesNDStream final
{
public:
	SamplesNDStream(
		const real* buffer, 
		std::size_t numDims, 
		std::size_t numSamples);

	/*! @brief Read the next N-dimensional sample.
	@return Pointer to a N-dimensional sample.
	*/
	const real* readSample();

	/*! @brief Read the next N-dimensional sample.
	@tparam N Number of dimensions. Must be equal to the sample dimension of this stream.
	*/
	template<std::size_t N>
	std::array<real, N> readSample();

	/*! @brief Read the next N-dimensional sample as flow, which has more flexible controls for sampling.
	@return The next N-dimensional sample in the form of flow.
	*/
	SampleFlow readSampleAsFlow();

	std::size_t numDims() const;
	std::size_t numSamples() const;

	template<std::size_t N>
	std::array<real, N> get(std::size_t index) const;

	/*! @brief Get the i-th N-dimensional sample.
	@return Pointer to the sample values (with N elements).
	*/
	const real* operator [] (std::size_t index) const;

private:
	const real*       m_buffer;
	std::size_t       m_numDims;
	std::size_t       m_numSamples;
	std::size_t       m_readIndex;
	std::vector<real> m_overreadBuffer;
};

// In-header Implementations:

inline SamplesNDStream::SamplesNDStream(
	const real* const buffer,
	const std::size_t numDims,
	const std::size_t numSamples) : 

	m_buffer        (buffer),
	m_numDims       (numDims),
	m_numSamples    (numSamples),
	m_readIndex     (0),
	m_overreadBuffer(numDims)// TODO: possibly make it record all samples for further usage (let SamplesND provide buffer?)
{
	PH_ASSERT(buffer);
	PH_ASSERT_GT(numDims, 0);
	PH_ASSERT_GT(numSamples, 0);
}

inline const real* SamplesNDStream::readSample()
{
	PH_ASSERT_EQ(m_overreadBuffer.size(), m_numDims);

	if(m_readIndex < m_numSamples)
	{
		return (*this)[m_readIndex++];
	}
	else
	{
		for(auto& element : m_overreadBuffer)
		{
			element = math::Random::sample();
		}
		return m_overreadBuffer.data();
	}
}

template<std::size_t N>
inline std::array<real, N> SamplesNDStream::readSample()
{
	PH_ASSERT_EQ(N, m_numDims);

	const real* const samplePtr = readSample();

	std::array<real, N> sample;
	for(std::size_t di = 0; di < N; ++di)
	{
		sample[di] = samplePtr[di];
	}
	return sample;
}

inline SampleFlow SamplesNDStream::readSampleAsFlow()
{
	return SampleFlow(readSample(), m_numDims);
}

inline std::size_t SamplesNDStream::numDims() const
{
	return m_numDims;
}

inline std::size_t SamplesNDStream::numSamples() const
{
	return m_numSamples;
}

template<std::size_t N>
inline std::array<real, N> SamplesNDStream::get(const std::size_t index) const
{
	PH_ASSERT_EQ(N, m_numDims);

	std::array<real, N> sample;
	for(std::size_t di = 0; di < N; ++di)
	{
		sample[di] = (*this)[index][di];
	}
	return sample;
}

inline const real* SamplesNDStream::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, numSamples());

	return &(m_buffer[index * m_numDims]);
}

}// end namespace ph
