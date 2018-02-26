#pragma once

#include "Core/SurfaceBehavior/Property/ConductorDielectricFresnel.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class ExactConductorDielectricFresnel final : public ConductorDielectricFresnel
{
public:
	ExactConductorDielectricFresnel(real iorOuter,
	                                const SpectralStrength& iorInner, 
	                                const SpectralStrength& iorInnerK);
	virtual ~ExactConductorDielectricFresnel() override;

	virtual void calcReflectance(real cosThetaIncident, 
	                             SpectralStrength* out_reflectance) const override;

private:
	SpectralStrength m_en2_sub_ek2;
	SpectralStrength m_4_mul_en2_mul_ek2;
};

}// end namespace ph