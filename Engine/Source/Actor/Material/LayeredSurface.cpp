#include "Actor/Material/LayeredSurface.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"

namespace ph
{

LayeredSurface::LayeredSurface() : 
	SurfaceMaterial()
{

}

void LayeredSurface::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	behavior.setOptics(std::make_shared<LbLayeredSurface>());
}

// command interface

LayeredSurface::LayeredSurface(const InputPacket& packet) : 
	SurfaceMaterial(packet)
{

}

SdlTypeInfo LayeredSurface::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "layered-surface");
}

void LayeredSurface::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<LayeredSurface>(packet);
	}));
}

}// end namespace ph
	