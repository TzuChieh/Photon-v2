#include "Engine/Actor/Material/BinaryMixedSurfaceMaterial.h"
#include "Engine/Actor/Image/ConstantImage.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(BinaryMixedSurfaceMaterial, Material);

void BinaryMixedSurfaceMaterial::storeCooked(
	CookedMaterial& out_material,
	const CookingContext& ctx) const
{
	if(!m_material0 || !m_material1)
	{
		throw CookException("One or more materials are empty. Cannot perform binary mix operation.");
	}

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

void BinaryMixedSurfaceMaterial::setMode(const ESurfaceMaterialMixMode mode)
{
	m_mode = mode;
}

void BinaryMixedSurfaceMaterial::setMaterials(
	std::shared_ptr<SurfaceMaterial> material0,
	std::shared_ptr<SurfaceMaterial> material1)
{
	m_material0 = std::move(material0);
	m_material1 = std::move(material1);
}

void BinaryMixedSurfaceMaterial::setFactor(const real factor)
{
	auto imageFactor = TSdl<ConstantImage>::makeResource();
	imageFactor->setColor(factor, math::EColorSpace::Spectral);
	setFactor(imageFactor);
}

void BinaryMixedSurfaceMaterial::setFactor(std::shared_ptr<Image> factor)
{
	m_factor = std::move(factor);
}

}// end namespace ph
