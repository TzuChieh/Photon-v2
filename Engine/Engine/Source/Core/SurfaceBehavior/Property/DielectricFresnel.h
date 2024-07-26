#pragma once

#include "Core/SurfaceBehavior/Property/FresnelEffect.h"
#include "Math/TVector3.h"

#include <optional>

namespace ph
{

/*! @brief Dielectric-dielectric interface Fresnel effect.

Fresnel effect where both inner and outer sides are dielectric.
*/
class DielectricFresnel : public FresnelEffect
{
public:
	DielectricFresnel(real iorOuter, real iorInner);

	math::Spectrum calcReflectance(real cosThetaIncident) const override = 0;

	/*! @brief Calculates the normalized refraction direction.
	@param I The normalized incident direction. Pointing away from the interface.
	@param N The normalized normal. Pointing to the outer side of the interface.
	@return The normalized refraction direction. May be empty if refraction is not possible,
	e.g., due to TIR.
	*/
	std::optional<math::Vector3R> calcRefractDir(
		const math::Vector3R& I,
		const math::Vector3R& N) const;

	/*! @brief Calculates the cosine (signed) of refraction direction.
	This method is similar to `calcRefractDir()`, but is slightly more efficient as it returns the
	cosine of refraction direction only.
	@param I The normalized incident direction. Pointing away from the interface.
	@param N The normalized normal. Pointing to the outer side of the interface.
	@return The cosine of the angle between refraction direction and `N`. May be empty if refraction
	is not possible, e.g., due to TIR.
	*/
	std::optional<real> calcRefractCos(
		const math::Vector3R& I,
		const math::Vector3R& N) const;

	real getIorOuter() const;
	real getIorInner() const;

protected:
	real m_iorOuter;
	real m_iorInner;
};

inline real DielectricFresnel::getIorOuter() const
{
	return m_iorOuter;
}

inline real DielectricFresnel::getIorInner() const
{
	return m_iorInner;
}

}// end namespace ph
