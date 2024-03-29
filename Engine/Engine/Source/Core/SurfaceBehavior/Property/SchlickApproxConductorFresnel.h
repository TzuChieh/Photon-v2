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
		real                  iorOuter,
		const math::Spectrum& iorInnerN,
		const math::Spectrum& iorInnerK);

	explicit SchlickApproxConductorFresnel(const math::Spectrum& f0);

	void calcReflectance(
		real            cosThetaIncident, 
		math::Spectrum* out_reflectance) const override;

private:
	math::Spectrum m_f0;
	math::Spectrum m_f0Complement;
};

}// end namespace ph
