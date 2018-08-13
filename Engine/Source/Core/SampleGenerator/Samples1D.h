#pragma once

#include "Core/SampleGenerator/SamplesBase.h"

namespace ph
{

class Samples1D : public SamplesBase
{
public:
	using SamplesBase::SamplesBase;

	void perElementShuffle();
	void perDimensionShuffle();

	inline void set(const std::size_t index, const real value)
	{
		m_data[index] = value;
	}

	inline real operator [] (const std::size_t index) const
	{
		return m_data[index];
	}

	inline std::size_t numReals() const
	{
		return m_numElements;
	}
};

// In-header Implementations:

inline void Samples1D::perElementShuffle()
{
	perElementShuffleDurstenfeld(1);
}

inline void Samples1D::perDimensionShuffle()
{
	perDimensionShuffleDurstenfeld(1);
}

}// end namespace ph