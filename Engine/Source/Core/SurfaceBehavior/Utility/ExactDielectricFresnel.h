#pragma once

#include "Core/SurfaceBehavior/Utility/DielectricFresnel.h"

namespace ph
{

class ExactDielectricFresnel final : public DielectricFresnel
{
public:
	ExactDielectricFresnel(real iorOuter, real iorInner);
	virtual ~ExactDielectricFresnel() override;

	virtual void calcReflectance(real cosThetaIncident, SpectralStrength* out_reflectance) const override;
};

}// end namespace ph