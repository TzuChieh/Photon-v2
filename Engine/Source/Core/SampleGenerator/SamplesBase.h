#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Common/assertion.h"

#include <cstddef>

namespace ph
{

class SamplesBase
{
public:
	inline SamplesBase() :
		m_data(nullptr), m_numSamples(0)
	{}

	inline SamplesBase(real* const data, const std::size_t numSamples) :
		m_data(data), m_numSamples(numSamples)
	{
		PH_ASSERT(data);
	}

	inline std::size_t numSamples() const
	{
		return m_numSamples;
	}

	bool isValid() const
	{
		return m_data != nullptr;
	}

protected:
	real*       m_data;
	std::size_t m_numSamples;

	inline ~SamplesBase() = default;

	void perSampleShuffleDurstenfeld(std::size_t dim);
	void perDimensionShuffleDurstenfeld(std::size_t dim);
};

}// end namespace ph