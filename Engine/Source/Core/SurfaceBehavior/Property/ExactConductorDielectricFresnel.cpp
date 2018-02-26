#include "Core/SurfaceBehavior/Property/ExactConductorDielectricFresnel.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

ExactConductorDielectricFresnel::ExactConductorDielectricFresnel(
	const real              iorOuter,
	const SpectralStrength& iorInner,
	const SpectralStrength& iorInnerK) : 

	ConductorDielectricFresnel(iorOuter, iorInner, iorInnerK)
{
	const SpectralStrength en2 = iorInner.div(iorOuter).pow(2);
	const SpectralStrength ek2 = iorInnerK.div(iorOuter).pow(2);
	m_en2_sub_ek2       = en2.sub(ek2);
	m_4_mul_en2_mul_ek2 = en2.mul(ek2).mul(4);
}

ExactConductorDielectricFresnel::~ExactConductorDielectricFresnel() = default;

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

	const SpectralStrength& t0       = m_en2_sub_ek2.sub(sinI2);
	const SpectralStrength& a2plusb2 = t0.mul(t0).addLocal(m_4_mul_en2_mul_ek2).sqrtLocal();
	const SpectralStrength& a        = a2plusb2.add(t0).mulLocal(0.5_r).sqrtLocal();
	const SpectralStrength& t1       = a2plusb2.add(cosI2);
	const SpectralStrength& t2       = a.mul(2.0_r * cosI);
	const SpectralStrength& t3       = a2plusb2.mul(cosI2).addLocal(sinI2 * sinI2);
	const SpectralStrength& t4       = t2.mul(sinI2);

	const SpectralStrength& Rs = t1.sub(t2).divLocal(t1.add(t2));
	const SpectralStrength& Rp = Rs.mul(t3.sub(t4).divLocal(t3.add(t4)));
	*out_reflectance = Rs.add(Rp).mulLocal(0.5_r);
}

}// end namespace ph