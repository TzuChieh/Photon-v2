#pragma once

#include "Common/primitive_type.h"
#include "Math/Color/Spectrum.h"

namespace ph
{

class SurfaceHit;

class BlockFunction
{
public:
	virtual ~BlockFunction() = 0;

	virtual void evalAbsorptionCoeff(const SurfaceHit& X, math::Spectrum* out_coeff) const = 0;
};

}// end namespace ph
