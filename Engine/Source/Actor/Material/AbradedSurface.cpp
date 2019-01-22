#include "Actor/Material/AbradedSurface.h"

namespace ph
{

// command interface

SdlTypeInfo AbradedSurface::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-surface");
}

void AbradedSurface::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph