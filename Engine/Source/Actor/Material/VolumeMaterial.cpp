#include "Actor/Material/Volume/VAbsorptionOnly.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "FileIO/SDL/InputPacket.h"
#include "Common/assertion.h"

namespace ph
{

VolumeMaterial::VolumeMaterial() : 
	Material(),
	m_sidedness(ESidedness::INTERIOR)
{}

VolumeMaterial::~VolumeMaterial() = default;

void VolumeMaterial::genBehaviors(
	CookingContext&    context, 
	PrimitiveMetadata& metadata) const
{
	switch(m_sidedness)
	{
	case ESidedness::INTERIOR:
		genVolume(context, metadata.getInterior());
		break;

	case ESidedness::EXTERIOR:
		genVolume(context, metadata.getExterior());
		break;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}
}

void VolumeMaterial::setSidedness(const ESidedness sidedness)
{
	m_sidedness = sidedness;
}

// command interface

VolumeMaterial::VolumeMaterial(const InputPacket& packet) : 
	Material(packet),
	m_sidedness(ESidedness::INTERIOR)
{
	const auto& sidedness = packet.getString("sidedness", "interior");
	if(sidedness == "interior")
	{
		m_sidedness = ESidedness::INTERIOR;
	}
	else if(sidedness == "exterior")
	{
		m_sidedness = ESidedness::EXTERIOR;
	}
}

}// end namespace ph