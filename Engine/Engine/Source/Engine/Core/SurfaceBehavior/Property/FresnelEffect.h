#pragma once

#include "Engine/Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

class SurfaceHit;

/*! @brief Modeling Fresnel effects that occur on interfaces.

The two sides of a interface are called inner and outer sides, where outer side
has @f$ \cos(\theta_{indicent})>0 @f$ and inner side has @f$ \cos(\theta_{indicent})<0 @f$.
*/
class FresnelEffect
{
public:
	virtual ~FresnelEffect() = default;

	/*!
	@param X The surface hit to evaluate the interface at.
	@param cosThetaIncident Cosine of the incident angle.
	@return The reflectance of the interface on the specified angle.
	@note `cosThetaIncident` is signed, so inner and outer sides can be determined.
	*/
	virtual math::Spectrum calcReflectance(
		const SurfaceHit& X,
		real cosThetaIncident) const = 0;

	/*!
	@param X The surface hit to evaluate the interface at.
	@param cosThetaIncident Cosine of the incident angle.
	@return The transmittance of the interface on the specified angle.
	@note `cosThetaIncident` is signed, so inner and outer sides can be determined.
	*/
	math::Spectrum calcTransmittance(
		const SurfaceHit& X,
		real cosThetaIncident) const;
};

inline math::Spectrum FresnelEffect::calcTransmittance(
	const SurfaceHit& X,
	const real cosThetaIncident) const
{
	return calcReflectance(X, cosThetaIncident).complement();
}

}// end namespace ph
