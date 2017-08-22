#pragma once

#include "Core/SurfaceBehavior/Utility/DielectricFresnel.h"

namespace ph
{

class SchlickApproxDielectricFresnel final : public DielectricFresnel
{
public:
	SchlickApproxDielectricFresnel(real iorOuter, real iorInner);
	~SchlickApproxDielectricFresnel() override;

	virtual void calcReflectance(real cosThetaIncident, SpectralStrength* out_reflectance) const override;

private:
	real m_f0;
	real m_tirIorRatio2;
};

}// end namespace ph