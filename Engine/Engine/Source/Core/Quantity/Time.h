#pragma once

#include <Common/primitive_type.h>
#include <Common/compiler.h>

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

	Time(real absoluteS, real relativeS, real relativeT) :
		absoluteS(absoluteS),
		relativeS(relativeS),
		relativeT(relativeT)
	{

	}

	bool operator == (const Time& other) const
	{
		return absoluteS == other.absoluteS && 
		       relativeS == other.relativeS &&
		       relativeT == other.relativeT;
	}

#if !PH_COMPILER_HAS_P2468R2
	bool operator != (const Time& other) const
	{
		return !(*this == other);
	}
#endif
};

}// end namespace ph
