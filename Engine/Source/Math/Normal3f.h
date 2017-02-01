#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Normal3f final
{
public:
	real x;
	real y;
	real z;

	inline Normal3f(const real x, const real y, const real z) : x(x), y(y), z(z) {}
	inline Normal3f(const real value) : x(value), y(value), z(value) {}
};

}// end namespace ph