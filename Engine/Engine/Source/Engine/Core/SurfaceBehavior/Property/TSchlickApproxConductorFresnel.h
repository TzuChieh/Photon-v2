#pragma once

#include "Engine/Core/SurfaceBehavior/Property/ConductorFresnel.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"

#include <Common/compiler.h>

#include <cmath>
#include <utility>

namespace ph
{

/*! @brief Conductor-dielectric interface Fresnel effect.

The formulae used are approximations made by Schlick @cite Schlick:1994:BRDF.
*/
template<typename F0>
class TSchlickApproxConductorFresnel : public ConductorFresnel
{
	static_assert(CSurfaceProperty<F0, math::Spectrum>,
		"`F0` must accept `SurfaceHit` and return a spectrum-convertible value.");

public:
	explicit TSchlickApproxConductorFresnel(F0 f0)
		: m_f0(std::move(f0))
	{}

	math::Spectrum calcReflectance(
		const SurfaceHit& X,
		const real        cosThetaIncident) const override
	{
		// We treat the incident light be always in the dielectric side (which is
		// reasonable since light should not penetrate conductors easily), so the
		// sign of cosI does not matter here.
		const real oneMinusCosI  = 1.0_r - std::abs(cosThetaIncident);
		const real oneMinusCosI2 = oneMinusCosI * oneMinusCosI;
		const real oneMinusCosI5 = oneMinusCosI2 * oneMinusCosI2 * oneMinusCosI;
		const math::Spectrum f0  = m_f0(X);

		return f0.complement().mul(oneMinusCosI5).add(f0);
	}

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	F0 m_f0;
};

template<>
class TSchlickApproxConductorFresnel<TConstantSurfaceProperty<math::Spectrum>> : public ConductorFresnel
{
public:
	explicit TSchlickApproxConductorFresnel(TConstantSurfaceProperty<math::Spectrum> f0)
		: TSchlickApproxConductorFresnel(f0.constant)
	{}

	TSchlickApproxConductorFresnel(
		const real            iorOuter,
		const math::Spectrum& iorInnerN,
		const math::Spectrum& iorInnerK)

		: TSchlickApproxConductorFresnel(calcF0(iorOuter, iorInnerN, iorInnerK))
	{}

	explicit TSchlickApproxConductorFresnel(const math::Spectrum& f0)

		: ConductorFresnel()

		, m_f0(f0)
		, m_f0Complement(f0.complement())
	{}

	math::Spectrum calcReflectance(
		const SurfaceHit& /* X */,
		const real        cosThetaIncident) const override
	{
		// We treat the incident light be always in the dielectric side (which is
		// reasonable since light should not penetrate conductors easily), so the
		// sign of `cosThetaIncident` does not matter here.
		const real oneMinusCosI  = 1.0_r - std::abs(cosThetaIncident);
		const real oneMinusCosI2 = oneMinusCosI * oneMinusCosI;
		const real oneMinusCosI5 = oneMinusCosI2 * oneMinusCosI2 * oneMinusCosI;

		return m_f0Complement.mul(oneMinusCosI5).add(m_f0);
	}

private:
	static math::Spectrum calcF0(
		const real            iorOuter,
		const math::Spectrum& iorInnerN,
		const math::Spectrum& iorInnerK)
	{
		const math::Spectrum neg2 = iorInnerN.sub(math::Spectrum(iorOuter)).pow(2);
		const math::Spectrum pos2 = iorInnerN.add(math::Spectrum(iorOuter)).pow(2);
		const math::Spectrum nume = neg2.add(iorInnerK.pow(2));
		const math::Spectrum deno = pos2.add(iorInnerK.pow(2));
		return nume.div(deno);
	}

	math::Spectrum m_f0;
	math::Spectrum m_f0Complement;
};

}// end namespace ph
