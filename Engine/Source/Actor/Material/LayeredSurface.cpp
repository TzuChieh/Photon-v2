#include "Actor/Material/LayeredSurface.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"
#include "FileIO/SDL/InputPacket.h"

namespace ph
{

LayeredSurface::LayeredSurface() : 
	SurfaceMaterial(),
	m_layers()
{}

void LayeredSurface::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	std::vector<real>             alphas;
	std::vector<SpectralStrength> iorNs;
	std::vector<SpectralStrength> iorKs;
	std::vector<real>             depths;
	std::vector<real>             gs;
	std::vector<SpectralStrength> sigmaAs;
	std::vector<SpectralStrength> sigmaSs;
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
	m_layers.push_back(SurfaceLayerProperty());
}

void LayeredSurface::setLayer(const std::size_t layerIndex, const SurfaceLayerProperty& layer)
{
	if(layerIndex >= m_layers.size())
	{
		std::cerr << "warning: at LayeredSurface::setLayer(2), invalid index" << std::endl;
		return;
	}

	m_layers[layerIndex] = layer;
}

// command interface

namespace
{

	ExitStatus ciAddLayer(const std::shared_ptr<LayeredSurface>& target, const InputPacket& packet)
	{
		target->addLayer();

		return ExitStatus::SUCCESS();
	}

	ExitStatus ciSetLayer(const std::shared_ptr<LayeredSurface>& target, const InputPacket& packet)
	{
		const integer layerIndex = packet.getInteger("index", -1, DataTreatment::REQUIRED());
		target->setLayer(static_cast<std::size_t>(layerIndex), SurfaceLayerProperty(packet));

		return ExitStatus::SUCCESS();
	}

}

LayeredSurface::LayeredSurface(const InputPacket& packet) : 
	SurfaceMaterial(packet),
	m_layers()
{}

SdlTypeInfo LayeredSurface::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "layered-surface");
}

void LayeredSurface::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<LayeredSurface>(packet);
	}));

	SdlExecutor addLayerSE;
	addLayerSE.setName("add");
	addLayerSE.setFunc<LayeredSurface>(ciAddLayer);
	cmdRegister.addExecutor(addLayerSE);

	SdlExecutor setLayerSE;
	setLayerSE.setName("set");
	setLayerSE.setFunc<LayeredSurface>(ciSetLayer);
	cmdRegister.addExecutor(setLayerSE);
}

}// end namespace ph
	