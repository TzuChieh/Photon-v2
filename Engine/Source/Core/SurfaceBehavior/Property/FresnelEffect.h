#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector3.h"

namespace ph
{

/*! @brief Modeling Fresnel effects that occur on interfaces.

The two sides of a interface are called inner and outer sides, where outer side
has $\cos(\theta_{indicent})>0$ and inner side has $\cos(\theta_{indicent})<0$.
*/
class FresnelEffect
{
public:
	virtual ~FresnelEffect() = default;

	virtual void calcReflectance(real cosThetaIncident, SpectralStrength* out_reflectance) const = 0;

	inline void calcTransmittance(real cosThetaIncident, SpectralStrength* out_transmittance) const
	{
		calcReflectance(cosThetaIncident, out_transmittance);
		out_transmittance->complementLocal();
	}
};

}// end namespace ph
