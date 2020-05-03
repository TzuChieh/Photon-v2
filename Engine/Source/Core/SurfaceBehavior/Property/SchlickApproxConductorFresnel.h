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
		real            iorOuter,
		const Spectrum& iorInnerN,
		const Spectrum& iorInnerK);

	explicit SchlickApproxConductorFresnel(const Spectrum& f0);

	void calcReflectance(
		real      cosThetaIncident, 
		Spectrum* out_reflectance) const override;

private:
	Spectrum m_f0;
	Spectrum m_f0Complement;
};

}// end namespace ph
