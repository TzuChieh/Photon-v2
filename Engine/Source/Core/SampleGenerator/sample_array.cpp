#include "Core/SampleGenerator/sample_array.h"
#include "Math/Random.h"

namespace ph
{

void SampleArray1D::perElementShuffle()
{
	perElementShuffleDurstenfeld(1);
}

void SampleArray1D::perDimensionShuffle()
{
	perDimensionShuffleDurstenfeld(1);
}

void SampleArray2D::perElementShuffle()
{
	perElementShuffleDurstenfeld(2);
}

void SampleArray2D::perDimensionShuffle()
{
	perDimensionShuffleDurstenfeld(2);
}

void SampleArray::perElementShuffleDurstenfeld(const std::size_t dim)
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

void SampleArray::perDimensionShuffleDurstenfeld(const std::size_t dim)
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