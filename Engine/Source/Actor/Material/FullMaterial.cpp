#include "Actor/Material/FullMaterial.h"
#include "FileIO/SDL/InputPacket.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/Material/SurfaceMaterial.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Material/VolumeMaterial.h"

#include <iostream>

namespace ph
{

FullMaterial::FullMaterial() : 
	FullMaterial(nullptr)
{}

FullMaterial::FullMaterial(const std::shared_ptr<SurfaceMaterial>& surfaceMaterial) : 
	Material(),
	m_surfaceMaterial(surfaceMaterial),
	m_interiorMaterial(nullptr),
	m_exteriorMaterial(nullptr)
{}

void FullMaterial::genBehaviors(
	CookingContext&    context, 
	PrimitiveMetadata& metadata) const
{
	if(m_surfaceMaterial)
	{
		m_surfaceMaterial->genBehaviors(context, metadata);
	}
	else
	{
		// TODO: logger
		std::cerr << "surface material is null" << std::endl;
		return;
	}

	if(m_interiorMaterial)
	{
		m_interiorMaterial->setSidedness(VolumeMaterial::ESidedness::INTERIOR);
		m_interiorMaterial->genBehaviors(context, metadata);
	}

	if(m_exteriorMaterial)
	{
		m_exteriorMaterial->setSidedness(VolumeMaterial::ESidedness::EXTERIOR);
		m_exteriorMaterial->genBehaviors(context, metadata);
	}
}

// command interface

FullMaterial::FullMaterial(const InputPacket& packet) : 
	Material(packet),
	m_surfaceMaterial(packet.get<SurfaceMaterial>("surface")),
	m_interiorMaterial(packet.get<VolumeMaterial>("interior")),
	m_exteriorMaterial(packet.get<VolumeMaterial>("exterior"))
{}

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
