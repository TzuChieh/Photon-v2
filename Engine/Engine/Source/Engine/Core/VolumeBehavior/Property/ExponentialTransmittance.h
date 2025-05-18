#pragma once

#include "Engine/Core/VolumeBehavior/Property/TransmittanceFunction.h"
#include "Engine/Math/Color/Spectrum.h"

namespace ph
{

class ExponentialTransmittance : public TransmittanceFunction
{
public:
	inline ExponentialTransmittance()
		: TransmittanceFunction()
	{}
	
	inline math::Spectrum eval(const math::Spectrum& opticalDepth) const override
	{
		return math::Spectrum().exp(-opticalDepth);
	}
};

}// end namespace ph
