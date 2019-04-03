#pragma once

#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"
#include "Core/Quantity/SpectralStrength.h"

#include <vector>

namespace ph
{

/*! @brief Conductor-dielectric interface Fresnel effect.

The formulae used are exact ones, i.e., no approximation is made.
*/
class ExactConductorFresnel final : public ConductorFresnel
{
public:
	ExactConductorFresnel(
		real                    iorOuter,
		const SpectralStrength& iorInner, 
		const SpectralStrength& iorInnerK);

	ExactConductorFresnel(
		real                     iorOuter,
		const std::vector<real>& iorWavelengthsNm, 
		const std::vector<real>& iorInnerNs, 
		const std::vector<real>& iorInnerKs);

	void calcReflectance(
		real              cosThetaIncident, 
		SpectralStrength* out_reflectance) const override;

private:
	SpectralStrength m_en2_sub_ek2;
	SpectralStrength m_4_mul_en2_mul_ek2;

	void setIors(
		real                    iorOuter,
		const SpectralStrength& iorInner,
		const SpectralStrength& iorInnerK);
};

}// end namespace ph