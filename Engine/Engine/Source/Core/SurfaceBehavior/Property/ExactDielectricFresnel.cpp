#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"

#include <Common/assertion.h>

namespace ph
{

ExactDielectricFresnel::ExactDielectricFresnel(const real iorOuter, const real iorInner)
	: DielectricFresnel(iorOuter, iorInner)
{}

void ExactDielectricFresnel::calcReflectance(
	const real            cosThetaIncident,
	math::Spectrum* const out_reflectance) const
{
	PH_ASSERT(out_reflectance);

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
		out_reflectance->setColorValues(1.0_r);
		return;
	}

	const real cosT = std::sqrt(1.0_r - sinT2);

	const real rParallel      = (etaT * cosI - etaI * cosT) / (etaT * cosI + etaI * cosT);
	const real rPerpendicular = (etaI * cosI - etaT * cosT) / (etaI * cosI + etaT * cosT);
	const real rSum           = 0.5_r * (rParallel * rParallel + rPerpendicular * rPerpendicular);

	out_reflectance->setColorValues(rSum);
}

}// end namespace ph
