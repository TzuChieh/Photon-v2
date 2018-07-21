#include "Actor/Material/SurfaceMaterial.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Intersectable/PrimitiveMetadata.h"

namespace ph
{

SurfaceMaterial::SurfaceMaterial() = default;

void SurfaceMaterial::genBehaviors(
	CookingContext&    context, 
	PrimitiveMetadata& metadata) const
{
	genSurface(context, metadata.getSurface());
}

// command interface

SurfaceMaterial::SurfaceMaterial(const InputPacket& packet) : 
	Material(packet)
{}

SdlTypeInfo SurfaceMaterial::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "surface-material");
}

void SurfaceMaterial::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph