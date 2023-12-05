#include "Actor/Material/Volume/VAbsorptionOnly.h"
#include "Core/Intersectable/PrimitiveMetadata.h"

#include <Common/assertion.h>

namespace ph
{

VolumeMaterial::VolumeMaterial() : 
	Material(),
	m_sidedness(ESidedness::INTERIOR)
{}

void VolumeMaterial::genBehaviors(
	const CookingContext& ctx,
	PrimitiveMetadata& metadata) const
{
	switch(m_sidedness)
	{
	case ESidedness::INTERIOR:
		genVolume(ctx, metadata.getInterior());
		break;

	case ESidedness::EXTERIOR:
		genVolume(ctx, metadata.getExterior());
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

}// end namespace ph
