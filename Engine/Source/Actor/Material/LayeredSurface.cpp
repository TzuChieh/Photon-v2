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
	std::vector<SpectralStrength> iorNs{SpectralStrength()};
	std::vector<SpectralStrength> iorKs{SpectralStrength()};
	std::vector<real>             alphas{1.0_r};

	behavior.setOptics(std::make_shared<LbLayeredSurface>(iorNs, iorKs, alphas));
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
	