#pragma once

#include "Math/Function/TMathFunction2D.h"

#include <cmath>
#include <algorithm>

namespace ph::math
{

/*! @brief Mitchell–Netravali filter function.
*/
template<typename Value>
class TMNCubic2D : public TMathFunction2D<Value>
{
public:
	TMNCubic2D(Value b, Value c);

	Value evaluate(Value x, Value y) const override;

private:
	Value m_b;
	Value m_c;

	Value mnCubic1D(Value x) const;
};

template<typename Value>
inline TMNCubic2D<Value>::TMNCubic2D(const Value b, const Value c)
	: m_b(b)
	, m_c(c)
{}

template<typename Value>
inline Value TMNCubic2D<Value>::evaluate(const Value x, const Value y) const
{
	return mnCubic1D(x) * mnCubic1D(y);
}

template<typename Value>
inline Value TMNCubic2D<Value>::mnCubic1D(const Value x) const
{
	const Value absX = std::abs(x);
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
