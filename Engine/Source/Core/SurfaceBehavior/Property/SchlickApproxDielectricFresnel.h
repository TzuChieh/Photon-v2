#pragma once

#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"

namespace ph
{

class SchlickApproxDielectricFresnel : public DielectricFresnel
{
public:
	SchlickApproxDielectricFresnel(real iorOuter, real iorInner);

	void calcReflectance(real cosThetaIncident, SpectralStrength* out_reflectance) const override;

private:
	real m_f0;
	real m_tirIorRatio2;
};

}// end namespace ph