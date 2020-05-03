#pragma once

#include "Core/SurfaceBehavior/Property/FresnelEffect.h"

namespace ph
{

/*! @brief Dielectric-dielectric interface Fresnel effect.

Fresnel effect where both inner and outer sides are dielectric.
*/
class DielectricFresnel : public FresnelEffect
{
public:
	DielectricFresnel(real iorOuter, real iorInner);

	// TODO: add a dielectric specific form (not spectral)
	void calcReflectance(real cosThetaIncident, Spectrum* out_reflectance) const override = 0;

	bool calcRefractDir(const math::Vector3R& I, const math::Vector3R& N, math::Vector3R* out_refractDir) const;

	inline real getIorOuter() const
	{
		return m_iorOuter;
	}

	inline real getIorInner() const
	{
		return m_iorInner;
	}

protected:
	real m_iorOuter;
	real m_iorInner;
};

}// end namespace ph
