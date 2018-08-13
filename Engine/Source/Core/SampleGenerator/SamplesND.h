#pragma once

#include "Core/SampleGenerator/SamplesBase.h"

namespace ph
{

class SamplesND : public SamplesBase
{
public:
	using SamplesBase::SamplesBase;

	/*void perElementShuffle();
	void perDimensionShuffle();*/

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

	inline std::size_t numReals() const
	{
		return m_numElements * 2;
	}
};

// In-header Implementations:

//inline void Samples2D::perElementShuffle()
//{
//	perElementShuffleDurstenfeld(2);
//}
//
//inline void Samples2D::perDimensionShuffle()
//{
//	perDimensionShuffleDurstenfeld(2);
//}

}// end namespace ph