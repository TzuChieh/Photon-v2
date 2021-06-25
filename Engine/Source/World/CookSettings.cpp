#include "World/CookSettings.h"
#include "Common/Logger.h"

#include <iostream>

namespace ph
{

namespace
{
	Logger logger(LogSender("Cook Settings"));
}

CookSettings::CookSettings() : 
	CookSettings(EAccelerator::BVH)
{}

CookSettings::CookSettings(const EAccelerator topLevelAccelerator)
{
	setTopLevelAccelerator(topLevelAccelerator);
}

}// end namespace ph
