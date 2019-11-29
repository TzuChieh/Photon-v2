#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Math/Random.h"

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

	const real* readSample();
	SampleFlow readSampleAsFlow();

	std::size_t numDims() const;
	std::size_t numSamples() const;

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
		return &(m_buffer[m_readIndex++]);
	}
	else
	{
		for(auto& element : m_overreadBuffer)
		{
			element = math::Random::genUniformReal_i0_e1();
		}
		return m_overreadBuffer.data();
	}
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

inline const real* SamplesNDStream::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, numSamples());

	return &(m_buffer[index * m_numDims]);
}

}// end namespace ph
