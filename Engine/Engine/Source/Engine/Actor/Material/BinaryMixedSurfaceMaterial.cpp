#include "Engine/Actor/Material/BinaryMixedSurfaceMaterial.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TLerpedSurfaceOptics.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>
#include <utility>

namespace ph
{

void BinaryMixedSurfaceMaterial::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	if(!m_material0 || !m_material1)
	{
		throw CookException("One or more materials are empty. Cannot perform binary mix operation.");
	}

	const CookedMaterial* cookedMaterial0 = ctx.getCooked(*m_material0);
	const CookedMaterial* cookedMaterial1 = ctx.getCooked(*m_material1);
	if(!cookedMaterial0->surfaceOptics || !cookedMaterial1->surfaceOptics)
	{
		throw CookException("Surface optics generation failed. Cannot perform binary mix operation.");
	}

	switch(m_mode)
	{
	case ESurfaceMaterialMixMode::Lerp:
		if(m_factorMap)
		{
			using Factor = TTexturedSurfaceProperty<math::Spectrum, math::EColorUsage::ECF>;
			using Optics = TLerpedSurfaceOptics<Factor>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				cookedMaterial0->surfaceOptics,
				cookedMaterial1->surfaceOptics,
				Factor(m_factorMap->genColorTexture(ctx)));
		}
		else
		{
			using Factor = TConstantSurfaceProperty<math::Spectrum>;
			using Optics = TLerpedSurfaceOptics<Factor>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				cookedMaterial0->surfaceOptics,
				cookedMaterial1->surfaceOptics,
				Factor(m_factor));
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
	setFactor(math::Spectrum(factor));
}

void BinaryMixedSurfaceMaterial::setFactor(const math::Spectrum& factor)
{
	m_factor = factor;
}

void BinaryMixedSurfaceMaterial::setFactor(std::shared_ptr<Image> factor)
{
	setFactorMap(std::move(factor));
}

void BinaryMixedSurfaceMaterial::setFactorMap(std::shared_ptr<Image> factorMap)
{
	m_factorMap = std::move(factorMap);
}

}// end namespace ph
