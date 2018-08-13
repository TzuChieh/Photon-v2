#include "Core/SampleGenerator/SamplesBase.h"
#include "Math/Random.h"

namespace ph
{

void SamplesBase::perElementShuffleDurstenfeld(const std::size_t dim)
{
	for(std::size_t e = 0; e < m_numElements; e++)
	{
		const std::size_t j = Random::genUniformIndex_iL_eU(e, m_numElements);
		for(std::size_t d = 0; d < dim; d++)
		{
			std::swap(m_data[e * dim + d], 
			          m_data[j * dim + d]);
		}
	}
}

void SamplesBase::perDimensionShuffleDurstenfeld(const std::size_t dim)
{
	for(std::size_t d = 0; d < dim; d++)
	{
		for(std::size_t e = 0; e < m_numElements; e++)
		{
			const std::size_t j = Random::genUniformIndex_iL_eU(e, m_numElements);
			std::swap(m_data[e * dim + d], 
			          m_data[j * dim + d]);
			
		}
	}
}

}// end namespace ph