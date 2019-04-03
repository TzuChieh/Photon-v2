#pragma once

#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"

namespace ph
{

/*! @brief Conductor-dielectric interface Fresnel effect.

The formulae used are approximations made by Schlick.
*/
class SchlickApproxConductorFresnel : public ConductorFresnel
{
public:
	SchlickApproxConductorFresnel(
		real                    iorOuter,
		const SpectralStrength& iorInner, 
		const SpectralStrength& iorInnerK);

	explicit SchlickApproxConductorFresnel(const SpectralStrength& f0);

	void calcReflectance(
		real              cosThetaIncident, 
		SpectralStrength* out_reflectance) const override;

private:
	SpectralStrength m_f0;
	SpectralStrength m_f0Complement;
};

}// end namespace ph