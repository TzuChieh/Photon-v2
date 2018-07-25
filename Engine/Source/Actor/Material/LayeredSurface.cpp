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
	/*std::vector<SpectralStrength> iorNs{SpectralStrength(1.4_r), SpectralStrength(2.92_r)};
	std::vector<SpectralStrength> iorKs{SpectralStrength(0), SpectralStrength(3.1_r)};
	std::vector<real>             alphas{0.01_r, 0.1_r};*/

	std::vector<SpectralStrength> iorNs{ SpectralStrength(1.4_r), SpectralStrength(1) };
	std::vector<SpectralStrength> iorKs{ SpectralStrength(0), SpectralStrength(0.8_r) };
	std::vector<real>             alphas{ 0.01_r, 0.1_r };

	/*std::vector<SpectralStrength> iorNs{SpectralStrength(1.4_r)};
	std::vector<SpectralStrength> iorKs{SpectralStrength(0)};
	std::vector<real>             alphas{0.01_r};*/

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
	