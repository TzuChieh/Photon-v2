#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Normal3f final
{
public:
	float32 x;
	float32 y;
	float32 z;

	inline Normal3f(const float32 x, const float32 y, const float32 z) : x(x), y(y), z(z) {}
	inline Normal3f(const float32 value) : x(value), y(value), z(value) {}
};

}// end namespace ph