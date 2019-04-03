#pragma once

#include "Common/primitive_type.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"

namespace ph
{

/*! @brief Conductor-dielectric interface Fresnel effect.

Fresnel effect where the inner side is conductor and the outer side is 
dielectric. We do not explicitly state conductor-dielectric in the type name
since light energy does not penetrate conductors significantly, so the 
situation where a dedicated type for conductor-conductor interface that 
conflicts with the current name is highly unlikely.
*/
class ConductorFresnel : public FresnelEffect
{
public:
	ConductorFresnel();
	ConductorFresnel(
		real                    iorOuter, 
		const SpectralStrength& iorInner, 
		const SpectralStrength& iorInnerK);

	void calcReflectance(
		real              cosThetaIncident, 
		SpectralStrength* out_reflectance) const override = 0;

protected:
	real             m_iorOuter;
	SpectralStrength m_iorInner;
	SpectralStrength m_iorInnerK;
};

}// end namespace ph