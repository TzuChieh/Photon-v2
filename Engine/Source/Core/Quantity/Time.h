#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Time final
{
public:
	real t;

	Time() : t(0.0_r) {}
};

}// end namespace ph