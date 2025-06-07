#include "Engine/Actor/Material/AbradedTranslucent.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

namespace ph
{

void AbradedTranslucent::storeCooked(
	CookedMaterial& out_material,
	const CookingContext& ctx) const
{
	out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<TranslucentMicrofacet>(
		m_interfaceInfo.genFresnelEffect(),
		m_microsurfaceInfo.genMicrofacet());

	// TODO: generate ideal dielectric if roughness == 0
}

}// end namespace ph
