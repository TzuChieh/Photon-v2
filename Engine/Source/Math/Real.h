#pragma once

#include "Common/primitive_type.h"

#include <vector>

namespace ph::math
{

class Real
{
public:
	inline Real() :
		Real(0.0_r) {}

	inline Real(const real value) :
		m_value(value) {}

	inline Real(const std::vector<real>& values) :
		Real(values.empty() ? 0.0_r : values[0]) {}

	inline ~Real() = default;

	inline operator real () const
	{
		return m_value;
	}

private:
	real m_value;
};

}// end namespace ph::math
