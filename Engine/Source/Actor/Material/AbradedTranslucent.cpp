#include "Actor/Material/AbradedTranslucent.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"

#include <memory>

namespace ph
{

AbradedTranslucent::AbradedTranslucent() :
	SurfaceMaterial(),

	m_interfaceInfo   (),
	m_microsurfaceInfo()
{}

void AbradedTranslucent::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	behavior.setOptics(
		std::make_unique<TranslucentMicrofacet>(
			m_interfaceInfo.genFresnelEffect(),
			m_microsurfaceInfo.genMicrofacet()));
}

// command interface

AbradedTranslucent::AbradedTranslucent(const InputPacket& packet) : 
	SurfaceMaterial(packet),

	m_interfaceInfo   (packet),
	m_microsurfaceInfo(packet)
{}

SdlTypeInfo AbradedTranslucent::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-translucent");
}

void AbradedTranslucent::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AbradedTranslucent>([](const InputPacket& packet)
	{
		return std::make_unique<AbradedTranslucent>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph
