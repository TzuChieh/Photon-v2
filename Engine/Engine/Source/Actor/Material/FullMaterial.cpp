#include "Actor/Material/FullMaterial.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/Material/SurfaceMaterial.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Material/VolumeMaterial.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FullMaterial, Material);

FullMaterial::FullMaterial() : 
	FullMaterial(nullptr)
{}

FullMaterial::FullMaterial(const std::shared_ptr<SurfaceMaterial>& surfaceMaterial) : 

	Material(),

	m_surfaceMaterial (surfaceMaterial),
	m_interiorMaterial(nullptr),
	m_exteriorMaterial(nullptr)
{}

void FullMaterial::genBehaviors(
	const CookingContext& ctx,
	PrimitiveMetadata& metadata) const
{
	if(!m_surfaceMaterial && !m_interiorMaterial && !m_exteriorMaterial)
	{
		PH_LOG_WARNING(FullMaterial, "no material specified, skipping behavior generation");
		return;
	}

	if(m_surfaceMaterial)
	{
		m_surfaceMaterial->genBehaviors(ctx, metadata);
	}

	if(m_interiorMaterial)
	{
		m_interiorMaterial->setSidedness(VolumeMaterial::ESidedness::INTERIOR);
		m_interiorMaterial->genBehaviors(ctx, metadata);
	}

	if(m_exteriorMaterial)
	{
		m_exteriorMaterial->setSidedness(VolumeMaterial::ESidedness::EXTERIOR);
		m_exteriorMaterial->genBehaviors(ctx, metadata);
	}
}

}// end namespace ph
