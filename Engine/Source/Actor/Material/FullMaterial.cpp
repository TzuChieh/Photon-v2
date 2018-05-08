#include "Actor/Material/FullMaterial.h"
#include "FileIO/SDL/InputPacket.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/Material/SurfaceMaterial.h"

#include <iostream>

namespace ph
{

FullMaterial::FullMaterial() : 
	FullMaterial(nullptr)
{}

FullMaterial::FullMaterial(const std::shared_ptr<SurfaceMaterial>& surfaceMaterial) : 
	Material(),
	m_surfaceMaterial(surfaceMaterial)
{}

FullMaterial::~FullMaterial() = default;

void FullMaterial::genSurfaceBehavior(
	CookingContext&        context, 
	SurfaceBehavior* const out_surfaceBehavior) const
{
	PH_ASSERT(out_surfaceBehavior != nullptr);

	if(m_surfaceMaterial == nullptr)
	{
		std::cerr << "surface material is null" << std::endl;
		return;
	}

	auto surfaceOptics = m_surfaceMaterial->genSurfaceOptics(context);
	out_surfaceBehavior->setOptics(surfaceOptics);
}

// command interface

FullMaterial::FullMaterial(const InputPacket& packet) : 
	Material(packet),
	m_surfaceMaterial(nullptr)
{
	auto surfaceMaterial = packet.get<SurfaceMaterial>("surface");
	m_surfaceMaterial = surfaceMaterial;
}

SdlTypeInfo FullMaterial::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "full");
}

void FullMaterial::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<FullMaterial>([](const InputPacket& packet)
	{
		return std::make_unique<FullMaterial>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph