#pragma once

#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"
#include "Math/Color/Spectrum.h"

#include <vector>

namespace ph
{

/*! @brief Conductor-dielectric interface Fresnel effect.

The formulae used are exact ones, i.e., no approximation is made.
*/
class ExactConductorFresnel : public ConductorFresnel
{
public:
	ExactConductorFresnel(
		real                  iorOuter,
		const math::Spectrum& iorInnerN,
		const math::Spectrum& iorInnerK);

	ExactConductorFresnel(
		real                     iorOuter,
		const std::vector<real>& iorWavelengthsNm, 
		const std::vector<real>& iorInnerNs, 
		const std::vector<real>& iorInnerKs);

	void calcReflectance(
		real            cosThetaIncident, 
		math::Spectrum* out_reflectance) const override;

private:
	real           m_iorOuter;
	math::Spectrum m_iorInnerN;
	math::Spectrum m_iorInnerK;
	math::Spectrum m_en2_sub_ek2;
	math::Spectrum m_4_mul_en2_mul_ek2;

	void setIors(
		real                  iorOuter,
		const math::Spectrum& iorInnerN,
		const math::Spectrum& iorInnerK);
};

}// end namespace ph
