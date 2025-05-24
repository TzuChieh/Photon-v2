#include "Engine/Actor/Material/FullMaterial.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Material/VolumeMaterial.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FullMaterial, Material);

void FullMaterial::storeCooked(
	CookedMaterial& out_material,
	const CookingContext& ctx) const
{
	if(!m_surfaceMaterial && !m_interiorMaterial && !m_exteriorMaterial)
	{
		PH_LOG(FullMaterial, Warning, "no material specified, skipping behavior generation");
		return;
	}

	if(m_surfaceMaterial)
	{
		const CookedMaterial* cooked = m_surfaceMaterial->createCooked(ctx);
		out_material.surfaceOptics = cooked && cooked->surfaceOptics ? cooked->surfaceOptics : nullptr;
	}

	const CookedMaterial* cookedInterior = m_interiorMaterial ? m_interiorMaterial->createCooked(ctx) : nullptr;
	if(cookedInterior)
	{
		const VolumeOptics* optics = nullptr;
		cookedInterior->findFirstCompatibleOptics(&optics, nullptr);
		if(optics)
		{
			out_material.volumeCompositions.push_back({
				.optics = optics,
				.type = EVolumeComposition::Interior});
		}
	}
	
	const CookedMaterial* cookedExterior = m_exteriorMaterial ? m_exteriorMaterial->createCooked(ctx) : nullptr;
	if(cookedExterior)
	{
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

}// end namespace ph
