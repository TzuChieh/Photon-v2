#include "Core/SampleGenerator/SamplesBase.h"
#include "Math/Random.h"

namespace ph
{

void SamplesBase::perSampleShuffleDurstenfeld(const std::size_t dim)
{
	for(std::size_t s = 0; s < m_numSamples; ++s)
	{
		const std::size_t j = Random::genUniformIndex_iL_eU(s, m_numSamples);
		for(std::size_t d = 0; d < dim; ++d)
		{
			std::swap(m_data[s * dim + d], 
			          m_data[j * dim + d]);
		}
	}
}

void SamplesBase::perDimensionShuffleDurstenfeld(const std::size_t dim)
{
	for(std::size_t d = 0; d < dim; ++d)
	{
		for(std::size_t s = 0; s < m_numSamples; ++s)
		{
			const std::size_t j = Random::genUniformIndex_iL_eU(s, m_numSamples);
			std::swap(m_data[s * dim + d], 
			          m_data[j * dim + d]);
			
		}
	}
}

}// end namespace ph