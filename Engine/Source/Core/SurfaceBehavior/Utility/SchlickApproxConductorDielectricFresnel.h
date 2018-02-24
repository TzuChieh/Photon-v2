#pragma once

#include "Core/SurfaceBehavior/Utility/ConductorDielectricFresnel.h"

namespace ph
{

class SchlickApproxConductorDielectricFresnel : public ConductorDielectricFresnel
{
public:
	SchlickApproxConductorDielectricFresnel(real iorOuter,
	                                        const SpectralStrength& iorInner, 
	                                        const SpectralStrength& iorInnerK);
	SchlickApproxConductorDielectricFresnel(const SpectralStrength& f0);
	virtual ~SchlickApproxConductorDielectricFresnel() override = default;

	virtual void calcReflectance(real cosThetaIncident, 
	                             SpectralStrength* out_reflectance) const override;

private:
	SpectralStrength m_f0;
	SpectralStrength m_f0Complement;
};

}// end namespace ph