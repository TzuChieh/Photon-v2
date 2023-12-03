#include "World/Foundation/CookedMotion.h"

namespace ph
{

MotionCookConfig::MotionCookConfig()
	: start()
	, end()
{
	// HACK
	Time t0;
	Time t1;
	t1.absoluteS = 1;
	t1.relativeS = 1;
	t1.relativeT = 1;

	start = t0;
	end = t1;
}

}// end namespace ph
