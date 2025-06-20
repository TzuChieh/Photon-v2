#include "Engine/Actor/Material/SurfaceNormalMap.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/MicrofacetNormalMapper.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/Actor/Basic/exceptions.h"

namespace ph
{

void SurfaceNormalMap::storeCooked(
	CookedMaterial& out_material,
	const CookingContext& ctx) const
{
	if(!m_material)
	{
		throw CookException("No target material specified for surface normal map.");
	}

	m_material->storeCooked(out_material, ctx);
	
	if(m_map)
	{
		std::shared_ptr<TTexture<math::Vector3R>> mapTexture = m_map->genVector3RTexture(ctx);

		auto const normalMappedSurface = ctx.getResources().makeSurfaceOptics<MicrofacetNormalMapper>(
			out_material.surfaceOptics, mapTexture);
		out_material.surfaceOptics = normalMappedSurface;
	}
	else
	{
		PH_DEFAULT_LOG(Warning,
			"SurfaceNormalMap has no normal map specified.");
	}
}

}// end namespace ph
