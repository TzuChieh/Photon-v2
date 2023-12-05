#pragma once

#include "Math/Function/TMathFunction2D.h"
#include "Math/constant.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph::math
{

/*! @brief Blackman-Harris window function.
A window function similar to Gaussian function in shape. It is defined 
over @f$ \left[-radius, radius\right] @f$ and will take on value 0 at boundaries. 
The peak of the function is at @f$ (0, 0) @f$.

Reference: https://en.wikipedia.org/wiki/Window_function
*/
template<typename Value>
class TBlackmanHarris2D : public TMathFunction2D<Value>
{
public:
	explicit TBlackmanHarris2D(Value radius);

	Value evaluate(Value x, Value y) const override;

private:
	Value m_reciRadius;

	Value blackmanHarris1D(Value n) const;
};

template<typename Value>
inline TBlackmanHarris2D<Value>::TBlackmanHarris2D(const Value radius) :
	m_reciRadius(radius > Value(0) ? Value(1) / radius : Value(0))
{
	PH_ASSERT(radius > Value(0));
}

template<typename Value>
inline Value TBlackmanHarris2D<Value>::evaluate(const Value x, const Value y) const
{
	const Value nx = (x * m_reciRadius + Value(1.0)) * Value(0.5);
	const Value ny = (y * m_reciRadius + Value(1.0)) * Value(0.5);

	return blackmanHarris1D(nx) * blackmanHarris1D(ny);
}

template<typename Value>
inline Value TBlackmanHarris2D<Value>::blackmanHarris1D(const Value n) const
{
	return 
		  Value(0.35875)
		- Value(0.48829) * std::cos(Value(2.0) * constant::pi<Value> * n)
		+ Value(0.14128) * std::cos(Value(4.0) * constant::pi<Value> * n)
		- Value(0.01174) * std::cos(Value(6.0) * constant::pi<Value> * n);
	//          ^^^^^^^
	// NOTE: Originally this coefficient is 0.01168, we modified it like how
	//       appleseed renderer did to make it evaluate to 0 at boundaries.
}

}// end namespace ph::math
