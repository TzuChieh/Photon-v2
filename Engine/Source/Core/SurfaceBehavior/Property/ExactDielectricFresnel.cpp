#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"

namespace ph
{

ExactDielectricFresnel::ExactDielectricFresnel(const real iorOuter, const real iorInner) :
	DielectricFresnel(iorOuter, iorInner)
{

}

ExactDielectricFresnel::~ExactDielectricFresnel() = default;

void ExactDielectricFresnel::calcReflectance(const real cosThetaIncident,
                                             SpectralStrength* const out_reflectance) const
{
	real etaI = m_iorOuter;
	real etaT = m_iorInner;

	// handles the case where incident light is from inner to outer
	real cosI = cosThetaIncident;
	if(cosThetaIncident < 0.0_r)
	{
		std::swap(etaI, etaT);
		cosI = std::abs(cosI);
	}

	const real etaRatio = etaI / etaT;
	const real sinT2    = (1.0_r - cosI * cosI) * etaRatio * etaRatio;

	// handles TIR
	if(sinT2 >= 1.0_r)
	{
		out_reflectance->setValues(1.0_r);
		return;
	}

	const real cosT = std::sqrt(1.0_r - sinT2);

	const real rParallel      = (etaT * cosI - etaI * cosT) / (etaT * cosI + etaI * cosT);
	const real rPerpendicular = (etaI * cosI - etaT * cosT) / (etaI * cosI + etaT * cosT);
	const real rSum           = 0.5_r * (rParallel * rParallel + rPerpendicular * rPerpendicular);

	out_reflectance->setValues(rSum);
}

}// end namespace ph