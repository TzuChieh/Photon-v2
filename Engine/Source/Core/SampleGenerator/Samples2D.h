#pragma once

#include "Core/SampleGenerator/SamplesBase.h"

namespace ph
{

class Samples2D : public SamplesBase
{
public:
	using SamplesBase::SamplesBase;

	void perSampleShuffle();
	void perDimensionShuffle();

	inline void set(const std::size_t index, const real valueX, const real valueY)
	{
		m_data[index * 2] = valueX;
		m_data[index * 2 + 1] = valueY;
	}

	inline void set(const std::size_t index, const Vector2R& value)
	{
		set(index, value.x, value.y);
	}

	inline Vector2R operator [] (const std::size_t index) const
	{
		return Vector2R(m_data[index * 2], m_data[index * 2 + 1]);
	}
};

// In-header Implementations:

inline void Samples2D::perSampleShuffle()
{
	perSampleShuffleDurstenfeld(2);
}

inline void Samples2D::perDimensionShuffle()
{
	perDimensionShuffleDurstenfeld(2);
}

}// end namespace ph