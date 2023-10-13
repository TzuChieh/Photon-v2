#include "Actor/Material/BinaryMixedSurfaceMaterial.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Actor/Image/ConstantImage.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Actor/Basic/exceptions.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(BinaryMixedSurfaceMaterial, Material);

void BinaryMixedSurfaceMaterial::genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const
{
	if(!m_material0 || !m_material1)
	{
		throw CookException("One or more materials are empty. Cannot perform binary mix operation.");
	}

	SurfaceBehavior behavior0, behavior1;
	m_material0->genSurface(ctx, behavior0);
	m_material1->genSurface(ctx, behavior1);
	auto optics0 = behavior0.getOpticsResource();
	auto optics1 = behavior1.getOpticsResource();
	if(!optics0 || !optics1)
	{
		throw CookException("Surface optics generation failed. Cannot perform binary mix operation.");
	}

	switch(m_mode)
	{
	case ESurfaceMaterialMixMode::Lerp:
		if(m_factor)
		{
			auto factor = m_factor->genColorTexture(ctx);
			behavior.setOptics(std::make_shared<LerpedSurfaceOptics>(optics0, optics1, factor));
		}
		else
		{
			PH_LOG_WARNING(BinaryMixedSurfaceMaterial,
				"No lerp factor specified. The result might not be what you want.");
			behavior.setOptics(std::make_shared<LerpedSurfaceOptics>(optics0, optics1));
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
