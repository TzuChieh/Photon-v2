#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph
{

SchlickApproxDielectricFresnel::SchlickApproxDielectricFresnel(
	const real iorOuter, 
	const real iorInner)

	: DielectricFresnel(iorOuter, iorInner)

	, m_f0{}
	, m_tirIorRatio2{}
{
	const double numi = std::pow(iorOuter - iorInner, 2);
	const double deno = std::pow(iorOuter + iorInner, 2);
	m_f0 = static_cast<real>(numi / deno);

	const auto tirIorRatio = std::max(iorOuter, iorInner) / std::min(iorOuter, iorInner);
	m_tirIorRatio2 = tirIorRatio * tirIorRatio;
}

void SchlickApproxDielectricFresnel::calcReflectance(
	const real            cosThetaIncident,
	math::Spectrum* const out_reflectance) const
{
	PH_ASSERT(out_reflectance);

	real cosTheta = std::abs(cosThetaIncident);

	// Schlick's approximation erroneously ignores the critical angle during calculation.
	// To fix this, we can use the refraction angle instead when TIR is possible (i.e., the
	// situation where the IoR of the incident side is greater than the opposite side) [1].
	if((cosThetaIncident > 0 && m_iorOuter > m_iorInner) ||
	   (cosThetaIncident < 0 && m_iorInner > m_iorOuter))
	{
		const real sinT2 = (1.0_r - cosThetaIncident * cosThetaIncident) * m_tirIorRatio2;

		// Handles TIR
		if(sinT2 >= 1.0_r)
		{
			out_reflectance->setColorValues(1.0_r);
			return;
		}

		cosTheta = std::sqrt(1.0_r - sinT2);
	}

	out_reflectance->setColorValues(
		m_f0 + (1.0_r - m_f0) * static_cast<real>(std::pow(1.0_r - cosTheta, 5)));
}

}// end namespace ph

/* References:
[1] Brent Burley, "Extending the Disney BRDF to a BSDF with Integrated Subsurface Scattering",
SIGGRAPH 2015 Course
*/
