#pragma once

#include "Engine/Math/Color/spectrum_fwd.h"

#include <Common/primitive_type.h>

namespace ph
{

class SurfaceHit;

class MediumCoefficient
{
public:
	virtual ~MediumCoefficient() = 0;

	virtual math::Spectrum eval(const SurfaceHit& X) const = 0;
};

}// end namespace ph
