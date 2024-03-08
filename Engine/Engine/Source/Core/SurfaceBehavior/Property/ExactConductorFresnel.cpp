#include "Core/SurfaceBehavior/Property/ExactConductorFresnel.h"
#include "Math/Color/spectral_samples.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <cmath>

namespace ph
{

ExactConductorFresnel::ExactConductorFresnel(
	const real            iorOuter,
	const math::Spectrum& iorInnerN,
	const math::Spectrum& iorInnerK)

	: ConductorFresnel()

	, m_iorOuter{}
	, m_iorInnerN{}
	, m_iorInnerK{}
	, m_en2_sub_ek2{}
	, m_4_mul_en2_mul_ek2{}
{
	setIors(iorOuter, iorInnerN, iorInnerK);
}

ExactConductorFresnel::ExactConductorFresnel(
	const real               iorOuter,
	const std::vector<real>& iorWavelengthsNm,
	const std::vector<real>& iorInnerNs,
	const std::vector<real>& iorInnerKs)

	: ConductorFresnel()
	
	, m_iorOuter{}
	, m_iorInnerN{}
	, m_iorInnerK{}
	, m_en2_sub_ek2{}
	, m_4_mul_en2_mul_ek2{}
{
	if(iorWavelengthsNm.size() != iorInnerNs.size() ||
	   iorWavelengthsNm.size() != iorInnerKs.size())
	{
		PH_DEFAULT_LOG(Warning,
			"at ExactConductorFresnel ctor, irregular-sized input data detected "
			"(#iorWavelengthsNm: {}, #iorInnerNs: {}, #iorInnerKs: {})",
			iorWavelengthsNm.size(), iorInnerNs.size(), iorInnerKs.size());
		return;
	}

	// TODO: this conversion part should be performed in data containers
	const auto& sampledInnerNs = math::resample_spectral_samples<math::ColorValue, real>(
		iorWavelengthsNm.data(), iorInnerNs.data(), iorWavelengthsNm.size());
	const auto& sampledInnerKs = math::resample_spectral_samples<math::ColorValue, real>(
		iorWavelengthsNm.data(), iorInnerKs.data(), iorWavelengthsNm.size());

	math::Spectrum iorInnerN, iorInnerK;
	iorInnerN.setSpectral(sampledInnerNs, math::EColorUsage::RAW);
	iorInnerK.setSpectral(sampledInnerKs, math::EColorUsage::RAW);
	setIors(iorOuter, iorInnerN, iorInnerK);
}

// Implementation follows the excellent blog post written by Sebastien Lagarde.
// Reference: https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
void ExactConductorFresnel::calcReflectance(
	const real            cosThetaIncident,
	math::Spectrum* const out_reflectance) const
{
	PH_ASSERT(out_reflectance);

	// We treat the incident light be always in the dielectric side (which is
	// reasonable since light should not penetrate conductors easily), so the 
	// sign of cosI does not matter here.
	const real cosI  = std::abs(cosThetaIncident);
	const real cosI2 = cosI * cosI;
	const real sinI2 = 1.0_r - cosI * cosI;

	const math::Spectrum t0       = m_en2_sub_ek2.sub(sinI2);
	const math::Spectrum a2plusb2 = t0.mul(t0).addLocal(m_4_mul_en2_mul_ek2).sqrtLocal();
	const math::Spectrum a        = a2plusb2.add(t0).mulLocal(0.5_r).sqrtLocal();
	const math::Spectrum t1       = a2plusb2.add(cosI2);
	const math::Spectrum t2       = a.mul(2.0_r * cosI);
	const math::Spectrum t3       = a2plusb2.mul(cosI2).addLocal(sinI2 * sinI2);
	const math::Spectrum t4       = t2.mul(sinI2);

	const math::Spectrum Rs = t1.sub(t2).divLocal(t1.add(t2));
	const math::Spectrum Rp = Rs.mul(t3.sub(t4).divLocal(t3.add(t4)));
	*out_reflectance = Rs.add(Rp).mulLocal(0.5_r);
}

void ExactConductorFresnel::setIors(
	const real            iorOuter,
	const math::Spectrum& iorInnerN,
	const math::Spectrum& iorInnerK)
{
	m_iorOuter  = iorOuter;
	m_iorInnerN = iorInnerN;
	m_iorInnerK = iorInnerK;

	const math::Spectrum en2 = iorInnerN.div(iorOuter).pow(2);
	const math::Spectrum ek2 = iorInnerK.div(iorOuter).pow(2);
	m_en2_sub_ek2       = en2.sub(ek2);
	m_4_mul_en2_mul_ek2 = en2.mul(ek2).mul(4);
}

}// end namespace ph
