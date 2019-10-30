#pragma once

#include "Core/SampleGenerator/SamplesBase.h"

namespace ph
{

class Samples1D final : public SamplesBase
{
public:
	using SamplesBase::SamplesBase;

	void perSampleShuffle();
	void perDimensionShuffle();

	void set(const std::size_t index, const real value);

	real operator [] (const std::size_t index) const;
};

// In-header Implementations:

inline void Samples1D::perSampleShuffle()
{
	perSampleShuffleDurstenfeld(1);
}

inline void Samples1D::perDimensionShuffle()
{
	perDimensionShuffleDurstenfeld(1);
}

inline void Samples1D::set(const std::size_t index, const real value)
{
	m_data[index] = value;
}

inline real Samples1D::operator [] (const std::size_t index) const
{
	return m_data[index];
}

}// end namespace ph
