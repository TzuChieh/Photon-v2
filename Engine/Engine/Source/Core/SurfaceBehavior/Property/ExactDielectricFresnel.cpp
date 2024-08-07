#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"

#include <Common/assertion.h>

namespace ph
{

ExactDielectricFresnel::ExactDielectricFresnel(const real iorOuter, const real iorInner)
	: DielectricFresnel(iorOuter, iorInner)
{}

math::Spectrum ExactDielectricFresnel::calcReflectance(const real cosThetaIncident) const
{
	real etaI = m_iorOuter;
	real etaT = m_iorInner;

	// Handles the case where incident light is from inner to outer
	real cosI = cosThetaIncident;
	if(cosThetaIncident < 0.0_r)
	{
		std::swap(etaI, etaT);
		cosI = std::abs(cosI);
	}

	const real etaRatio = etaI / etaT;
	const real sinT2    = (1.0_r - cosI * cosI) * etaRatio * etaRatio;

	// Handles TIR
	if(sinT2 >= 1.0_r)
	{
		return math::Spectrum(1);
	}

	const real cosT = std::sqrt(1.0_r - sinT2);

	const real rParallel      = (etaT * cosI - etaI * cosT) / (etaT * cosI + etaI * cosT);
	const real rPerpendicular = (etaI * cosI - etaT * cosT) / (etaI * cosI + etaT * cosT);
	const real rSum           = 0.5_r * (rParallel * rParallel + rPerpendicular * rPerpendicular);

	return math::Spectrum(rSum);
}

}// end namespace ph
