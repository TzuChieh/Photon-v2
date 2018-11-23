#include "Actor/Material/Utility/SurfaceLayerProperty.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/Quantity/SpectralData.h"
#include "FileIO/SDL/InputPrototype.h"

namespace ph
{

SurfaceLayerProperty::SurfaceLayerProperty() : 
	m_roughness(0.0_r), 
	m_iorN(1.0_r), 
	m_iorK(0.0_r),
	m_depth(0.0_r),
	m_g(1.0_r),
	m_sigmaA(0.0_r),
	m_sigmaS(0.0_r)
{}

SurfaceLayerProperty::SurfaceLayerProperty(const InputPacket& packet) : 
	SurfaceLayerProperty()
{
	m_roughness = packet.getReal("roughness", m_roughness);

	m_iorN = loadIorN(packet, m_iorN);
	m_iorK = loadIorK(packet, m_iorK);

	m_depth = packet.getReal("depth", m_depth);
	m_g     = packet.getReal("g", m_g);
	m_sigmaA.setValues(packet.getReal("sigma-a", m_sigmaA.avg()));
	m_sigmaS.setValues(packet.getReal("sigma-s", m_sigmaS.avg()));
}

SpectralStrength SurfaceLayerProperty::loadIorN(const InputPacket& packet, const SpectralStrength& defaultIorN)
{
	SpectralStrength iorN(defaultIorN);

	InputPrototype exactIorN;
	exactIorN.addRealArray("ior-n-wavelength-nm");
	exactIorN.addRealArray("ior-n");

	if(packet.isPrototypeMatched(exactIorN))
	{
		const auto wavelengths = packet.getRealArray("ior-n-wavelength-nm");
		const auto iorNs       = packet.getRealArray("ior-n");

		iorN.setSampled(
			SpectralData::calcPiecewiseAveraged(wavelengths.data(), iorNs.data(), wavelengths.size()), 
			EQuantity::RAW);
	}
	else if(packet.hasVector3("ior-n"))
	{
		iorN.setLinearSrgb(packet.getVector3("ior-n"), EQuantity::RAW);
	}
	else if(packet.hasReal("ior-n"))
	{
		iorN.setValues(packet.getReal("ior-n"));
	}

	return iorN;
}

SpectralStrength SurfaceLayerProperty::loadIorK(const InputPacket& packet, const SpectralStrength& defaultIorK)
{
	SpectralStrength iorK(defaultIorK);

	InputPrototype exactIorK;
	exactIorK.addRealArray("ior-k-wavelength-nm");
	exactIorK.addRealArray("ior-k");

	if(packet.isPrototypeMatched(exactIorK))
	{
		const auto wavelengths = packet.getRealArray("ior-k-wavelength-nm");
		const auto iorKs       = packet.getRealArray("ior-k");

		iorK.setSampled(
			SpectralData::calcPiecewiseAveraged(wavelengths.data(), iorKs.data(), wavelengths.size()), 
			EQuantity::RAW);
	}
	else if(packet.hasVector3("ior-k"))
	{
		iorK.setLinearSrgb(packet.getVector3("ior-k"), EQuantity::RAW);
	}
	else if(packet.hasReal("ior-k"))
	{
		iorK.setValues(packet.getReal("ior-k"));
	}

	return iorK;
}

}// end namespace ph