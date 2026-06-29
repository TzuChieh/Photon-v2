#include "Engine/Actor/Material/FullMaterial.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"

#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FullMaterial, Material);

void FullMaterial::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	if(!m_surfaceMaterial && !m_interiorMaterial && !m_exteriorMaterial)
	{
		PH_LOG(FullMaterial, Warning, "no material specified, skipping behavior generation");
		return;
	}

	if(m_surfaceMaterial)
	{
		const CookedMaterial* cooked = ctx.getCooked(*m_surfaceMaterial);
		out_material.surfaceOptics = cooked->surfaceOptics;
	}

	if(m_interiorMaterial)
	{
		const CookedMaterial* cookedInterior = ctx.getCooked(*m_interiorMaterial);
		const VolumeOptics* optics = nullptr;
		cookedInterior->findFirstCompatibleOptics(&optics, nullptr);
		if(optics)
		{
			out_material.volumeCompositions.push_back({
				.optics = optics,
				.type = EVolumeComposition::Interior});
		}
	}
	
	if(m_exteriorMaterial)
	{
		const CookedMaterial* cookedExterior = ctx.getCooked(*m_exteriorMaterial);
		const VolumeOptics* optics = nullptr;
		cookedExterior->findFirstCompatibleOptics(nullptr, &optics);
		if(optics)
		{
			out_material.volumeCompositions.push_back({
				.optics = optics,
				.type = EVolumeComposition::Exterior});
		}
	}
}

uint16 FullMaterial::getOverlapPriority() const
{
	uint16 priority = 0;

	if(m_interiorMaterial)
	{
		priority = std::max(m_interiorMaterial->getOverlapPriority(), priority);
	}

	if(m_exteriorMaterial)
	{
		priority = std::max(m_exteriorMaterial->getOverlapPriority(), priority);
	}

	return priority;
}

}// end namespace ph
