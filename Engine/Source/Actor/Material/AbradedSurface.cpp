#include "Actor/Material/AbradedSurface.h"

namespace ph
{

AbradedSurface::~AbradedSurface() = default;

// command interface

SdlTypeInfo AbradedSurface::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-surface");
}

ExitStatus AbradedSurface::ciExecute(const std::shared_ptr<AbradedSurface>& targetResource,
                                     const std::string& functionName,
                                     const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph