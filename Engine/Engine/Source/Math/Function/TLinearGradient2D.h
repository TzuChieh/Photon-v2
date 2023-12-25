#pragma once

#include "Math/Function/TMathFunction2D.h"
#include "Math/TVector2.h"

#include <cmath>
#include <algorithm>

namespace ph::math
{

/*! @brief A linearly increasing/decreasing gradient in a specific direction.
Value at (0, 0) is 0.
*/
template<typename Value>
class TLinearGradient2D : public TMathFunction2D<Value>
{
public:
	static auto makeHorizontal(Value slope) -> TLinearGradient2D;
	static auto makeVertical(Value slope) -> TLinearGradient2D;

	TLinearGradient2D(TVector2<Value> direction, Value slope);

	Value evaluate(Value x, Value y) const override;

private:
	// Think in 3D, this is basically the direction of the ramp projected on the xy-plane
	TVector2<Value> m_unitDir;
	
	// Amount of value change in one unit amount of movement in the direction of `m_unitDir` 
	Value m_slope;
};

template<typename Value>
inline auto TLinearGradient2D<Value>::makeHorizontal(const Value slope)
-> TLinearGradient2D
{
	return TLinearGradient2D({1, 0}, slope);
}

template<typename Value>
inline auto TLinearGradient2D<Value>::makeVertical(const Value slope)
-> TLinearGradient2D
{
	return TLinearGradient2D({0, 1}, slope);
}

template<typename Value>
inline TLinearGradient2D<Value>::TLinearGradient2D(TVector2<Value> direction, Value slope)
	: m_unitDir(direction.normalize())
	, m_slope(slope)
{}

template<typename Value>
inline Value TLinearGradient2D<Value>::evaluate(const Value x, const Value y) const
{
	const Value projectedSignedLengthOnDir = m_unitDir.dot({x, y});
	return projectedSignedLengthOnDir * m_slope;
}

}// end namespace ph::math
