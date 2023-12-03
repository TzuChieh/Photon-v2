#include "Actor/Material/ThinFilm.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/ThinDielectricFilm.h"
#include "Math/Color/TSampledSpectrum.h"
#include "Math/Color/spectral_samples.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"

namespace ph
{

ThinFilm::ThinFilm() :
	SurfaceMaterial()
{}

void ThinFilm::genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const
{
	std::vector<math::SampledSpectrum> reflectanceTable(91);
	std::vector<math::SampledSpectrum> transmittanceTable(91);
	for(std::size_t i = 0; i <= 90; ++i)
	{
		reflectanceTable[i].setColorValues(math::resample_spectral_samples<math::ColorValue, real>(
			m_wavelengthTable.data() + i * 31,
			m_reflectanceTable.data() + i * 31,
			31));
		transmittanceTable[i].setColorValues(math::resample_spectral_samples<math::ColorValue, real>(
			m_wavelengthTable.data() + i * 31,
			m_transmittanceTable.data() + i * 31,
			31));
	}

	auto optics = std::make_shared<ThinDielectricFilm>(
		std::make_shared<ExactDielectricFresnel>(1.0_r, 1.5_r),
		reflectanceTable, 
		transmittanceTable);

	behavior.setOptics(optics);
}

}// end namespace ph
