#pragma once

#include "Common/primitive_type.h"
#include "Core/SurfaceBehavior/Utility/FresnelEffect.h"

namespace ph
{

class ConductorDielectricFresnel : public FresnelEffect
{
public:
	ConductorDielectricFresnel(real iorOuter, 
	                           const SpectralStrength& iorInner, 
	                           const SpectralStrength& iorInnerK);
	virtual ~ConductorDielectricFresnel() override;

	virtual void calcReflectance(real cosThetaIncident, SpectralStrength* out_reflectance) const = 0;

protected:
	real             m_iorOuter;
	SpectralStrength m_iorInner;
	SpectralStrength m_iorInnerK;
};

}// end namespace ph