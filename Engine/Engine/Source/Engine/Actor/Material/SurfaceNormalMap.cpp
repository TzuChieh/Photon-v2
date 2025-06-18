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

	if(!m_map)
	{
		PH_DEFAULT_LOG(Warning,
			"SurfaceNormalMap has no normal map specified.");
	}

	std::shared_ptr<TTexture<real>> maskTexture;
	{
		auto mask = TSdl<SwizzledImage>::makeResource();
		mask->setInput(m_mask);
		mask->setSwizzleSubscripts("x");

		maskTexture = mask->genRealTexture(ctx);
	}

	auto const normalMapper = ctx.getResources().makeSurfaceOptics<MicrofacetNormalMapper>(
		out_material.surfaceOptics,
		factor);

	const CookedMaterial* cookedMaterial0 = m_material0->createCooked(ctx);
	const CookedMaterial* cookedMaterial1 = m_material1->createCooked(ctx);

	if(!(cookedMaterial0 && cookedMaterial0->surfaceOptics) || 
	   !(cookedMaterial1 && cookedMaterial1->surfaceOptics))
	{
		throw CookException("Surface optics generation failed. Cannot perform binary mix operation.");
	}

	switch(m_mode)
	{
	case ESurfaceMaterialMixMode::Lerp:
		if(m_factor)
		{
			auto factor = m_factor->genColorTexture(ctx);
			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<LerpedSurfaceOptics>(
				cookedMaterial0->surfaceOptics,
				cookedMaterial1->surfaceOptics,
				factor);
		}
		else
		{
			PH_LOG(BinaryMixedSurfaceMaterial, Warning,
				"No lerp factor specified. The result might not be what you want.");
			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<LerpedSurfaceOptics>(
				cookedMaterial0->surfaceOptics,
				cookedMaterial1->surfaceOptics);
		}
		break;

	default:
		throw CookException("Unsupported material mixing mode.");
		break;
	}
}

}// end namespace ph
