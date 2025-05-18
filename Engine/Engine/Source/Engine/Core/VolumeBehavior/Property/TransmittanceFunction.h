#pragma once

#include "Engine/Math/Color/spectrum_fwd.h"

namespace ph
{

class TransmittanceFunction
{
public:
	virtual ~TransmittanceFunction();

	virtual math::Spectrum eval(const math::Spectrum& opticalDepth) const = 0;
};

}// end namespace ph