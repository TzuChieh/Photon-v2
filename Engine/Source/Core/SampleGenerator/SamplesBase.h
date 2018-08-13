#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"

#include <cstddef>

namespace ph
{

class SamplesBase
{
public:
	inline SamplesBase() :
		m_data(nullptr), m_numElements(0)
	{}

	inline SamplesBase(real* const data, std::size_t numElements) :
		m_data(data), m_numElements(numElements)
	{}

	inline std::size_t numElements() const
	{
		return m_numElements;
	}

	bool isValid() const
	{
		return m_data != nullptr;
	}

protected:
	real*       m_data;
	std::size_t m_numElements;

	inline ~SamplesBase() = default;

	void perElementShuffleDurstenfeld(std::size_t dim);
	void perDimensionShuffleDurstenfeld(std::size_t dim);
};

}// end namespace ph