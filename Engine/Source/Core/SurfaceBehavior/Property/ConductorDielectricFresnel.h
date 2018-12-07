#pragma once

#include "Common/primitive_type.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"

namespace ph
{

class ConductorDielectricFresnel : public FresnelEffect
{
public:
	ConductorDielectricFresnel();
	ConductorDielectricFresnel(real iorOuter, 
	                           const SpectralStrength& iorInner, 
	                           const SpectralStrength& iorInnerK);

	void calcReflectance(real cosThetaIncident, SpectralStrength* out_reflectance) const override = 0;

protected:
	real             m_iorOuter;
	SpectralStrength m_iorInner;
	SpectralStrength m_iorInnerK;
};

}// end namespace ph