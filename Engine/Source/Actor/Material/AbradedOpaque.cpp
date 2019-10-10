#include "Actor/Material/AbradedOpaque.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"

#include <memory>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 

	SurfaceMaterial(),

	m_interfaceInfo(),
	m_microsurfaceInfo()
{}

void AbradedOpaque::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	behavior.setOptics(
		std::make_unique<OpaqueMicrofacet>(
			m_interfaceInfo.genFresnelEffect(),
			m_microsurfaceInfo.genMicrofacet()));
}

// command interface

AbradedOpaque::AbradedOpaque(const InputPacket& packet) : 
	SurfaceMaterial(packet),

	m_interfaceInfo   (packet),
	m_microsurfaceInfo(packet)
{}

SdlTypeInfo AbradedOpaque::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-opaque");
}

void AbradedOpaque::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AbradedOpaque>([](const InputPacket& packet)
	{
		return std::make_unique<AbradedOpaque>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph
