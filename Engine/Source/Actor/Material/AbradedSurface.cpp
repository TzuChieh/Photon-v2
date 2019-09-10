#include "Actor/Material/AbradedSurface.h"

namespace ph
{

// command interface

AbradedSurface::AbradedSurface(const InputPacket& packet) : 
	SurfaceMaterial(packet)
{

}

SdlTypeInfo AbradedSurface::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-surface");
}

void AbradedSurface::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
