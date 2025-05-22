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

	if(m_interiorMaterial)
	{
		const CookedMaterial* cooked = m_interiorMaterial->createCooked(ctx);
		out_material.volumeCompositions.push_back({
			.optics = cooked ? cooked->getInteriorOptics() : nullptr,
			.type = EVolumeComposition::Interior});
	}

	if(m_exteriorMaterial)
	{
		const CookedMaterial* cooked = m_exteriorMaterial->createCooked(ctx);
		out_material.volumeCompositions.push_back({
			.optics = cooked ? cooked->getExteriorOptics() : nullptr,
			.type = EVolumeComposition::Exterior});
	}
}

}// end namespace ph
