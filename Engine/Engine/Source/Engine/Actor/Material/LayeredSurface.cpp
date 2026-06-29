#include "Engine/Actor/Material/LayeredSurface.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>

namespace ph
{

void LayeredSurface::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	std::vector<real>           alphas;
	std::vector<math::Spectrum> iorNs;
	std::vector<math::Spectrum> iorKs;
	std::vector<real>           depths;
	std::vector<real>           gs;
	std::vector<math::Spectrum> sigmaAs;
	std::vector<math::Spectrum> sigmaSs;
	for(const auto& layer : m_layers)
	{
		alphas.push_back(layer.getAlpha());
		iorNs.push_back(layer.getIorN());
		iorKs.push_back(layer.getIorK());
		depths.push_back(layer.getDepth());
		gs.push_back(layer.getG());
		sigmaAs.push_back(layer.getSigmaA());
		sigmaSs.push_back(layer.getSigmaS());
	}

	out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<LbLayeredSurface>(
		iorNs, iorKs, alphas, depths, gs, sigmaAs, sigmaSs);
}

void LayeredSurface::addLayer()
{
	m_layers.push_back(SurfaceLayerInfo());
}

void LayeredSurface::setLayer(const std::size_t layerIndex, const SurfaceLayerInfo& layer)
{
	PH_ASSERT_LT(layerIndex, m_layers.size());
	m_layers[layerIndex] = layer;
}

}// end namespace ph
