#include "Actor/Material/ThinFilm.h"
#include "Core/Texture/TConstantTexture.h"
#include "DataIO/SDL/InputPacket.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "DataIO/PictureLoader.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/ThinDielectricFilm.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Quantity/SpectralData.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"

namespace ph
{

ThinFilm::ThinFilm() :
	SurfaceMaterial()
{}

void ThinFilm::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	std::vector<SampledSpectralStrength> reflectanceTable(91);
	std::vector<SampledSpectralStrength> transmittanceTable(91);

	for(std::size_t i = 0; i <= 90; ++i)
	{
		reflectanceTable[i] = SpectralData::calcPiecewiseAveraged(
			m_wavelengthTable.data() + i * 31,
			m_reflectanceTable.data() + i * 31,
			31);
		transmittanceTable[i] = SpectralData::calcPiecewiseAveraged(
			m_wavelengthTable.data() + i * 31,
			m_transmittanceTable.data() + i * 31,
			31);
	}

	auto optics = std::make_shared<ThinDielectricFilm>(
		std::make_shared<ExactDielectricFresnel>(1.0_r, 1.5_r),
		reflectanceTable, 
		transmittanceTable);

	behavior.setOptics(optics);
}

// command interface

ThinFilm::ThinFilm(const InputPacket& packet) :
	SurfaceMaterial(packet)
{
	m_wavelengthTable = packet.getRealArray("wavelength");
	m_reflectanceTable = packet.getRealArray("reflectance");
	m_transmittanceTable = packet.getRealArray("transmittance");
}

SdlTypeInfo ThinFilm::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "thin-film");
}

void ThinFilm::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<ThinFilm>([](const InputPacket& packet)
	{
		return std::make_unique<ThinFilm>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph
