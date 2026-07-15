#include "Engine/Actor/Material/SurfaceNormalMap.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TMicrofacetNormalMapper.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/Actor/Basic/exceptions.h"

#include <algorithm>

namespace ph
{

void SurfaceNormalMap::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	if(!m_material)
	{
		throw CookException("No target material specified for surface normal map.");
	}

	const CookedMaterial* cookedMaterial = ctx.getCooked(*m_material);

	// Fallback to base material first, then wrap it with normal map
	out_material = *cookedMaterial;
	
	if(m_map)
	{
		const auto mapTexture = m_map->genVector3RTexture(ctx);

		if(m_strengthMap)
		{
			using Strength = TTexturedSurfaceProperty<real>;
			using Optics = TMicrofacetNormalMapper<Strength>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				out_material.surfaceOptics,
				mapTexture,
				m_format,
				Strength{m_strengthMap->genRealTexture(ctx)});
		}
		else
		{
			const real strength = std::max(0.0_r, m_strength);
			if(strength == 0.0_r)
			{
				return;
			}

			if(strength == 1.0_r)
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<MicrofacetNormalMapper>(
					out_material.surfaceOptics, mapTexture, m_format);
			}
			else
			{
				using Strength = TConstantSurfaceProperty<real>;
				using Optics = TMicrofacetNormalMapper<Strength>;

				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					out_material.surfaceOptics,
					mapTexture,
					m_format,
					Strength{strength});
			}
		}
	}
	else
	{
		PH_DEFAULT_LOG(Warning,
			"SurfaceNormalMap has no normal map specified.");
	}
}

}// end namespace ph
