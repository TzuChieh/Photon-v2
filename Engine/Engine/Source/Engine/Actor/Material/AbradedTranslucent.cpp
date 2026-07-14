#include "Engine/Actor/Material/AbradedTranslucent.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <utility>

namespace ph
{

void AbradedTranslucent::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<TranslucentMicrofacet>(
		m_interfaceInfo.genFresnelEffect(ctx),
		m_microsurfaceInfo.genMicrofacet(ctx));

	// TODO: generate ideal dielectric if roughness == 0
}

void AbradedTranslucent::setRoughnessMap(std::shared_ptr<Image> roughnessMap)
{
	m_microsurfaceInfo.setRoughnessMap(std::move(roughnessMap));
}

void AbradedTranslucent::setRoughnessVMap(std::shared_ptr<Image> roughnessVMap)
{
	m_microsurfaceInfo.setRoughnessVMap(std::move(roughnessVMap));
}

}// end namespace ph
