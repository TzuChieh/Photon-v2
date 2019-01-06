#include "Core/SurfaceBehavior/Property/ExactConductorDielectricFresnel.h"
#include "Common/assertion.h"
#include "Core/Quantity/SpectralData.h"

#include <cmath>
#include <iostream>

namespace ph
{

ExactConductorDielectricFresnel::ExactConductorDielectricFresnel(
	const real              iorOuter,
	const SpectralStrength& iorInner,
	const SpectralStrength& iorInnerK) : 

	ConductorDielectricFresnel()
{
	setIors(iorOuter, iorInner, iorInnerK);
}

ExactConductorDielectricFresnel::ExactConductorDielectricFresnel(
	const real               iorOuter,
	const std::vector<real>& iorWavelengthsNm,
	const std::vector<real>& iorInnerNs,
	const std::vector<real>& iorInnerKs) : 

	ConductorDielectricFresnel()
{
	if(iorWavelengthsNm.size() != iorInnerNs.size() ||
	   iorWavelengthsNm.size() != iorInnerKs.size())
	{
		std::cerr << "warning: at ExactConductorDielectricFresnel ctor, "
		          << "irregular-sized input data detected" << std::endl;
		return;
	}

	const auto& sampledInnerNs = SpectralData::calcPiecewiseAveraged(
		iorWavelengthsNm.data(), iorInnerNs.data(), iorWavelengthsNm.size());
	const auto& sampledInnerKs = SpectralData::calcPiecewiseAveraged(
		iorWavelengthsNm.data(), iorInnerKs.data(), iorWavelengthsNm.size());

	SpectralStrength iorInnerN, iorInnerK;
	iorInnerN.setSampled(sampledInnerNs);
	iorInnerK.setSampled(sampledInnerKs);
	setIors(iorOuter, iorInnerN, iorInnerK);
}

// Implementation follows the excellent blog post written by Sebastien Lagarde.
// Reference: https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
//
void ExactConductorDielectricFresnel::calcReflectance(
	const real              cosThetaIncident,
	SpectralStrength* const out_reflectance) const
{
	PH_ASSERT(out_reflectance != nullptr);

	// We treat the incident light be always in the dielectric side (which is
	// reasonable since light should not penetrate conductors easily), so the 
	// sign of cosI does not matter here.
	//
	const real cosI  = std::abs(cosThetaIncident);
	const real cosI2 = cosI * cosI;
	const real sinI2 = 1.0_r - cosI * cosI;

	const SpectralStrength t0       = m_en2_sub_ek2.sub(sinI2);
	const SpectralStrength a2plusb2 = t0.mul(t0).addLocal(m_4_mul_en2_mul_ek2).sqrtLocal();
	const SpectralStrength a        = a2plusb2.add(t0).mulLocal(0.5_r).sqrtLocal();
	const SpectralStrength t1       = a2plusb2.add(cosI2);
	const SpectralStrength t2       = a.mul(2.0_r * cosI);
	const SpectralStrength t3       = a2plusb2.mul(cosI2).addLocal(sinI2 * sinI2);
	const SpectralStrength t4       = t2.mul(sinI2);

	const SpectralStrength Rs = t1.sub(t2).divLocal(t1.add(t2));
	const SpectralStrength Rp = Rs.mul(t3.sub(t4).divLocal(t3.add(t4)));
	*out_reflectance = Rs.add(Rp).mulLocal(0.5_r);
}

void ExactConductorDielectricFresnel::setIors(
	const real              iorOuter,
	const SpectralStrength& iorInner,
	const SpectralStrength& iorInnerK)
{
	m_iorOuter  = iorOuter;
	m_iorInner  = iorInner;
	m_iorInnerK = iorInnerK;

	const SpectralStrength en2 = iorInner.div(iorOuter).pow(2);
	const SpectralStrength ek2 = iorInnerK.div(iorOuter).pow(2);
	m_en2_sub_ek2       = en2.sub(ek2);
	m_4_mul_en2_mul_ek2 = en2.mul(ek2).mul(4);
}

}// end namespace ph