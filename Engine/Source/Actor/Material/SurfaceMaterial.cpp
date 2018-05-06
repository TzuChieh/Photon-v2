#include "Actor/Material/SurfaceMaterial.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"

namespace ph
{

SurfaceMaterial::SurfaceMaterial() = default;

SurfaceMaterial::~SurfaceMaterial() = default;

void SurfaceMaterial::genSurfaceBehavior(
	CookingContext&        context, 
	SurfaceBehavior* const out_surfaceBehavior) const
{
	PH_ASSERT(out_surfaceBehavior != nullptr);

	auto surfaceOptics = genSurfaceOptics(context);

	// TODO: log on null

	out_surfaceBehavior->setOptics(surfaceOptics);
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