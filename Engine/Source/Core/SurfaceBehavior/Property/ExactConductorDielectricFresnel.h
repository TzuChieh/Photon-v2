#pragma once

#include "Core/SurfaceBehavior/Property/ConductorDielectricFresnel.h"
#include "Core/Quantity/SpectralStrength.h"

#include <vector>

namespace ph
{

class ExactConductorDielectricFresnel final : public ConductorDielectricFresnel
{
public:
	ExactConductorDielectricFresnel(real iorOuter,
	                                const SpectralStrength& iorInner, 
	                                const SpectralStrength& iorInnerK);
	ExactConductorDielectricFresnel(real iorOuter,
	                                const std::vector<real>& iorWavelengthsNm, 
	                                const std::vector<real>& iorInnerNs, 
	                                const std::vector<real>& iorInnerKs);
	virtual ~ExactConductorDielectricFresnel() override;

	virtual void calcReflectance(real cosThetaIncident, 
	                             SpectralStrength* out_reflectance) const override;

private:
	SpectralStrength m_en2_sub_ek2;
	SpectralStrength m_4_mul_en2_mul_ek2;

	void setIors(real iorOuter,
	             const SpectralStrength& iorInner,
	             const SpectralStrength& iorInnerK);
};

}// end namespace ph