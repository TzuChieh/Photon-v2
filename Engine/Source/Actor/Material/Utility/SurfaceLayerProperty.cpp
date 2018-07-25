#include "Actor/Material/Utility/SurfaceLayerProperty.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/Quantity/SpectralData.h"
#include "FileIO/SDL/InputPrototype.h"

namespace ph
{

SurfaceLayerProperty::SurfaceLayerProperty() : 
	m_roughness(0.0_r), 
	m_iorN(1.0_r), 
	m_iorK(0.0_r)
{}

SurfaceLayerProperty::SurfaceLayerProperty(const InputPacket& packet) : 
	SurfaceLayerProperty()
{
	m_roughness = packet.getReal("roughness", m_roughness);

	InputPrototype exactIorN;
	exactIorN.addRealArray("ior-n-wavelength-nm");
	exactIorN.addRealArray("ior-n");

	InputPrototype exactIorK;
	exactIorK.addRealArray("ior-k-wavelength-nm");
	exactIorK.addRealArray("ior-k");

	if(packet.isPrototypeMatched(exactIorN))
	{
		const auto wavelengths = packet.getRealArray("ior-n-wavelength-nm");
		const auto iorNs       = packet.getRealArray("ior-n");

		m_iorN.setSampled(SpectralData::calcPiecewiseAveraged(
			wavelengths.data(), iorNs.data(), wavelengths.size()));
	}
	else if(packet.hasReal("ior-n"))
	{
		m_iorN.setValues(packet.getReal("ior-n"));
	}

	if(packet.isPrototypeMatched(exactIorK))
	{
		const auto wavelengths = packet.getRealArray("ior-k-wavelength-nm");
		const auto iorKs       = packet.getRealArray("ior-k");

		m_iorK.setSampled(SpectralData::calcPiecewiseAveraged(
			wavelengths.data(), iorKs.data(), wavelengths.size()));
	}
	else if(packet.hasReal("ior-k"))
	{
		m_iorK.setValues(packet.getReal("ior-k"));
	}
}

}// end namespace ph