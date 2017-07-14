#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Time final
{
public:
	real absoluteS;// s units of time since global beginning
	real relativeS;// s units of time since the begining of local time step
	real relativeT;// parametric time t in local time step, in [0, 1]

	Time() : 
		absoluteS(0.0_r),
		relativeS(0.0_r),
		relativeT(0.0_r)
	{

	}
};

}// end namespace ph