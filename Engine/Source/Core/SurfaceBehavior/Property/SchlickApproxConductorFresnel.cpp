#include "Core/SurfaceBehavior/Property/SchlickApproxConductorFresnel.h"
#include "Common/assertion.h"

namespace ph
{

SchlickApproxConductorFresnel::SchlickApproxConductorFresnel(
	const real      iorOuter,
	const Spectrum& iorInnerN,
	const Spectrum& iorInnerK) :

	ConductorFresnel(iorOuter, iorInnerN, iorInnerK)
{
	const Spectrum neg2 = iorInnerN.sub(Spectrum(iorOuter)).pow(2);
	const Spectrum pos2 = iorInnerN.add(Spectrum(iorOuter)).pow(2);
	const Spectrum nume = neg2.add(iorInnerK.pow(2));
	const Spectrum deno = pos2.add(iorInnerK.pow(2));
	m_f0           = nume.div(deno);
	m_f0Complement = m_f0.complement();
}

SchlickApproxConductorFresnel::SchlickApproxConductorFresnel(
	const Spectrum& f0) : 

	// FIXME: this might cause problems if the class is used polymorphically
	// actual IoR values are not needed by Schlick's approximation during runtime
	ConductorFresnel(1, Spectrum(1), Spectrum(1)),

	m_f0(f0), m_f0Complement(f0.complement())
{}

void SchlickApproxConductorFresnel::calcReflectance(
	const real      cosThetaIncident,
	Spectrum* const out_reflectance) const
{
	PH_ASSERT(out_reflectance);

	// We treat the incident light be always in the dielectric side (which is
	// reasonable since light should not penetrate conductors easily), so the 
	// sign of cosI does not matter here.
	const real cosI = std::abs(cosThetaIncident);

	out_reflectance->setValues(m_f0Complement.mul(std::pow(1.0_r - cosI, 5)).addLocal(m_f0));
}

}// end namespace ph
