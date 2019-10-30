#pragma once

#include "Common/assertion.h"
#include "Math/TArithmeticArray.h"
#include "Math/Random/shuffle.h"

#include <cstddef>

namespace ph
{

template<std::size_t N>
class TSamplesND final
{
public:
	using Sample = math::TArithmeticArray<real, N>;

	TSamplesND();
	TSamplesND(real* const buffer, const std::size_t numSamples);

	void shuffle();

	template<std::size_t DIM_INDEX>
	void shuffleDimension();

	template<std::size_t DIM_INDEX_BEGIN, std::size_t DIM_INDEX_END>
	void shuffleDimensions();

	std::size_t numSamples() const;
	bool isValid() const;
	Sample getSample(std::size_t index) const;

	Sample operator [] (const std::size_t index) const;

protected:
	real*       m_buffer;
	std::size_t m_numSamples;
};

// In-header Implementations:

template<std::size_t N>
inline TSamplesND<N>::TSamplesND() :
	m_buffer(nullptr), m_numSamples(0)
{}

template<std::size_t N>
inline TSamplesND<N>::TSamplesND(real* const buffer, const std::size_t numSamples) :
	m_buffer(buffer), m_numSamples(numSamples)
{
	PH_ASSERT(buffer);
}

template<std::size_t N>
inline void TSamplesND<N>::shuffle()
{
	math::shuffle_durstenfeld_index_pairs(
		0, m_numSamples, 
		[this](const std::size_t indexA, const std::size_t indexB)
		{
			for(std::size_t d = 0; d < N; ++d)
			{
				std::swap(m_buffer[indexA * N + d],
				          m_buffer[indexB * N + d]);
			}
		});
}

template<std::size_t N>
template<std::size_t DIM_INDEX>
inline void TSamplesND<N>::shuffleDimension()
{
	static_assert(DIM_INDEX < N, 
		"dimension index must be smaller than number of dimensions");

	math::shuffle_durstenfeld_index_pairs(
		0, m_numSamples, 
		[this](const std::size_t indexA, const std::size_t indexB)
		{
			std::swap(m_buffer[indexA * N + DIM_INDEX],
			          m_buffer[indexB * N + DIM_INDEX]);
		});
}

template<std::size_t N>
template<std::size_t DIM_INDEX_BEGIN, std::size_t DIM_INDEX_END>
inline void TSamplesND<N>::shuffleDimensions()
{
	static_assert(DIM_INDEX_BEGIN <= DIM_INDEX_END,
		"invalid dimension index range");

	if constexpr(DIM_INDEX_BEGIN == DIM_INDEX_END)
	{
		return;
	}
	else
	{
		shuffleDimension<DIM_INDEX_BEGIN>();
		shuffleDimensions<DIM_INDEX_BEGIN + 1, DIM_INDEX_END>();
	}
}

template<std::size_t N>
inline std::size_t TSamplesND<N>::numSamples() const
{
	return m_numSamples;
}

template<std::size_t N>
inline bool TSamplesND<N>::isValid() const
{
	return m_buffer != nullptr;
}

template<std::size_t N>
inline auto TSamplesND<N>::getSample(const std::size_t index) const
	-> Sample
{
	Sample sample;
	for(std::size_t i = 0; i < N; ++i)
	{
		sample[i] = m_buffer[index * N + i];
	}
	return sample;
}

template<std::size_t N>
inline auto TSamplesND<N>::operator [] (const std::size_t index) const
	-> Sample
{
	return getSample(index);
}

}// end namespace ph
