#pragma once

#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"

namespace ph
{

/*! @brief Dielectric-dielectric interface Fresnel effect.

The formulae used are approximations made by Schlick.
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
