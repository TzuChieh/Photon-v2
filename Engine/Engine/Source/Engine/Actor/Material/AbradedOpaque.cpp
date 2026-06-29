#include "Engine/Actor/Material/AbradedOpaque.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

namespace ph
{

void AbradedOpaque::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<OpaqueMicrofacet>(
		m_interfaceInfo.genFresnelEffect(),
		m_microsurfaceInfo.genMicrofacet());

	// TODO: generate ideal reflector if roughness == 0
}

}// end namespace ph
