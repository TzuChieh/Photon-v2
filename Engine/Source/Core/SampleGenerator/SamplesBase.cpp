#include "Core/SampleGenerator/SamplesBase.h"
#include "Math/Random.h"
#include "Math/Random/shuffle.h"

namespace ph
{

void SamplesBase::perSampleShuffleDurstenfeld(const std::size_t dim)
{
	math::shuffle_durstenfeld_index_pairs(
		0, m_numSamples, 
		[this, dim](const std::size_t indexA, const std::size_t indexB)
		{
			for(std::size_t d = 0; d < dim; ++d)
			{
				std::swap(m_data[indexA * dim + d],
				          m_data[indexB * dim + d]);
			}
		});
}

void SamplesBase::perDimensionShuffleDurstenfeld(const std::size_t dim)
{
	for(std::size_t d = 0; d < dim; ++d)
	{
		math::shuffle_durstenfeld_index_pairs(
			0, m_numSamples, 
			[this, dim, d](const std::size_t indexA, const std::size_t indexB)
			{
				std::swap(m_data[indexA * dim + d],
				          m_data[indexB * dim + d]);
			});
	}
}

}// end namespace ph
