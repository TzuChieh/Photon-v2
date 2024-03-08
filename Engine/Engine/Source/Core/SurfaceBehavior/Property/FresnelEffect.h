#pragma once

#include "Math/Color/Spectrum.h"
#include "Math/TVector3.h"

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

	virtual void calcReflectance(real cosThetaIncident, math::Spectrum* out_reflectance) const = 0;

	void calcTransmittance(real cosThetaIncident, math::Spectrum* out_transmittance) const;
};

inline void FresnelEffect::calcTransmittance(
	const real cosThetaIncident,
	math::Spectrum* const out_transmittance) const
{
	calcReflectance(cosThetaIncident, out_transmittance);
	out_transmittance->complementLocal();
}

}// end namespace ph
