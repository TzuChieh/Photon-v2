#pragma once

#include "Math/Function/TMathFunction2D.h"

#include <cmath>
#include <algorithm>

namespace ph::math
{

/*! @brief Mitchell–Netravali filter function.
*/
template<typename ValueType>
class TMNCubic2D : public TMathFunction2D<ValueType>
{
public:
	TMNCubic2D(ValueType b, ValueType c);

	ValueType evaluate(ValueType x, ValueType y) const override;

private:
	ValueType m_b;
	ValueType m_c;

	ValueType mnCubic1D(ValueType x) const;
};

template<typename ValueType>
inline TMNCubic2D<ValueType>::TMNCubic2D(const ValueType b, const ValueType c) :
	TMathFunction2D<ValueType>(),
	m_b(b), m_c(c)
{}

template<typename ValueType>
inline ValueType TMNCubic2D<ValueType>::evaluate(const ValueType x, const ValueType y) const
{
	return mnCubic1D(x) * mnCubic1D(y);
}

template<typename ValueType>
inline ValueType TMNCubic2D<ValueType>::mnCubic1D(const ValueType x) const
{
	const ValueType absX = std::abs(x);
	if(absX < 1)
	{
		return
		(
			(12 - 9 * m_b - 6 * m_c) * absX * absX * absX +
			(-18 + 12 * m_b + 6 * m_c) * absX * absX +
			(6 - 2 * m_b)
		) / 6;
	}
	else if(absX < 2)
	{
		return
		(
			(-m_b - 6 * m_c) * absX * absX * absX +
			(6 * m_b + 30 * m_c) * absX * absX +
			(-12 * m_b - 48 * m_c) * absX +
			(8 * m_b + 24 * m_c)
		) / 6;
	}
	else
	{
		return 0;
	}
}

}// end namespace ph::math
