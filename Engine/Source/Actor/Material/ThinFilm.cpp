#include "Actor/Material/ThinFilm.h"
#include "Core/Texture/TConstantTexture.h"
#include "FileIO/SDL/InputPacket.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "FileIO/PictureLoader.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/ThinDielectricFilm.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Quantity/SpectralData.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Actor/Material/ThinFilm/InterferenceTable.h"
#include "Actor/Material/ThinFilm/InterferenceTableMetal.h"

namespace ph
{

ThinFilm::ThinFilm() :

	SurfaceMaterial(),

	m_iorOuter(1.0_r),
	m_iorFilm(1.5_r),
	m_iorInner(1.0_r),
	m_thicknessNm(500.0_r)
{}

void ThinFilm::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	InterferenceTableMetal table({m_iorOuter, m_iorFilm, m_iorInner}, {0, m_thicknessNm, 0});
	// table.enable_debug();
	// table.read_iorfile("../IorFile/Au.csv", 1);
	// table.read_iorfile("../IorFile/Ag.csv", 2);
	table.simulate_single_thin_film();
	// table.output_log();

	const auto wavelengthsNm = table.getWavelengthsNm();
	auto reflectances = table.getReflectances();
	auto transmittances = table.getTransmittance();

	std::vector<SampledSpectralStrength> reflectanceTable(91);
	std::vector<SampledSpectralStrength> transmittanceTable(91);
	for(std::size_t i = 0; i <= 90; ++i)
	{
		reflectanceTable[i] = SpectralData::calcPiecewiseAveraged(
			wavelengthsNm.data() + i * 31,
			reflectances.data() + i * 31,
			31);
		transmittanceTable[i] = SpectralData::calcPiecewiseAveraged(
			wavelengthsNm.data() + i * 31,
			transmittances.data() + i * 31,
			31);
	}

	auto optics = std::make_shared<ThinDielectricFilm>(
		reflectanceTable,
		transmittanceTable);

	behavior.setOptics(optics);
}

// command interface

ThinFilm::ThinFilm(const InputPacket& packet) :
	SurfaceMaterial(packet)
{
	/*m_wavelengthTable = packet.getRealArray("wavelength");
	m_reflectanceTable = packet.getRealArray("reflectance");
	m_transmittanceTable = packet.getRealArray("transmittance");*/

	m_iorOuter = packet.getReal("ior-outer", 1.0_r);
	m_iorFilm = packet.getReal("ior-film", 1.5_r);
	m_iorInner = packet.getReal("ior-inner", 1.0_r);
	m_thicknessNm = packet.getReal("thickness-nm", 500.0_r);
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
