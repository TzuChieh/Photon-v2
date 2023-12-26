#pragma once

#include "Math/Function/TMathFunction2D.h"
#include "Math/TVector2.h"

#include <cmath>
#include <algorithm>

namespace ph::math
{

/*! @brief A step function in 2-D, with a specific normal direction.
This is a simplified version of general Heaviside step function in 2-D. This class represents 1-D
Heaviside in 2-D, dividing the xy-plane into regions of value 0 & 1 (while a general Heaviside
step function in 2-D can have the discontinuity along an arbitarary 2-D contour, this class *does not*
support that). Value along the line of discontinuity is 0.5 (for integer types, this would be 0).
*/
template<typename Value>
class THeavisideStep2D : public TMathFunction2D<Value>
{
public:
	static auto makeHorizontal() -> THeavisideStep2D;
	static auto makeVertical() -> THeavisideStep2D;

	explicit THeavisideStep2D(TVector2<Value> normal);

	Value evaluate(Value x, Value y) const override;

private:
	TVector2<Value> m_unitNormal;
};

template<typename Value>
inline auto THeavisideStep2D<Value>::makeHorizontal()
-> THeavisideStep2D
{
	return THeavisideStep2D({1, 0});
}

template<typename Value>
inline auto THeavisideStep2D<Value>::makeVertical()
-> THeavisideStep2D
{
	return THeavisideStep2D({0, 1});
}

template<typename Value>
inline THeavisideStep2D<Value>::THeavisideStep2D(TVector2<Value> normal)
	: m_unitNormal(normal.normalize())
{}

template<typename Value>
inline Value THeavisideStep2D<Value>::evaluate(const Value x, const Value y) const
{
	const Value projectedSignedLengthOnNormal = m_unitNormal.dot({x, y});
	return projectedSignedLengthOnNormal > 0 
		? 1 
		: (projectedSignedLengthOnNormal < 0 ? 0 : static_cast<Value>(0.5));
}

}// end namespace ph::math
