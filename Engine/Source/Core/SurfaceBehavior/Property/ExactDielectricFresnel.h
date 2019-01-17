#pragma once

#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"

namespace ph
{

class ExactDielectricFresnel : public DielectricFresnel
{
public:
	ExactDielectricFresnel(real iorOuter, real iorInner);

	void calcReflectance(real cosThetaIncident, SpectralStrength* out_reflectance) const override;
};

}// end namespace ph