#include "Engine/Actor/Material/ThinFilm.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/ThinDielectricFilm.h"
#include "Engine/Math/Color/TSampledSpectrum.h"
#include "Engine/Math/Color/spectral_samples.h"
#include "Engine/Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

namespace ph
{

ThinFilm::ThinFilm() :
	SurfaceMaterial()
{}

void ThinFilm::storeCooked(
	CookedMaterial& out_material,
	const CookingContext& ctx) const
{
	std::vector<math::SampledSpectrum> reflectanceTable(91);
	std::vector<math::SampledSpectrum> transmittanceTable(91);
	for(std::size_t i = 0; i <= 90; ++i)
	{
		reflectanceTable[i].setColorValues(
			math::resample_spectral_samples<math::ColorValue, real>(
				{m_wavelengthTable.data() + i * 31, 31}, {m_reflectanceTable.data() + i * 31, 31}));
		transmittanceTable[i].setColorValues(
			math::resample_spectral_samples<math::ColorValue, real>(
				{m_wavelengthTable.data() + i * 31, 31}, {m_transmittanceTable.data() + i * 31, 31}));
	}

	out_material.surfaceOptics = ctx.getResources()->makeSurfaceOptics<ThinDielectricFilm>(
		std::make_shared<ExactDielectricFresnel>(1.0_r, 1.5_r),
		reflectanceTable,
		transmittanceTable);
}

}// end namespace ph
