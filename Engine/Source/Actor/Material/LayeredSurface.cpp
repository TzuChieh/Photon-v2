#include "Actor/Material/LayeredSurface.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"

namespace ph
{

LayeredSurface::LayeredSurface() : 
	SurfaceMaterial(),
	m_layers()
{}

void LayeredSurface::genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const
{
	std::vector<real>     alphas;
	std::vector<Spectrum> iorNs;
	std::vector<Spectrum> iorKs;
	std::vector<real>     depths;
	std::vector<real>     gs;
	std::vector<Spectrum> sigmaAs;
	std::vector<Spectrum> sigmaSs;
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

	std::cout << iorKs.back().toString();

	behavior.setOptics(std::make_shared<LbLayeredSurface>(iorNs, iorKs, alphas, depths, gs, sigmaAs, sigmaSs));
}

void LayeredSurface::addLayer()
{
	m_layers.push_back(SurfaceLayerInfo());
}

void LayeredSurface::setLayer(const std::size_t layerIndex, const SurfaceLayerInfo& layer)
{
	if(layerIndex >= m_layers.size())
	{
		std::cerr << "warning: at LayeredSurface::setLayer(2), invalid index" << std::endl;
		return;
	}

	m_layers[layerIndex] = layer;
}

}// end namespace ph
