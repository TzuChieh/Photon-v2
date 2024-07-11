#pragma once

#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"

namespace ph
{

/*! @brief Dielectric-dielectric interface Fresnel effect.

The formulae used are dielectric-specific approximations made by Brent Burley
@cite Burley:2015:Extending, which provide some improvement over the approximation originally done
by Schlick @cite Schlick:1994:BRDF. See the implementation of `calcReflectance()` for more details.
*/
class SchlickApproxDielectricFresnel : public DielectricFresnel
{
public:
	SchlickApproxDielectricFresnel(real iorOuter, real iorInner);

	void calcReflectance(
		real            cosThetaIncident, 
		math::Spectrum* out_reflectance) const override;

private:
	real m_f0;
	real m_tirIorRatio2;
};

}// end namespace ph
