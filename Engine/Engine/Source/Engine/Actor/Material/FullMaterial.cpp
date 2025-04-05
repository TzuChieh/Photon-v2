#include "Engine/Actor/Material/FullMaterial.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Actor/Material/VolumeMaterial.h"

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
		PH_LOG(FullMaterial, Warning, "no material specified, skipping behavior generation");
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
