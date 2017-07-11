#include "Actor/MotionSource/MotionSource.h"

namespace ph
{

MotionSource::~MotionSource() = default;

// command interface

MotionSource::MotionSource(const InputPacket& packet) : 
	MotionSource()
{

}

SdlTypeInfo MotionSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MOTION, "motion");
}

ExitStatus MotionSource::ciExecute(const std::shared_ptr<MotionSource>& targetResource,
                                   const std::string& functionName,
                                   const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph