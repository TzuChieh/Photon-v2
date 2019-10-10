#include "Actor/MotionSource/MotionSource.h"

namespace ph
{

// command interface

SdlTypeInfo MotionSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MOTION, "motion");
}

void MotionSource::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph
