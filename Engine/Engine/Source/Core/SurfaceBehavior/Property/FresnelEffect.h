#pragma once

#include "Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Modeling Fresnel effects that occur on interfaces.

The two sides of a interface are called inner and outer sides, where outer side
has @f$ \cos(\theta_{indicent})>0 @f$ and inner side has @f$ \cos(\theta_{indicent})<0 @f$.
*/
class FresnelEffect
{
public:
	virtual ~FresnelEffect() = default;

	/*!
	@param cosThetaIncident Cosine of the incident angle.
	@return The reflectance of the interface on the specified angle.
	@note `cosThetaIncident` is signed, so inner and outer sides can be determined.
	*/
	virtual math::Spectrum calcReflectance(real cosThetaIncident) const = 0;

	/*!
	@param cosThetaIncident Cosine of the incident angle.
	@return The transmittance of the interface on the specified angle.
	@note `cosThetaIncident` is signed, so inner and outer sides can be determined.
	*/
	math::Spectrum calcTransmittance(real cosThetaIncident) const;
};

inline math::Spectrum FresnelEffect::calcTransmittance(const real cosThetaIncident) const
{
	return calcReflectance(cosThetaIncident).complement();
}

}// end namespace ph
